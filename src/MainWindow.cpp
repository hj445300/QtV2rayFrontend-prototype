#include "MainWindow.h"
#include "ConfigEditorDialog.h"
#include "ChartWidget.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QIcon>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QFileInfo>
#include <QDateTime>
#include <QListWidget>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <QBuffer>
#include <QRegularExpression>
#include <QCryptographicHash>
#include <QUrl>

#include <QJsonParseError>
#include <QJsonArray>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_process(new QProcess(this)),
      m_netManager(new QNetworkAccessManager(this)),
      m_chart(new ChartWidget(this))
{
    setWindowTitle("Qt V2Ray Frontend (Extended)");
    auto central = new QWidget(this);
    auto mainLayout = new QHBoxLayout(central);

    // Left column: nodes and controls
    auto leftWidget = new QWidget(this);
    auto leftLayout = new QVBoxLayout(leftWidget);

    auto h1 = new QHBoxLayout();
    h1->addWidget(new QLabel("v2ray exe:"));
    m_v2rayPathEdit = new QLineEdit();
    h1->addWidget(m_v2rayPathEdit);
    auto browseV = new QPushButton("Browse");
    h1->addWidget(browseV);

    leftLayout->addLayout(h1);

    auto h2 = new QHBoxLayout();
    h2->addWidget(new QLabel("config.json:"));
    m_configPathEdit = new QLineEdit();
    h2->addWidget(m_configPathEdit);
    auto browseC = new QPushButton("Browse");
    h2->addWidget(browseC);
    leftLayout->addLayout(h2);

    m_subscribeBtn = new QPushButton("Import subscription");
    leftLayout->addWidget(m_subscribeBtn);

    m_nodeList = new QListWidget();
    leftLayout->addWidget(new QLabel("Nodes:"));
    leftLayout->addWidget(m_nodeList, 1);

    m_editConfigBtn = new QPushButton("Edit config");
    leftLayout->addWidget(m_editConfigBtn);

    auto h3 = new QHBoxLayout();
    m_startBtn = new QPushButton("Start");
    m_stopBtn = new QPushButton("Stop");
    m_stopBtn->setEnabled(false);
    h3->addWidget(m_startBtn);
    h3->addWidget(m_stopBtn);
    leftLayout->addLayout(h3);

    mainLayout->addWidget(leftWidget, 1);

    // Right column: logs + chart
    auto rightWidget = new QWidget(this);
    auto rightLayout = new QVBoxLayout(rightWidget);

    m_logView = new QTextEdit();
    m_logView->setReadOnly(true);
    rightLayout->addWidget(new QLabel("v2ray logs:"));
    rightLayout->addWidget(m_logView, 2);

    rightLayout->addWidget(new QLabel("Realtime stats:"));
    rightLayout->addWidget(m_chart, 1);

    mainLayout->addWidget(rightWidget, 2);

    setCentralWidget(central);
    resize(1000, 600);

    // tray
    m_trayIcon = new QSystemTrayIcon(QIcon(":/tray/tray.xpm"), this);
    auto trayMenu = new QMenu(this);
    m_actionShow = trayMenu->addAction("Show", this, SLOT(showWindow()));
    m_actionStart = trayMenu->addAction("Start v2ray", this, SLOT(startV2ray()));
    m_actionStop = trayMenu->addAction("Stop v2ray", this, SLOT(stopV2ray()));
    trayMenu->addSeparator();
    m_actionQuit = trayMenu->addAction("Quit", this, SLOT(quitApp()));
    m_trayIcon->setContextMenu(trayMenu);
    m_trayIcon->show();

    // connections
    connect(browseV, &QPushButton::clicked, this, &MainWindow::chooseV2ray);
    connect(browseC, &QPushButton::clicked, this, &MainWindow::chooseConfig);
    connect(m_startBtn, &QPushButton::clicked, this, &MainWindow::startV2ray);
    connect(m_stopBtn, &QPushButton::clicked, this, &MainWindow::stopV2ray);

    connect(m_process, &QProcess::readyReadStandardOutput, this, &MainWindow::readStdOutput);
    connect(m_process, &QProcess::readyReadStandardError, this, &MainWindow::readStdError);
    connect(m_process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &MainWindow::processFinished);
    connect(m_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::trayActivated);

    connect(m_subscribeBtn, &QPushButton::clicked, this, &MainWindow::importSubscription);
    connect(m_editConfigBtn, &QPushButton::clicked, this, &MainWindow::editConfig);
    connect(m_nodeList, &QListWidget::itemDoubleClicked, this, &MainWindow::onNodeSelected);
}

