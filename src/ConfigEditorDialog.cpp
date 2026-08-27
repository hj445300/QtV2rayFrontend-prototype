#include "ConfigEditorDialog.h"
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QFileDialog>
#include <QMessageBox>

ConfigEditorDialog::ConfigEditorDialog(const QString &initialContent, QWidget *parent)
    : QDialog(parent), m_filePath()
{
    setWindowTitle("Config Editor");
    resize(700, 500);
    auto v = new QVBoxLayout(this);
    m_editor = new QPlainTextEdit();
    m_editor->setPlainText(initialContent);
    v->addWidget(m_editor, 1);

    auto h = new QHBoxLayout();
    m_validateBtn = new QPushButton("Validate JSON");
    m_saveBtn = new QPushButton("Save As...");
    auto closeBtn = new QPushButton("Close");
    h->addWidget(m_validateBtn);
    h->addWidget(m_saveBtn);
    h->addStretch();
    h->addWidget(closeBtn);
    v->addLayout(h);

    connect(m_validateBtn, &QPushButton::clicked, this, &ConfigEditorDialog::onValidate);
    connect(m_saveBtn, &QPushButton::clicked, this, &ConfigEditorDialog::onSaveAs);
    connect(closeBtn, &QPushButton::clicked, this, &ConfigEditorDialog::accept);
}

QString ConfigEditorDialog::content() const
{
    return m_editor->toPlainText();
}

QString ConfigEditorDialog::filePath() const
{
    return m_filePath;
}

void ConfigEditorDialog::onValidate()
{
    QByteArray raw = m_editor->toPlainText().toUtf8();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(raw, &error);
    if (error.error == QJsonParseError::NoError) {
        QMessageBox::information(this, "Valid", "JSON is valid.");
    } else {
        QMessageBox::warning(this, "Invalid JSON", error.errorString());
    }
}

void ConfigEditorDialog::onSaveAs()
{
    QString path = QFileDialog::getSaveFileName(this, "Save config as", "config.json", "JSON files (*.json);;All files (*)");
    if (path.isEmpty()) return;
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Error", "Cannot write file: " + path);
        return;
    }
    QByteArray raw = m_editor->toPlainText().toUtf8();
    f.write(raw);
    f.close();
    m_filePath = path;
    QMessageBox::information(this, "Saved", "Saved config to: " + path);
}
