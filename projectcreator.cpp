#include "projectcreator.h"

#include <QDir>
#include <QFileInfo>
#include <QSet>

bool ProjectCreator::createProject(
    const QString &parentDirectory,
    const QString &projectName,
    const QStringList &folders,
    QString *createdPath,
    QString *errorMessage)
{
    const QString name = projectName.trimmed();
    const QString parentPath =
        QDir::cleanPath(parentDirectory);

    auto setError = [errorMessage](const QString &message) {
        if (errorMessage)
            *errorMessage = message;
    };

    if (name.isEmpty()) {
        setError("Введите название проекта.");
        return false;
    }

    if (name == "." || name == ".." ||
        name.contains('/') || name.contains('\\') ||
        name.contains(':') || name.contains('*') ||
        name.contains('?') || name.contains('"') ||
        name.contains('<') || name.contains('>') ||
        name.contains('|') ||
        name.endsWith('.') || name.endsWith(' ')) {
        setError("Название проекта содержит недопустимые символы.");
        return false;
    }

    const QString baseName =
        name.section('.', 0, 0).toUpper();

    const QSet<QString> reservedNames = {
        "CON", "PRN", "AUX", "NUL",
        "COM1", "COM2", "COM3", "COM4", "COM5",
        "COM6", "COM7", "COM8", "COM9",
        "LPT1", "LPT2", "LPT3", "LPT4", "LPT5",
        "LPT6", "LPT7", "LPT8", "LPT9"
    };

    if (reservedNames.contains(baseName)) {
        setError("Это название зарезервировано Windows.");
        return false;
    }

    const QFileInfo parentInfo(parentPath);

    if (!parentInfo.exists() || !parentInfo.isDir()) {
        setError("Выбранная родительская папка не существует.");
        return false;
    }

    QDir parentDir(parentPath);
    const QString fullPath = parentDir.filePath(name);

    if (QFileInfo::exists(fullPath)) {
        setError("Папка с таким названием уже существует.");
        return false;
    }

    if (!parentDir.mkdir(name)) {
        setError("Не удалось создать папку проекта.");
        return false;
    }

    QDir projectDir(fullPath);

    for (const QString &folder : folders) {
        if (!projectDir.mkpath(folder)) {
            projectDir.removeRecursively();
            setError("Не удалось создать папку: " + folder);
            return false;
        }
    }

    if (createdPath)
        *createdPath = projectDir.absolutePath();

    return true;
}