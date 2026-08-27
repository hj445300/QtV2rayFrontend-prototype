#pragma once

#include <QMainWindow>
#include <QProcess>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QPushButton;
class QTextEdit;
class QSystemTrayIcon;
class QAction;
class QListWidget;
class QNetworkAccessManager;
QT_END_NAMESPACE

class ChartWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void chooseV2ray();
    void chooseConfig();
    void startV2ray();
    void stopV2ray();
    void readStdOutput();
    void readStdError();
    void processFinished(int exitCode, QProcess::ExitStatus status);
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
    void showWindow();
    void quitApp();

    // new features
    void importSubscription();
    void onSubscriptionDownloaded();
    void onSubscriptionDownloadError();

    void editConfig();
    void onNodeSelected();

private:
    void appendLog(const QString &text);
    void parseStatsFromLine(const QString &line);
    void addNodeToList(const QString &displayName, const QString &raw);

    QLineEdit *m_v2rayPathEdit;
    QLineEdit *m_configPathEdit;
    QPushButton *m_startBtn;
    QPushButton *m_stopBtn;
    QPushButton *m_subscribeBtn;
    QPushButton *m_editConfigBtn;
    QTextEdit *m_logView;

    QListWidget *m_nodeList;

    QProcess *m_process;
    QSystemTrayIcon *m_trayIcon;
    QAction *m_actionShow;
    QAction *m_actionStart;
    QAction *m_actionStop;
    QAction *m_actionQuit;

    QNetworkAccessManager *m_netManager;
    ChartWidget *m_chart;
};
