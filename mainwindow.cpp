#include "mainwindow.h"
#include "menubar.h"
#include "menuactions.h"
#include "docsdialog.h"
#include "assetpreview.h"
#include "editor/codeeditor.h"
#include "projectcreatedialog.h"
#include "settings/settingsdialog.h"
#include "themes/thememanager.h"

#include <QApplication>
#include <QWidget>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QAction>
#include <QStatusBar>
#include <QFileSystemModel>
#include <QTreeView>
#include <QModelIndex>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonParseError>
#include <QDir>
#include <QStyle>
#include <QDebug>
#include <QVariant>
#include <QSaveFile>
#include <QFileDialog>

    MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("ES Scenario Editor [Beta] - v0.100");
    resize(1400, 900);

    MenuActions a = createMenuBar(this);

    setupCentralWidget();

    ThemeManager::apply(qApp, m_codeEditor);

    connect(a.openprojectAction, &QAction::triggered, this, &MainWindow::openProject);
    connect(a.docAction, &QAction::triggered, this, &MainWindow::showDocs);
    connect(a.saveAction, &QAction::triggered, this, &MainWindow::saveFile);
    connect(a.saveasAction, &QAction::triggered, this, &MainWindow::saveFileAs);
    connect(a.newprojectAction, &QAction::triggered, this, &MainWindow::createProject);
    connect(a.settingsAction, &QAction::triggered, this, &MainWindow::showSetting);
}

