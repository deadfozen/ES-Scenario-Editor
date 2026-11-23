#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

class AssetPreview;
class QFileSystemModel;
class QTreeView;
class QLabel;
class QModelIndex;
class CodeEditor;
class CreateProjectDialog;
class SettingsDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void showDocs();
    void openProject();
    bool saveFile();
    void saveFileAs();
    void createProject();
    void showSetting();

private:
    void setupCentralWidget();
    void openFile(const QString &filePath);
    void setProjectDirectory(const QString &directory);

    AssetPreview *m_assetPreview = nullptr;

    QFileSystemModel *m_fileModel = nullptr;
    QTreeView *m_fileTree = nullptr;
    QLabel *m_hintLabel = nullptr;
    QLabel *m_statusLabel = nullptr;

    CodeEditor *m_codeEditor = nullptr;
    QLabel *m_fileLabel = nullptr;

    QString m_projectPath;
    QString m_currentFilePath;
};

#endif