#pragma once

#include <QDialog>

QT_BEGIN_NAMESPACE
class QPlainTextEdit;
class QPushButton;
QT_END_NAMESPACE

class ConfigEditorDialog : public QDialog
{
    Q_OBJECT
public:
    // initialContent may be empty; if filePathOut is set in OK, dialog can return it via filePath()
    explicit ConfigEditorDialog(const QString &initialContent = QString(), QWidget *parent = nullptr);

    QString content() const;
    QString filePath() const;

private slots:
    void onValidate();
    void onSaveAs();

private:
    QPlainTextEdit *m_editor;
    QPushButton *m_validateBtn;
    QPushButton *m_saveBtn;
    QString m_filePath;
};
