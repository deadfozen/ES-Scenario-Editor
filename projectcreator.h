#ifndef PROJECTCREATOR_H
#define PROJECTCREATOR_H

#include <QString>
#include <QStringList>

class ProjectCreator
{
public:
    static bool createProject(
        const QString &parentDirectory,
        const QString &projectName,
        const QStringList &folders,
        QString *createdPath = nullptr,
        QString *errorMessage = nullptr
    );
};

#endif // PROJECTCREATOR_H