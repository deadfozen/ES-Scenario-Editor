#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QMenu>
#include <QAction>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QComboBox>
#include <QDir>
#include <QTextEdit>
#include <QSplitter>
#include <QListWidget>
#include <QTreeWidget>
#include <QWidget>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QTreeView>
#include <QHeaderView>
#include <QShortcut>

static QString currentOpenFolder = "";
static QString currentFilePath = "";

bool showNewFileDialog(QWidget *parent) {
    QDialog dialog(parent);
    dialog.setWindowTitle("Создание нового файла");
    dialog.setModal(true);
    dialog.setMinimumWidth(500);

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    // поле для ввода названия файла
    QHBoxLayout *nameLayout = new QHBoxLayout();
    QLabel *nameLabel = new QLabel("Имя файла:", &dialog);
    nameLabel->setFixedWidth(100);
    QLineEdit *nameEdit = new QLineEdit(&dialog);
    nameEdit->setPlaceholderText("Введите название...");
    nameEdit->setText("untitled");
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameEdit);
    mainLayout->addLayout(nameLayout);

    // выбор расширения
    QHBoxLayout *extLayout = new QHBoxLayout();
    QLabel *extlabel = new QLabel("Расширение:", &dialog);
    extlabel->setFixedWidth(100);
    QComboBox *extCombo = new QComboBox(&dialog);
    extCombo->addItems({".rpy", ".txt", ".py"});
    extLayout->addWidget(extlabel);
    extLayout->addWidget(extCombo);
    mainLayout->addLayout(extLayout);

    // выбор папки для сейва
    QHBoxLayout *pathLayout = new QHBoxLayout();
    QLabel *pathLabel = new QLabel("Папка:", &dialog);
    pathLabel->setFixedWidth(100);
    QLineEdit *pathEdit = new QLineEdit(&dialog);

    if (!currentOpenFolder.isEmpty() && QDir(currentOpenFolder).exists()) {
        pathEdit->setText(currentOpenFolder);
    } else {
        pathEdit->setText("Папка не выбрана");
    }
    pathEdit->setReadOnly(true);
    QPushButton *browseButton = new QPushButton("Обзор...", &dialog);
    browseButton->setFixedWidth(80);
    pathLayout->addWidget(pathLabel);
    pathLayout->addWidget(pathEdit);
    pathLayout->addWidget(browseButton);
    mainLayout->addLayout(pathLayout);
    mainLayout->addStretch();

    // кнопки
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *createButton = new QPushButton("Создать", &dialog);
    QPushButton *cancelButton = new QPushButton("Отмена", &dialog);
    buttonLayout->addStretch();
    buttonLayout->addWidget(createButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    // Обзор папки
    QObject::connect(browseButton, &QPushButton::clicked, [&]() {
        QString folder = QFileDialog::getExistingDirectory(&dialog, "Выберите папку",
                                                           currentOpenFolder.isEmpty() ? QDir::homePath() : currentOpenFolder);
        if (!folder.isEmpty()) {
            pathEdit->setText(folder);
            currentOpenFolder = folder;
        }
    });

    QObject::connect(createButton, &QPushButton::clicked, [&]() {
        if (currentOpenFolder.isEmpty()) {
            QMessageBox::warning(&dialog, "Ошибка", "Сначала выберите папку для сохранения файла!");
            return;
        }

        if (nameEdit->text().isEmpty()) {
            QMessageBox::warning(&dialog, "Ошибка", "Введите имя файла!");
            return;
        }

        QString fileName = nameEdit->text();
        bool isValid = true;
        for (int i = 0; i < fileName.length(); ++i) {
            QChar ch = fileName[i];
            if (!((ch >= 'a' && ch <= 'z') ||
                  (ch >= 'A' && ch <= 'Z') ||
                  (ch >= '0' && ch <= '9') ||
                  ch == '-' ||
                  ch == '_')) {
                isValid = false;
                break;
            }
        }
        if (!isValid) {
            QMessageBox::warning(&dialog, "Ошибка", "Имя файла должно содержать только латинские буквы, цифры, дефис и подчеркивание");
            return;
        }
        dialog.accept();
    });
    QObject::connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    return dialog.exec();
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QMainWindow window;
    window.setWindowTitle("ES Scenario Editor");
    window.resize(1400, 900);

    // Центральный виджет
    QWidget *centralWidget = new QWidget(&window);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(2);

    // Левая панель - файловый менеджер
    QWidget *leftPanel = new QWidget();
    leftPanel->setMinimumWidth(200);
    leftPanel->setMaximumWidth(350);
    leftPanel->setStyleSheet("background-color: #1c1c1c;");

    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    QLabel *projectLabel = new QLabel("📁 Папка проекта");
    projectLabel->setStyleSheet("font-weight: bold; padding: 5px; color: #ffffff;");
    leftLayout->addWidget(projectLabel);

    // Создаем пустую модель для файлов
    QFileSystemModel *fileModel = new QFileSystemModel();
    fileModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);

    QTreeView *fileTree = new QTreeView();
    fileTree->setModel(fileModel);
    // Не устанавливаем корневую папку - показываем пустой список
    fileTree->setRootIndex(QModelIndex());
    fileTree->setHeaderHidden(true);
    fileTree->setIndentation(15);

    // Скрываем все колонки кроме первой (с именами файлов)
    fileTree->hideColumn(1); // Size
    fileTree->hideColumn(2); // Type
    fileTree->hideColumn(3); // Date Modified

    fileTree->setStyleSheet(
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
    leftLayout->addWidget(fileTree);

    // Кнопка выбора папки
    QPushButton *changeFolderBtn = new QPushButton("📂 Выбрать папку проекта");
    changeFolderBtn->setStyleSheet(
        "QPushButton {"
        "   background-color: #2a2a2a;"
        "   color: #fff;"
        "   border: 1px solid #3a3a3a;"
        "   padding: 8px;"
        "   border-radius: 3px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #3a3a3a;"
        "   border: 1px solid #333;"
        "}"
        );

    // Добавляем подсказку о выборе папки
    QLabel *hintLabel = new QLabel("⚠️ Папка проекта не выбрана\nНажмите кнопку ниже,\nчтобы выбрать папку");
    hintLabel->setStyleSheet("color: #888888; padding: 10px; text-align: center;");
    hintLabel->setAlignment(Qt::AlignCenter);
    hintLabel->setWordWrap(true);
    leftLayout->addWidget(hintLabel);

    QObject::connect(changeFolderBtn, &QPushButton::clicked, [&]() {
        QString folder = QFileDialog::getExistingDirectory(&window, "Выберите папку проекта",
                                                           currentOpenFolder.isEmpty() ? QDir::homePath() : currentOpenFolder);
        if (!folder.isEmpty()) {
            // Скрываем подсказку
            hintLabel->hide();

            // Устанавливаем новую корневую папку
            fileModel->setRootPath(folder);
            fileTree->setRootIndex(fileModel->index(folder));
            currentOpenFolder = folder;

            // Обновляем заголовок
            projectLabel->setText("📁 " + QFileInfo(folder).fileName());

            // Обновляем статус
            window.statusBar()->showMessage("Выбрана папка: " + folder, 3000);
        }
    });
    leftLayout->addWidget(changeFolderBtn);

    // Центральная панель - редактор кода
    QWidget *centerPanel = new QWidget();
    centerPanel->setStyleSheet("background-color: #1a1a1a;");

    QVBoxLayout *centerLayout = new QVBoxLayout(centerPanel);
    centerLayout->setContentsMargins(5, 5, 5, 5);

    // Панель с именем файла
    QHBoxLayout *toolbarLayout = new QHBoxLayout();
    QLabel *fileLabel = new QLabel("📄 Без названия");
    fileLabel->setStyleSheet("font-weight: bold; color: #ffffff;");
    toolbarLayout->addWidget(fileLabel);
    toolbarLayout->addStretch();
    centerLayout->addLayout(toolbarLayout);

    // Редактор кода
    QTextEdit *codeEditor = new QTextEdit();
    codeEditor->setFontFamily("Consolas");
    codeEditor->setFontPointSize(12);
    codeEditor->setStyleSheet(
        "QTextEdit {"
        "   border: 1px solid #333;"
        "   border-radius: 4px;"
        "   background-color: #1a1a1a;"
        "   color: #d4d4d4;"
        "   selection-background-color: #2a2a2a;"
        "}"
        "QTextEdit:focus {"
        "   border: 2px solid #2a2a2a;"
        "}"
        );
    codeEditor->setPlaceholderText("Сначала выберите папку проекта, затем создайте или откройте файл");
    centerLayout->addWidget(codeEditor);

    // Правая панель - ассеты
    QWidget *rightPanel = new QWidget();
    rightPanel->setMinimumWidth(200);
    rightPanel->setMaximumWidth(350);
    rightPanel->setStyleSheet("background-color: #1c1c1c;");

    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    QLabel *assetsLabel = new QLabel("🎨 Ассеты игры");
    assetsLabel->setStyleSheet("font-weight: bold; padding: 5px; color: #ffffff;");
    rightLayout->addWidget(assetsLabel);

    // Список ассетов с сообщением о недоступности
    QListWidget *assetsList = new QListWidget();
    assetsList->setStyleSheet(
        "QListWidget {"
        "   background-color: #1c1c1c;"
        "   border: none;"
        "   color: #666;"
        "}"
        "QListWidget::item {"
        "   padding: 10px;"
        "   text-align: center;"
        "}"
        );

    QListWidgetItem *placeholderItem = new QListWidgetItem("🖼️ Ассеты пока недоступны\n\nАссеты игры будут добавлены\nпосле выбора папки проекта");
    placeholderItem->setTextAlignment(Qt::AlignCenter);
    placeholderItem->setForeground(QColor(100, 100, 100));
    assetsList->addItem(placeholderItem);

    rightLayout->addWidget(assetsList);

    // Добавляем панели в сплиттер
    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(leftPanel);
    splitter->addWidget(centerPanel);
    splitter->addWidget(rightPanel);
    splitter->setSizes({250, 700, 250});

    mainLayout->addWidget(splitter);
    window.setCentralWidget(centralWidget);

    // Статусная строка
    QStatusBar *statusBar = window.statusBar();
    statusBar->setStyleSheet("background-color: #1c1c1c; color: #fff;");

    QLabel *statusLabel = new QLabel("⚠️ Папка проекта не выбрана");
    statusLabel->setStyleSheet("padding: 2px; color: #ffaa00;");
    statusBar->addWidget(statusLabel);

    // Информация о позиции курсора
    QLabel *cursorLabel = new QLabel("Строка: 1, Колонка: 1");
    cursorLabel->setStyleSheet("color: #888;");
    QObject::connect(codeEditor, &QTextEdit::cursorPositionChanged, [codeEditor, cursorLabel]() {
        QTextCursor cursor = codeEditor->textCursor();
        int line = cursor.blockNumber() + 1;
        int column = cursor.columnNumber() + 1;
        cursorLabel->setText(QString("Строка: %1, Колонка: %2").arg(line).arg(column));
    });
    statusBar->addPermanentWidget(cursorLabel);

    // Версия
    QLabel *versionLabel = new QLabel("Версия: 0.1.0");
    versionLabel->setStyleSheet("color: #666; font-size: 10px; padding: 2px;");
    statusBar->addPermanentWidget(versionLabel);

    // Функция открытия файла
    auto openFile = [&](const QString &filePath) {
        if (filePath.isEmpty()) return;
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            codeEditor->setPlainText(in.readAll());
            file.close();
            currentFilePath = filePath;
            fileLabel->setText("📄 " + QFileInfo(filePath).fileName());
            statusLabel->setText("Открыт: " + filePath);
            statusLabel->setStyleSheet("padding: 2px; color: #fff;");
        }
    };

    // Функция сохранения файла
    auto saveFile = [&]() {
        if (currentOpenFolder.isEmpty()) {
            QMessageBox::warning(&window, "Ошибка", "Сначала выберите папку проекта!");
            return;
        }

        if (currentFilePath.isEmpty()) {
            QString filePath = QFileDialog::getSaveFileName(&window, "Сохранить файл",
                                                            currentOpenFolder + "/untitled",
                                                            "All Files (*.rpy *.txt *.py);;Ren'Py (*.rpy);;Text (*.txt);;Python (*.py)");
            if (!filePath.isEmpty()) {
                currentFilePath = filePath;
                fileLabel->setText("📄 " + QFileInfo(filePath).fileName());
            } else {
                return;
            }
        }

        QFile file(currentFilePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << codeEditor->toPlainText();
            file.close();
            statusLabel->setText("Сохранено: " + currentFilePath);
            statusLabel->setStyleSheet("padding: 2px; color: #fff;");
        }
    };

    // Двойной клик по файлу в дереве
    QObject::connect(fileTree, &QTreeView::doubleClicked, [&](const QModelIndex &index) {
        if (currentOpenFolder.isEmpty()) {
            QMessageBox::information(&window, "Информация", "Сначала выберите папку проекта!");
            return;
        }
        if (fileModel->isDir(index)) {
            return;
        }
        QString filePath = fileModel->filePath(index);
        openFile(filePath);
    });

    // Глобальные горячие клавиши
    QShortcut *openShortcut = new QShortcut(QKeySequence::Open, &window);
    QObject::connect(openShortcut, &QShortcut::activated, [&]() {
        if (currentOpenFolder.isEmpty()) {
            QMessageBox::information(&window, "Информация", "Сначала выберите папку проекта!");
            return;
        }
        QString filePath = QFileDialog::getOpenFileName(&window, "Открыть файл",
                                                        currentOpenFolder,
                                                        "All Files (*.rpy *.txt *.py *.json);;Ren'Py (*.rpy);;Text (*.txt);;Python (*.py)");
        openFile(filePath);
    });

    QShortcut *saveShortcut = new QShortcut(QKeySequence::Save, &window);
    QObject::connect(saveShortcut, &QShortcut::activated, saveFile);

    // Меню
    QMenuBar *menuBar = window.menuBar();
    menuBar->setStyleSheet(
        "QMenuBar {"
        "   background-color: #1c1c1c;"
        "   color: #fff;"
        "}"
        "QMenuBar::item:selected {"
        "   background-color: #2a2a2a;"
        "}"
        "QMenu {"
        "   background-color: #1c1c1c;"
        "   color: #fff;"
        "}"
        "QMenu::item:selected {"
        "   background-color: #2a2a2a;"
        "}"
        );

    QMenu *fileMenu = menuBar->addMenu("Файл");
    QAction *newAction = fileMenu->addAction("Новый файл");
    QObject::connect(newAction, &QAction::triggered, [&window]() {
        if (showNewFileDialog(&window)) {
            QMessageBox::information(&window, "Успех", "Файл создан!");
        }
    });
    newAction->setShortcut(QKeySequence::New);

    QAction *openAction = fileMenu->addAction("Открыть...");
    openAction->setShortcut(QKeySequence::Open);
    QObject::connect(openAction, &QAction::triggered, [&]() {
        if (currentOpenFolder.isEmpty()) {
            QMessageBox::information(&window, "Информация", "Сначала выберите папку проекта!");
            return;
        }
        QString filePath = QFileDialog::getOpenFileName(&window, "Открыть файл",
                                                        currentOpenFolder,
                                                        "All Files (*.rpy *.txt *.py *.json);;Ren'Py (*.rpy);;Text (*.txt);;Python (*.py)");
        openFile(filePath);
    });

    QAction *saveAction = fileMenu->addAction("Сохранить");
    saveAction->setShortcut(QKeySequence::Save);
    QObject::connect(saveAction, &QAction::triggered, saveFile);

    fileMenu->addSeparator();
    QAction *exitAction = fileMenu->addAction("Выход");
    QObject::connect(exitAction, &QAction::triggered, &app, &QApplication::quit);
    exitAction->setShortcut(QKeySequence::Quit);

    QMenu *helpMenu = menuBar->addMenu("Справка");
    QAction *aboutAction = helpMenu->addAction("О программе");
    QObject::connect(aboutAction, &QAction::triggered, [&window]() {
        QMessageBox::about(&window, "О программе",
                           "<h2>ES Scenario Editor</h2>"
                           "<p><b>Версия:</b> 0.1.0</p>"
                           "<p>Редактор сценариев и кода для визуальной новеллы 'Бесконечное Лето'</p>"
                           "<p><i>Разработано с использованием Qt</i></p>");
    });

    QAction *aboutdevAction = helpMenu->addAction("О разработчиках");
    QObject::connect(aboutdevAction, &QAction::triggered, [&window]() {
        QMessageBox::about(&window, "О разработчиках",
                           "<h3>Разработчики</h3>"
                           "<p><b>Главный разработчик:</b> Я</p>"
                           "<p>Сделано с ❤️ для сообщества</p>");
    });

    window.show();
    return app.exec();
}