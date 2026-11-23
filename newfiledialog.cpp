#include "newfiledialog.h"

#include <QComboBox>
#include <QDialog>
#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QRegularExpression>

QString showNewFileDialog(QWidget *parent, QString &currentOpenFolder)
{
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
    extCombo->addItems({".rpy", ".txt", ".py", "json"});
    extLayout->addWidget(extlabel);
    extLayout->addWidget(extCombo);
    mainLayout->addLayout(extLayout);

    QHBoxLayout *pathLayout = new QHBoxLayout();
    QLabel *pathLabel = new QLabel("Папка:", &dialog);
    pathLabel->setFixedWidth(100);
    QLineEdit *pathEdit = new QLineEdit(&dialog);
    pathEdit->setReadOnly(true);

    if (!currentOpenFolder.isEmpty()
        && QDir(currentOpenFolder).exists()) {
        pathEdit->setText(currentOpenFolder);
    } else {
        currentOpenFolder.clear();
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

    QObject::connect(browseButton, &QPushButton::clicked, &dialog, [&]() {
        const QString startDirecroty = currentOpenFolder.isEmpty() ? QDir::homePath() : currentOpenFolder;
        const QString folder = QFileDialog::getExistingDirectory(&dialog, "Выберите папку", startDirecroty, QFileDialog::ShowDirsOnly);
        if (!folder.isEmpty()) {
            currentOpenFolder = folder;
            pathEdit->setText(folder);
        }
    });
    QObject::connect(createButton, &QPushButton::clicked, &dialog, [&]() {
        const QString fileName = nameEdit->text().trimmed();
        if (currentOpenFolder.isEmpty() || !QDir(currentOpenFolder).exists()) {
            QMessageBox::warning(&dialog, "Ошибка", "Сначала выберите существующую папку!");
            return;
        }
        if (fileName.isEmpty()) {
            QMessageBox::warning(&dialog, "Ошибка", "Введите имя файла!");
            nameEdit->setFocus();
            return;
        }
        const QRegularExpression validName("^[A-Za-z0-9_-]+$");
        if (!validName.match(fileName).hasMatch()) {
            QMessageBox::warning(&dialog, "Ошибка", "Имя должно содержать только латинские буквы, цифры, дефис и подчёркивание.");
            nameEdit->setFocus();
            return;
        }
        const QString fullPath = QDir(currentOpenFolder).filePath(fileName + extCombo->currentText());
        if (QFileInfo::exists(fullPath)) {
            QMessageBox::warning(&dialog, "Ошибка", "Файл с таким именем уже существует!");
            return;
        }
        dialog.accept();
    });
    QObject::connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted)
        return {};
    return QDir(currentOpenFolder).filePath(nameEdit->text().trimmed() + extCombo->currentText())
}