void MainWindow::setupCentralWidget()
{
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(2);

    QWidget *leftPanel = new QWidget();
    leftPanel->setMinimumWidth(200);
    leftPanel->setMaximumWidth(350);
    leftPanel->setStyleSheet("background-color: #1c1c1c");

    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    QLabel *projectLabel = new QLabel("📂 Папка проекта");
    projectLabel->setStyleSheet("font-weight: bold; padding: 5px; color: #fff");
    leftLayout->addWidget(projectLabel);
    leftLayout->setAlignment(Qt::AlignTop);

    m_fileModel = new QFileSystemModel(this);
    m_fileModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);

    m_fileTree = new QTreeView(leftPanel);
    m_fileTree->setModel(m_fileModel);
    m_fileTree->setRootIndex(QModelIndex());
    m_fileTree->setHeaderHidden(true);
    m_fileTree->setIndentation(15);
    m_fileTree->setAnimated(true);

    m_fileTree->hideColumn(1);
    m_fileTree->hideColumn(2);
    m_fileTree->hideColumn(3);

    m_fileTree->setStyleSheet(
        "QTreeView {"
        "   background-color: #1c1c1c;"
        "   border: none;"
        "   outline: none;"
        "   color: #ccc;"
        "}"
        "QTreeView::item {"
        "   padding: 2px;"
        "   color: #ccc;"
        "}"
        "QTreeView::item:hover {"
        "   background-color: #2a2a2a;"
        "}"
        "QTreeView::item:selected {"
        "   background-color: #2a2a2a;"
        "   color: #ccc;"
        "}"
        );
    leftLayout->addWidget(m_fileTree);
    connect(
        m_fileTree,
        &QTreeView::doubleClicked,
        this,
        [this](const QModelIndex &index) {
            if (!index.isValid()) {
                return;
            }
            if (m_fileModel->isDir(index)) {
                if (m_fileTree->isExpanded(index)) {
                    m_fileTree->collapse(index);
                } else {
                    m_fileTree->expand(index);
                }
                return;
            }
            const QString filePath = m_fileModel->filePath(index);
            const QString suffix = QFileInfo(filePath).suffix().toLower();

            if (suffix == "rpy" ||
                suffix == "txt" ||
                suffix == "py") {
                openFile(filePath);
            }
        }
        );
    m_fileTree->hide();

    m_hintLabel = new QLabel("⚠ Папка проекта не выбрана\nНажмите Файл -> Открыть проект, чтобы выбрать папку");
    m_hintLabel->setStyleSheet("color: #888");
    m_hintLabel->setAlignment(Qt::AlignCenter);
    m_hintLabel->setWordWrap(true);

    leftLayout->addWidget(m_hintLabel);

    QWidget *centerPanel = new QWidget();
    centerPanel->setStyleSheet("background-color: #1a1a1a");

    QVBoxLayout *centerLayout = new QVBoxLayout(centerPanel);
    centerLayout->setContentsMargins(5, 5, 5, 5);

    QHBoxLayout *toolbarLayout = new QHBoxLayout();

    m_fileLabel = new QLabel("📄 Без названия");
    m_fileLabel->setStyleSheet(
        "QLabel {"
        "   font-weight: bold;"
        "   color: #fff;"
        "}"
        );

    toolbarLayout->addWidget(m_fileLabel);
    toolbarLayout->addStretch();

    centerLayout->addLayout(toolbarLayout);

    m_codeEditor = new CodeEditor(centerPanel);
    centerLayout->addWidget(m_codeEditor);

    //    connect(m_codeEditor,
    //           &QsciScintilla::cursorPositionChanged,
    //            this,
    //            [this](int line, int index) {
    //                statusBar()->showMessage(
    //                    QString("Строка: %1, Колонка: %2")
    //                        .arg(line + 1)
    //                        .arg(index + 1)
    //                    );
    //            });

    QWidget *rightPanel = new QWidget();
    rightPanel->setMinimumWidth(310);
    rightPanel->setMaximumWidth(400);
    rightPanel->setStyleSheet("background-color: #1c1c1c");

    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    QLabel *assetsLabel = new QLabel("🎨 Ассеты игры");
    assetsLabel->setStyleSheet("font-weight: bold; padding: 5px; color: #fff");
    rightLayout->addWidget(assetsLabel);

    QTreeWidget *assetsList = new QTreeWidget();
    assetsList->setHeaderHidden(true);
    assetsList->setIndentation(18);
    assetsList->setAnimated(true);
    assetsList->setExpandsOnDoubleClick(false);

    assetsList->setStyleSheet(
        "QTreeWidget {"
        "   background-color: #1c1c1c;"
        "   border: none;"
        "   outline: none;"
        "   color: #ccc;"
        "}"
        "QTreeWidget::item {"
        "   padding: 5px;"
        "}"
        "QTreeWidget::item:hover {"
        "   background-color: #2a2a2a;"
        "}"
        "QTreeWidget::item:selected {"
        "   background-color: #333;"
        "   color: #fff;"
        "}"
        );
    rightLayout->addWidget(assetsList, 1);

    QFile assetsFile(":/assets/assets.json");

    if (assetsFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QJsonParseError parseError;

        QJsonDocument document = QJsonDocument::fromJson(
            assetsFile.readAll(),
            &parseError
            );
        assetsFile.close();

        if (parseError.error == QJsonParseError::NoError && document.isObject()) {
            auto AddAssets = [&](auto && self,
                                 const QJsonArray &array,
                                 QTreeWidgetItem *parent) -> void {

                for (const QJsonValue &value : array) {

                    if (!value.isObject()) {
                        continue;
                    }
                    QJsonObject object = value.toObject();
                    QString name = object.value("name").toString();

                    auto *item = new QTreeWidgetItem();
                    item->setText(0, name);

                    item->setData(0, Qt::UserRole, object.value("code").toString());
                    item->setData(0, Qt::UserRole + 1, object.value("file").toString());
                    parent->addChild(item);

                    if (object.contains("children") && object.value("children").isArray()) {
                        item->setIcon(0, style()->standardIcon(QStyle::SP_DirIcon));
                        self(self, object.value("children").toArray(), item);
                    } else {
                        item->setIcon(0, style()->standardIcon(QStyle::SP_FileIcon));
                    }
                }
            };
            QJsonObject root = document.object();

            if (root.value("children").isArray()) {
                AddAssets(
                    AddAssets,
                    root.value("children").toArray(),
                    assetsList->invisibleRootItem()
                    );
            } else {
                qWarning() << "В assets.json отсутствует корневой массив children";
            }
            connect(
                assetsList,
                &QTreeWidget::itemClicked,
                assetsList,
                [assetsList](QTreeWidgetItem *item, int column) {
                    Q_UNUSED(column);

                    if (item->childCount() > 0) {
                        item->setExpanded(!item->isExpanded());
                    }
                }
                );
        } else {
            qWarning() << "Ошибка чтения assets.json:" << parseError.errorString();
            assetsList->addTopLevelItem(
                new QTreeWidgetItem({"❌ Ошибка формата assets.json"})
                );
        }
    } else {
        qWarning() << "Не удалось открыть ресурсы assets.json" << assetsFile.errorString();

        assetsList->addTopLevelItem(
            new QTreeWidgetItem({"❌ Не удалось загрузить ассеты"})
            );
    };

    m_assetPreview = new AssetPreview(rightPanel);
    m_assetPreview->setFixedSize(300, 250);

    rightLayout->addWidget(m_assetPreview, 0, Qt::AlignBottom | Qt::AlignCenter);

    QSplitter *s = new QSplitter(Qt::Horizontal);
    s->addWidget(leftPanel);
    s->addWidget(centerPanel);
    s->addWidget(rightPanel);
    s->setSizes({250, 900, 250});

    mainLayout->addWidget(s);
    setCentralWidget(centralWidget);

    QStatusBar *statusBar = this->statusBar();
    statusBar->setStyleSheet("background-color: #1c1c1c; color: #fff");

    m_statusLabel = new QLabel("⚠ Папка проекта не выбрана");
    m_statusLabel->setStyleSheet("padding: 2px; color: #fa0");
    statusBar->addWidget(m_statusLabel);

    QLabel *cursorLabel = new QLabel("Строка: 1, Колонка: 1");
    cursorLabel->setStyleSheet("color: #888");
    //  QObject::connect(codeEditor, &QTextEdit::cursorPositionChanged, [codeEditor, cursorLabel]() {
    //      QTextCursor cursor = codeEditor->textCursor();
    //      int line = cursor.blockNumber() + 1;
    //      int column = cursor.columnNumber() + 1;
    //      cursorLabel->setText(QString("Строка: %1, Колонка: %2").arg(line).arg(column));
    //  });
    statusBar->addPermanentWidget(cursorLabel);

    QLabel *versionLabel = new QLabel("Версия: v0.100");
    versionLabel->setStyleSheet("color: #666; font-size: 10px; padding: 2px;");
    statusBar->addPermanentWidget(versionLabel);

    QLabel *wikiverse = new QLabel("Разработка: WikiVerse Team");
    wikiverse->setStyleSheet("color: #888; font-size: 10px; padding: 2px;");
    statusBar->addPermanentWidget(wikiverse);
}

