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
#include <QSyntaxHighlighter>
#include <QRegularExpression>
#include <QTextCharFormat>
#include <QColor>

class RenPyHighlighter : public QSyntaxHighlighter
{
public:
    RenPyHighlighter(QTextDocument *parent = nullptr) : QSyntaxHighlighter(parent) {
        HighlightingRule rule;

        QTextCharFormat stringFormat;
        stringFormat.setForeground(QColor(206, 145, 120));
        rule.pattern = QRegularExpression("\".*?\"", QRegularExpression::InvertedGreedinessOption);
        rule.format = stringFormat;
        highlightingRules.append(rule);

        QTextCharFormat commentFormat;
        commentFormat.setForeground(QColor(87, 166, 74));

        rule.pattern = QRegularExpression("(?<![\\w\"'])#.*$");
        rule.format = commentFormat;
        highlightingRules.append(rule);

        QTextCharFormat keywordFormat;
        keywordFormat.setForeground(QColor(86, 156, 214));
        keywordFormat.setFontWeight(QFont::Bold);

        QStringList keywords;
        keywords << "define" << "scene" << "show" << "play" << "return" << "label" << "if" << "elif" << "else" << "while" << "for";

        for (int i = 0; i < keywords.size(); ++i) {
            rule.pattern = QRegularExpression("\\b" + keywords[i] + "\\b");
            rule.format = keywordFormat;
            highlightingRules.append(rule);
        }

        QTextCharFormat dollarFormat;
        dollarFormat.setForeground(QColor(206, 145, 120));
        rule.pattern = QRegularExpression("\\$\\s*\\w+");
        rule.format = dollarFormat;
        highlightingRules.append(rule);

        QTextCharFormat labelFormat;
        labelFormat.setForeground(QColor(220, 220, 170));
        labelFormat.setFontWeight(QFont::Bold);
        rule.pattern = QRegularExpression("^\\s*label\\s+\\w+:");
        rule.format = labelFormat;
        highlightingRules.append(rule);

        QTextCharFormat numberFormat;
        numberFormat.setForeground(QColor(181, 206, 168));
        rule.pattern = QRegularExpression("\\b\\d+\\b");
        rule.format = numberFormat;
        highlightingRules.append(rule);
    }

protected:
    void highlightBlock(const QString &text) override {

        bool isCommentLine = false;
        QString trimmed = text.trimmed();
        if (trimmed.startsWith("#")) {
            isCommentLine = true;
        }

        if (isCommentLine) {
            QTextCharFormat commentFormat;
            commentFormat.setForeground(QColor(87, 166, 74));
            setFormat(0, text.length(), commentFormat);
            return;
        }

        for (int i = 0; i < highlightingRules.size(); ++i) {
            const HighlightingRule &rule = highlightingRules[i];
            QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
            while (matchIterator.hasNext()) {
                QRegularExpressionMatch match = matchIterator.next();
                setFormat(match.capturedStart(), match.capturedLength(), rule.format);
            }
        }
    }

private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QList<HighlightingRule> highlightingRules;
};

static QString currentOpenFolder = "";
static QString currentFilePath = "";

