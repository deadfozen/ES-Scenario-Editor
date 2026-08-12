#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
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
// #include <QStatusBar>

static QString currentOpenFolder = QDir::currentPath();

bool showNewFileDialog(QWidget *parent) {
    QDialog dialog(parent);
    dialog.setWindowTitle("Создание нового файла");
    dialog.setModal(true);
    dialog.setMinimumWidth(500);
    // dialog.setFixedHeight(200);

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
    extCombo->addItems({".rpy (Ren'Py)", ".txt (Текстовый документ)", ".py (Python)"});
    extLayout->addWidget(extlabel);
    extLayout->addWidget(extCombo);
    mainLayout->addLayout(extLayout);

    // выбор папки для сейва
    QHBoxLayout *pathLayout = new QHBoxLayout();
    QLabel *pathLabel = new QLabel("Папка:", &dialog);
    pathLabel->setFixedWidth(100);
    QLineEdit *pathEdit = new QLineEdit(&dialog);

    // если папка открыта, юзаем её, иначе другую
    if (!currentOpenFolder.isEmpty() && QDir(currentOpenFolder).exists()) {
        pathEdit->setText(currentOpenFolder);
    } else {
        pathEdit->setText(QDir::currentPath());
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
    QObject::connect(createButton, &QPushButton::clicked, [&]() {
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

    // запуск модалки
    return dialog.exec();
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QMainWindow window;
    window.setWindowTitle("ES Scenario Editor");
    window.resize(1200, 800);

    QMenuBar *menuBar = window.menuBar();
    QMenu *fileMenu = menuBar->addMenu("Файл");
    QAction *newAction = fileMenu->addAction("Новый файл");
    QObject::connect(newAction, &QAction::triggered, [&window]() {
        showNewFileDialog(&window);
    });
    newAction->setShortcut(QKeySequence::New);

    fileMenu->addSeparator();
    QAction *exitAction = fileMenu->addAction("Выход");
    QObject::connect(exitAction, &QAction::triggered, &app, &QApplication::quit);

    QMenu *helpMenu = menuBar->addMenu("Справка");
    QAction *aboutAction = helpMenu->addAction("О программе");
    QObject::connect(aboutAction, &QAction::triggered, [&window]() {
        QMessageBox::about(&window, "О программе",
                           "<h2>ES Scenario Editor</h2>"
                           "<p><b>Версия:</b> 0.0.2 (2)</p>"
                           "<p>Редактор сценариев и кода для визуальной новеллы 'Бесконечное Лето'</p>");
    });
    QAction *aboutdevAction = helpMenu->addAction("О разработчиках");
    QObject::connect(aboutdevAction, &QAction::triggered, [&window]() {
        QMessageBox::about(&window, "О разработчиках",
                           "<p><b>Главный разработчик (и единственный)</b></p>"
                           "<p>Я</p>");
    });

    window.show();
    return app.exec();
}


// 3yNDMKbdVgs -- id от юбуст!