#include "projectcreatedialog.h"
#include "projectcreator.h"

#include <QCheckBox>
#include <QDir>
#include <QFileDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSettings>
#include <QStandardPaths>
#include <QRegularExpression>
#include <QFile>
#include <QFileInfo>

static QString findEverlastingSummerMods()
{
    QStringList steamPaths;

    steamPaths << "C:/Program Files (x86)/Steam" << "C:/Program Files/Steam" << "C:/Steam";
    const QString progamFilesX86 = qEnvironmentVariable("ProgramFiles(x86)");

    if (!progamFilesX86.isEmpty()) {
        steamPaths << QDir(progamFilesX86).filePath("Steam");
    }

    QSettings steamRegistry(
        "HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Valve\\Steam",
        QSettings::NativeFormat
    );

    const QString registryPath = steamRegistry.value("InstallPath").toString();

    if (!registryPath.isEmpty())
        steamPaths << registryPath;

    steamPaths.removeDuplicates();

    for (const QString &steamPath : steamPaths) {
        const QString gamePath = QDir(steamPath).filePath("steamapps/common/Everlasting Summer/game/mods");
        if (QDir(gamePath).exists())
            return QDir::cleanPath(gamePath);

        const QString libraryFile = QDir(steamPath).filePath("steamapps/libraryfolders.vdf");
        QFile file(libraryFile);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            continue;

        const QString content = QString::fromUtf8(file.readAll());

        const QRegularExpression libraryRegex(R"("path"\s*"([^"]+)");

        auto it = libraryRegex.globalMatch(content);

        while (it.hasNext()) {
            const auto match = it.next();
            QString libraryPath = match.captured(1);
            libraryPath.replace("\\\\", "/");
            const QString modsPath = QDir(libraryPath).filePath("steamapps/common/Everlasting Summer/game/mods");

            if (QDir(modsPath).exists())
                return QDir::cleanPath(modsPath);
        }
    }
    return {};
}

CreateProjectDialog::CreateProjectDialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle("Создание проекта");
    setModal(true);
    setMinimumWidth(500);
    setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(10);

    auto *nameLayout = new QHBoxLayout();

    auto *nameLabel = new QLabel("Название проекта:", this);
    nameLabel->setFixedWidth(100);

    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText("Введите название проекта...");
    m_nameEdit->setText("untitled");
    m_nameEdit->setClearButtonEnabled(true);

    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(m_nameEdit);

    mainLayout->addLayout(nameLayout);

    auto *locationLayout = new QHBoxLayout();

    auto *locationLabel = new QLabel("Расположение: ", this);
    locationLabel->setFixedWidth(100);

    m_locationEdit = new QLineEdit(this);
    m_locationEdit->setReadOnly(true);
    m_locationEdit->setPlaceholderText("Папка модов автоматически не найдена - выберите вручную");

    const QString modsPath = findEverlastingSummerMods();
    if (!modsPath.isEmpty())
        m_locationEdit->setText(modsPath);

    auto *browseButton = new QPushButton("Обзор...", this);
    browseButton->setFixedWidth(80);

    locationLayout->addWidget(locationLabel);
    locationLayout->addWidget(m_locationEdit);
    locationLayout->addWidget(browseButton);

    connect(browseButton, &QPushButton::clicked, this, &CreateProjectDialog::chooseLocation);

    mainLayout->addLayout(locationLayout);

    auto *foldersGroup = new QGroupBox("Папки проекта:", this);
    auto *foldersLayout = new QGridLayout(foldersGroup);

    m_imagesCheck = new QCheckBox("images", foldersGroup);
    m_soundsCheck = new QCheckBox("sound", foldersGroup);
    m_scriptsCheck = new QCheckBox("script", foldersGroup);

    foldersLayout->addWidget(m_imagesCheck, 0, 0);
    foldersLayout->addWidget(m_soundsCheck, 0, 1);
    foldersLayout->addWidget(m_musicsCheck, 1, 0);
    foldersLayout->addWidget(m_scriptsCheck, 1, 1);

    mainLayout->addWidget(foldersGroup);

    auto *imagesGroup = new QGroupBox("Подпапки images:", this);
    auto *imagesLayout = new QHBoxLayout(imagesGroup);

    m_bgCheck = new QCheckBox("bg", imagesGroup);
    m_cgCheck = new QCheckBox("cg", imagesGroup);
    m_spritesCheck = new QCheckBox("sprites",imagesGroup);

    imagesLayout->addWidget(m_bgCheck);
    imagesLayout->addWidget(m_cgCheck);
    imagesLayout->addWidget(m_spritesCheck);
    imagesLayout->addStretch();

    imagesGroup->setEnabled(false);

    mainLayout->addWidget(imagesGroup);

    connect(m_imagesCheck, &QCheckBox::toggled, imagesGroup, &QWidget::setEnabled);

    mainLayout->addStretch();

    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    auto *createButton = new QPushButton("Создать", this);
    auto *cancelButton = new QPushButton("Отмена", this);

    buttonLayout->addWidget(createButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(buttonLayout);

    connect(createButton, &QPushButton::clicked, this, &CreateProjectDialog::createProject);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

QString CreateProjectDialog::projectPath() const
{
    return m_projectPath;
}

void CreateProjectDialog::chooseLocation()
{
    const QString initialDirectory = m_locationEdit->text().isEmpty() ? QDir::homePath() : m_locationEdit->text();
    const QString directory = QFileDialog::getExistingDirectory(this, "Выберите место для проекта", initialDirectory, QFileDialog::ShowDirsOnly);
    if (!directory.isEmpty())
        m_locationEdit->setText(directory);
}

void CreateProjectDialog::createProject()
{
    const QString name = m_nameEdit->text().trimmed();
    const QString parentDirectory = m_locationEdit->text().trimmed();
    if (name.isEmpty()) {
        QMessageBox::warning(this, "Создание проекта", "Введите название проекта.");
        m_nameEdit->setFocus();
        return;
    }
    if (parentDirectory.isEmpty() || !QDir(parentDirectory).exists()) {
        QMessageBox::warning(this, "Создание проекта", "Выберите существующую папку для сохранения проекта.");
        return;
    }
    QStringList folders;

    if (m_imagesCheck->isChecked()) {
        folders.append("images");
        if (m_bgCheck->isChecked())
            folders.append("images/bg");
        if (m_cgCheck->isChecked())
            folders.append("images/cg");
        if (m_spritesCheck->isChecked())
            folders.append("images/sprites");
    }
    if (m_soundsCheck->isChecked())
        folders.append("sound");
    if (m_scriptsCheck->isChecked())
        folders.append("script");

    QString createPath;
    QString errorMessage;

    if (!ProjectCreator::createProject(parentDirectory, name, folders, &createPath, &errorMessage)) {
        QMessageBox::warning(this, "Не удалось создать проект", errorMessage);
        return;
    }
    m_projectPath = createPath;
    accept();
}