bool showNewFileDialog(QWidget *parent) {
    QDialog dialog(parent);
    dialog.setWindowTitle("Создание нового файла");
    dialog.setModal(true);
    dialog.setMinimumWidth(500);

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    QHBoxLayout *nameLayout = new QHBoxLayout();
    QLabel *nameLabel = new QLabel("Имя файла:", &dialog);
    nameLabel->setFixedWidth(100);
    QLineEdit *nameEdit = new QLineEdit(&dialog);
    nameEdit->setPlaceholderText("Введите название...");
    nameEdit->setText("untitled");
    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameEdit);
    mainLayout->addLayout(nameLayout);

    QHBoxLayout *extLayout = new QHBoxLayout();
    QLabel *extlabel = new QLabel("Расширение:", &dialog);
    extlabel->setFixedWidth(100);
    QComboBox *extCombo = new QComboBox(&dialog);
    extCombo->addItems({".rpy", ".txt", ".py"});
    extLayout->addWidget(extlabel);
    extLayout->addWidget(extCombo);
    mainLayout->addLayout(extLayout);

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

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *createButton = new QPushButton("Создать", &dialog);
    QPushButton *cancelButton = new QPushButton("Отмена", &dialog);
    buttonLayout->addStretch();
    buttonLayout->addWidget(createButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

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

    QWidget *centralWidget = new QWidget(&window);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(2);

    QWidget *leftPanel = new QWidget();
    leftPanel->setMinimumWidth(200);
    leftPanel->setMaximumWidth(350);
    leftPanel->setStyleSheet("background-color: #1c1c1c;");

    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    QLabel *projectLabel = new QLabel("📁 Папка проекта");
    projectLabel->setStyleSheet("font-weight: bold; padding: 5px; color: #ffffff;");
    leftLayout->addWidget(projectLabel);

    QFileSystemModel *fileModel = new QFileSystemModel();
    fileModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files);

    QTreeView *fileTree = new QTreeView();
    fileTree->setModel(fileModel);
    fileTree->setRootIndex(QModelIndex());
    fileTree->setHeaderHidden(true);
    fileTree->setIndentation(15);

    fileTree->hideColumn(1);
    fileTree->hideColumn(2);
    fileTree->hideColumn(3);

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

    QLabel *hintLabel = new QLabel("⚠️ Папка проекта не выбрана\nНажмите кнопку ниже,\nчтобы выбрать папку");
    hintLabel->setStyleSheet("color: #888888; padding: 10px; text-align: center;");
    hintLabel->setAlignment(Qt::AlignCenter);
    hintLabel->setWordWrap(true);
    leftLayout->addWidget(hintLabel);

    QObject::connect(changeFolderBtn, &QPushButton::clicked, [&]() {
        QString folder = QFileDialog::getExistingDirectory(&window, "Выберите папку проекта",
                                                           currentOpenFolder.isEmpty() ? QDir::homePath() : currentOpenFolder);
        if (!folder.isEmpty()) {
            hintLabel->hide();

            fileModel->setRootPath(folder);
            fileTree->setRootIndex(fileModel->index(folder));
            currentOpenFolder = folder;

            projectLabel->setText("📁 " + QFileInfo(folder).fileName());

            window.statusBar()->showMessage("Выбрана папка: " + folder, 3000);
        }
    });
    leftLayout->addWidget(changeFolderBtn);

    QWidget *centerPanel = new QWidget();
    centerPanel->setStyleSheet("background-color: #1a1a1a;");

    QVBoxLayout *centerLayout = new QVBoxLayout(centerPanel);
    centerLayout->setContentsMargins(5, 5, 5, 5);

    QHBoxLayout *toolbarLayout = new QHBoxLayout();
    QLabel *fileLabel = new QLabel("📄 Без названия");
    fileLabel->setStyleSheet("font-weight: bold; color: #ffffff;");
    toolbarLayout->addWidget(fileLabel);
    toolbarLayout->addStretch();
    centerLayout->addLayout(toolbarLayout);

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

    // Создаем объект для подсветки синтаксиса
    RenPyHighlighter *highlighter = new RenPyHighlighter(codeEditor->document());

    QWidget *rightPanel = new QWidget();
    rightPanel->setMinimumWidth(200);
    rightPanel->setMaximumWidth(350);
    rightPanel->setStyleSheet("background-color: #1c1c1c;");

    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    QLabel *assetsLabel = new QLabel("🎨 Ассеты игры");
    assetsLabel->setStyleSheet("font-weight: bold; padding: 5px; color: #ffffff;");
    rightLayout->addWidget(assetsLabel);

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

    QListWidgetItem *placeholderItem = new QListWidgetItem("🖼️ Ассеты пока недоступны\n\nАссеты игры будут добавлены позже");
    placeholderItem->setTextAlignment(Qt::AlignCenter);
    placeholderItem->setForeground(QColor(100, 100, 100));
    assetsList->addItem(placeholderItem);

    rightLayout->addWidget(assetsList);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    splitter->addWidget(leftPanel);
    splitter->addWidget(centerPanel);
    splitter->addWidget(rightPanel);
    splitter->setSizes({250, 700, 250});

    mainLayout->addWidget(splitter);
    window.setCentralWidget(centralWidget);

    QStatusBar *statusBar = window.statusBar();
    statusBar->setStyleSheet("background-color: #1c1c1c; color: #fff;");

    QLabel *statusLabel = new QLabel("⚠️ Папка проекта не выбрана");
    statusLabel->setStyleSheet("padding: 2px; color: #ffaa00;");
    statusBar->addWidget(statusLabel);

    QLabel *cursorLabel = new QLabel("Строка: 1, Колонка: 1");
    cursorLabel->setStyleSheet("color: #888;");
    QObject::connect(codeEditor, &QTextEdit::cursorPositionChanged, [codeEditor, cursorLabel]() {
        QTextCursor cursor = codeEditor->textCursor();
        int line = cursor.blockNumber() + 1;
        int column = cursor.columnNumber() + 1;
        cursorLabel->setText(QString("Строка: %1, Колонка: %2").arg(line).arg(column));
    });
    statusBar->addPermanentWidget(cursorLabel);

    QLabel *versionLabel = new QLabel("Версия: 0.2.0");
    versionLabel->setStyleSheet("color: #666; font-size: 10px; padding: 2px;");
    statusBar->addPermanentWidget(versionLabel);

    // Функция для определения языка подсветки по расширению
    auto updateHighlighter = [&](const QString &filePath) {
        QFileInfo info(filePath);
        QString extension = info.suffix().toLower();

        // Пока только для .rpy, можно расширить позже
        if (extension == "rpy") {
            highlighter->setDocument(codeEditor->document());
            // Переподсвечиваем весь документ
            highlighter->rehighlight();
        } else {
            // Для других расширений отключаем подсветку
            highlighter->setDocument(nullptr);
            // Восстанавливаем документ без подсветки
            highlighter->setDocument(codeEditor->document());
        }
    };

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

            // Обновляем подсветку в зависимости от расширения
            updateHighlighter(filePath);
        }
    };

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
                updateHighlighter(filePath);
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
                           "<p><b>Версия:</b> 0.2.0</p>"
                           "<p>Редактор сценариев и кода для визуальной новеллы 'Бесконечное Лето'</p>"
                           "<p><i>Разработано с использованием Qt</i></p>");
    });

    QAction *aboutdevAction = helpMenu->addAction("О разработчиках");
    QObject::connect(aboutdevAction, &QAction::triggered, [&window]() {
        QMessageBox::about(&window, "О разработчиках",
                           "");
    });

    window.show();
    return app.exec();
}