void MainWindow::showDocs()
{
    showDocDialog(this);
}

void MainWindow::openProject()
{
    const QString directory = QFileDialog::getExistingDirectory(
        this,
        "Открыть проект",
        QString(),
        QFileDialog::ShowDirsOnly
        );

    if (directory.isEmpty())
        return;

    setProjectDirectory(directory);
}

void MainWindow::openFile(const QString &filePath)
{
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(
            this,
            "Ошибка открытия",
            "Не удалось открыть файл:\n" + filePath + "\n\nПричина: " + file.errorString()
            );
        return;
    }
    const QByteArray fileData = file.readAll();
    file.close();

    m_codeEditor->setText(QString::fromUtf8(fileData));

    m_currentFilePath = filePath;

    const QString fileName = QFileInfo(filePath).fileName();
    m_fileLabel->setText("📄 " + fileName);
    m_fileLabel->setToolTip(filePath);

    setWindowTitle(
        "ES Scenario Editor [Beta] - v0.100 | " + fileName
        );

    if (m_statusLabel) {
        m_statusLabel->setText("📄 Открыт файл: " + filePath);
        m_statusLabel->setStyleSheet("padding: 2px; color: #65d69a");
    }

    m_codeEditor->setCursorPosition(0, 0);
    m_codeEditor->setFocus();
}