MainWindow::~MainWindow()
{
    if (m_process->state() != QProcess::NotRunning) {
        m_process->kill();
        m_process->waitForFinished(3000);
    }
}

void MainWindow::chooseV2ray()
{
    QString path = QFileDialog::getOpenFileName(this, "Select v2ray executable");
    if (!path.isEmpty()) m_v2rayPathEdit->setText(path);
}

void MainWindow::chooseConfig()
{
    QString path = QFileDialog::getOpenFileName(this, "Select config.json");
    if (!path.isEmpty()) m_configPathEdit->setText(path);
}

void MainWindow::startV2ray()
{
    QString prog = m_v2rayPathEdit->text().trimmed();
    QString cfg = m_configPathEdit->text().trimmed();

    if (prog.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please specify v2ray executable path.");
        return;
    }
    if (!QFileInfo::exists(prog)) {
        QMessageBox::warning(this, "Error", "v2ray path not found.");
        return;
    }

    QStringList args;
    if (!cfg.isEmpty()) {
        args << "-config" << cfg;
    }

    appendLog(QString("[%1] Starting v2ray: %2 %3")
              .arg(QDateTime::currentDateTime().toString())
              .arg(prog).arg(args.join(' ')));

    m_process->start(prog, args);
    if (!m_process->waitForStarted(3000)) {
        appendLog("[ERROR] Failed to start v2ray.");
        return;
    }

    m_startBtn->setEnabled(false);
    m_stopBtn->setEnabled(true);
    m_actionStart->setEnabled(false);
    m_actionStop->setEnabled(true);
}

void MainWindow::stopV2ray()
{
    if (m_process->state() == QProcess::Running) {
        m_process->terminate();
        if (!m_process->waitForFinished(3000)) {
            m_process->kill();
        }
    }
}

void MainWindow::readStdOutput()
{
    QByteArray data = m_process->readAllStandardOutput();
    QString text = QString::fromUtf8(data);
    appendLog(text);

    // parse lines
    for (const QString &line : text.split('\n', Qt::SkipEmptyParts)) {
        parseStatsFromLine(line);
    }
}

void MainWindow::readStdError()
{
    QByteArray data = m_process->readAllStandardError();
    QString text = QString::fromUtf8(data);
    appendLog(text);

    for (const QString &line : text.split('\n', Qt::SkipEmptyParts)) {
        parseStatsFromLine(line);
    }
}

void MainWindow::processFinished(int exitCode, QProcess::ExitStatus status)
{
    Q_UNUSED(status)
    appendLog(QString("[%1] v2ray exited with code %2")
              .arg(QDateTime::currentDateTime().toString())
              .arg(exitCode));
    m_startBtn->setEnabled(true);
    m_stopBtn->setEnabled(false);
    m_actionStart->setEnabled(true);
    m_actionStop->setEnabled(false);
}

void MainWindow::appendLog(const QString &text)
{
    m_logView->append(text);
}

void MainWindow::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        if (isVisible()) hide();
        else show();
    }
}

void MainWindow::showWindow()
{
    show();
    raise();
    activateWindow();
}

void MainWindow::quitApp()
{
    qApp->quit();
}

// ---- subscription import ----
static QByteArray base64UrlDecode(const QByteArray &in) {
    QByteArray s = in;
    s = s.replace('-', '+').replace('_', '/');
    while (s.size() % 4) s.append('=');
    return QByteArray::fromBase64(s);
}

void MainWindow::importSubscription()
{
    bool ok = false;
    QString url = QInputDialog::getText(this, "Subscription URL", "Enter subscription URL:", QLineEdit::Normal, "", &ok);
    if (!ok || url.isEmpty()) return;

    QUrl qurl(url);
    if (!qurl.isValid()) {
        QMessageBox::warning(this, "Invalid URL", "The subscription URL is invalid.");
        return;
    }

    appendLog(QString("[%1] Downloading subscription: %2").arg(QDateTime::currentDateTime().toString()).arg(url));
    QNetworkRequest req(qurl);
    QNetworkReply *reply = m_netManager->get(req);
    connect(reply, &QNetworkReply::finished, this, &MainWindow::onSubscriptionDownloaded);
    connect(reply, &QNetworkReply::errorOccurred, this, &MainWindow::onSubscriptionDownloadError);
}