bool MainWindow::saveFile()
{
    if (m_currentFilePath.isEmpty()) {
        saveFile();
        return false;
    }
    QSaveFile file(m_currentFilePath);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(
            this,
            "Ошибка сохранения",
            "Не удалось открыть файл для записи:\n"
                + m_currentFilePath
                + "\n\nПричина: "
                + file.errorString()
            );
        return false;
    }
    const QByteArray content = m_codeEditor->text().toUtf8();

    if (file.write(content) != content.size()) {
        const QString error = file.errorString();
        file.cancelWriting();

        QMessageBox::critical(
            this,
            "Ошибка сохранения",
            "Не удалось записать содержиме файла:\n"
                + m_currentFilePath
                + "\n\nПричина: " + error
            );
        return false;
    }

    if (!file.commit()) {
        QMessageBox::critical(
            this,
            "Ошибка сохранения",
            "Не удалось завершить сохранение файла:\n"
                + m_currentFilePath
                + "\n\nПричина: "
                + file.errorString()
            );
        return false;
    }

    const QString fileName = QFileInfo(m_currentFilePath).fileName();

    m_fileLabel->setText("📄 " + fileName);
    m_fileLabel->setToolTip(m_currentFilePath);

    setWindowTitle("ES Scenario Editor [Beta] - v0.100 | " + fileName);

    if (m_statusLabel) {
        m_statusLabel->setText("✓ Файл сохранён: " + m_currentFilePath);
        m_statusLabel->setStyleSheet("padding: 2px; color: #65d69a");
    }
    return true;
}

void MainWindow::saveFileAs()
{
    const QString initialPath = m_currentFilePath.isEmpty()
    ? m_projectPath
    : m_currentFilePath;

    const QString filePath = QFileDialog::getSaveFileName(
        this,
        "Сохранить файл как",
        initialPath,
        "Сценарии Ren'Py (*.rpy);;"
        "Текстовые файлы (*.txt);;"
        "Python-файлы (*.py);;"
        "Все файлы (*.*)"
        );

    if (filePath.isEmpty()) {
        return;
    }

    const QString previousPath = m_currentFilePath;
    m_currentFilePath = filePath;

    if (!saveFile()) {
        m_currentFilePath = previousPath;
    }
}

void MainWindow::createProject()
{
    CreateProjectDialog dialog(this);

    if (dialog.exec() != QDialog::Accepted)
        return;

    const QString directory = dialog.projectPath();

    if (directory.isEmpty())
        return;

    setProjectDirectory(directory);
}

void MainWindow::setProjectDirectory(const QString &directory)
{
    m_projectPath = directory;

    m_fileModel->setRootPath(m_projectPath);
    m_fileTree->setRootIndex(m_fileModel->index(m_projectPath));

    m_fileTree->show();
    m_hintLabel->hide();

    if (m_statusLabel) {
        m_statusLabel->setText("📂 Проект: " + m_projectPath);
        m_statusLabel->setStyleSheet(
            "padding: 2px; color: #65d69a"
            );
    }

    const QString projectName =
        QFileInfo(m_projectPath).fileName();

    setWindowTitle(
        "ES Scenario Editor [Beta] - v0.100 | " + projectName
        );
}

void MainWindow::showSetting()
{
    SettingsDialog dialog(this);

    connect(&dialog, &SettingsDialog::themeChanged,
            this, [this](const QString &) {
                ThemeManager::apply(qApp, m_codeEditor);
            });

    dialog.exec();
}