void MainWindow::onSubscriptionDownloaded()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    QByteArray data = reply->readAll();
    reply->deleteLater();

    // subscription often base64-encoded list
    QByteArray maybe = data.trimmed();
    QByteArray decoded = QByteArray::fromBase64(maybe);
    if (decoded.size() > 0) {
        maybe = decoded;
    }

    QString content = QString::fromUtf8(maybe);

    int added = 0;
    // split by lines and parse vmess:// base64 or plain items
    const QStringList tokens = content.split(QRegExp("\\s+"), Qt::SkipEmptyParts);
    for (const QString &t : tokens) {
        if (t.startsWith("vmess://")) {
            QByteArray b = t.mid(QString("vmess://").length()).toUtf8();
            QByteArray jb = base64UrlDecode(b);
            QJsonParseError err;
            QJsonDocument doc = QJsonDocument::fromJson(jb, &err);
            if (err.error == QJsonParseError::NoError && doc.isObject()) {
                QJsonObject obj = doc.object();
                QString ps = obj.value("ps").toString();
                QString add = obj.value("add").toString();
                QString disp = ps.isEmpty() ? add : ps;
                addNodeToList(disp, t);
                ++added;
                continue;
            }
            // fallback try plain base64 decode
            QByteArray jb2 = QByteArray::fromBase64(b);
            QJsonDocument doc2 = QJsonDocument::fromJson(jb2, &err);
            if (err.error == QJsonParseError::NoError && doc2.isObject()) {
                QJsonObject obj = doc2.object();
                QString ps = obj.value("ps").toString();
                QString add = obj.value("add").toString();
                QString disp = ps.isEmpty() ? add : ps;
                addNodeToList(disp, t);
                ++added;
                continue;
            }
        } else {
            // add raw line
            addNodeToList(t.left(60), t);
            ++added;
        }
    }

    appendLog(QString("[%1] Subscription parsed, %2 nodes added.").arg(QDateTime::currentDateTime().toString()).arg(added));
}

void MainWindow::onSubscriptionDownloadError()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    appendLog(QString("[ERROR] Subscription download failed: %1").arg(reply->errorString()));
    reply->deleteLater();
}

void MainWindow::addNodeToList(const QString &displayName, const QString &raw)
{
    QListWidgetItem *it = new QListWidgetItem(displayName, m_nodeList);
    it->setData(Qt::UserRole, raw);
}

// ---- config editor ----
void MainWindow::editConfig()
{
    QString cfgPath = m_configPathEdit->text().trimmed();
    if (cfgPath.isEmpty()) {
        // if no config specified, open empty editor
        ConfigEditorDialog dlg("", this);
        if (dlg.exec() == QDialog::Accepted) {
            QString saved = dlg.filePath();
            if (!saved.isEmpty()) m_configPathEdit->setText(saved);
        }
        return;
    }
    QFile f(cfgPath);
    if (!f.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Error", "Cannot open config: " + cfgPath);
        return;
    }
    QByteArray content = f.readAll();
    f.close();
    ConfigEditorDialog dlg(QString::fromUtf8(content), this);
    if (dlg.exec() == QDialog::Accepted) {
        QString newContent = dlg.content();
        QString savePath = dlg.filePath();
        if (savePath.isEmpty()) savePath = cfgPath;
        QFile out(savePath);
        if (!out.open(QIODevice::WriteOnly)) {
            QMessageBox::warning(this, "Error", "Cannot write config: " + savePath);
        } else {
            out.write(newContent.toUtf8());
            out.close();
            appendLog(QString("[%1] Saved config to %2").arg(QDateTime::currentDateTime().toString()).arg(savePath));
            m_configPathEdit->setText(savePath);
        }
    }
}

void MainWindow::onNodeSelected()
{
    QListWidgetItem *it = m_nodeList->currentItem();
    if (!it) return;
    QString raw = it->data(Qt::UserRole).toString();
    // For simplicity, when node double-clicked, put raw into config path edit (user can edit)
    m_configPathEdit->setText(raw);
}

// ---- simple stats parser ----
void MainWindow::parseStatsFromLine(const QString &line)
{
    // try to find patterns like "in: 12345" or "out: 6789" or "... bytes: 1234"
    static QRegularExpression reWords("(?:in|out|recv|sent|bytes?)[:=]?\\s*(\\d+)", QRegularExpression::CaseInsensitiveOption);
    auto m = reWords.globalMatch(line);
    bool found = false;
    while (m.hasNext()) {
        auto match = m.next();
        bool ok = false;
        qint64 v = match.captured(1).toLongLong(&ok);
        if (ok) {
            m_chart->appendSample(int(v));
            found = true;
        }
    }

    if (!found) {
        // fallback: pick the first integer in the line
        static QRegularExpression reNum("(\\d{2,})"); // at least 2 digits to avoid timestamps
        auto mm = reNum.match(line);
        if (mm.hasMatch()) {
            bool ok = false;
            qint64 v = mm.captured(1).toLongLong(&ok);
            if (ok) m_chart->appendSample(int(v));
        }
    }
}
