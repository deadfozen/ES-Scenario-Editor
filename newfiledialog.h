#ifndef NEWFILEDIALOG_H
#define NEWFILEDIALOG_H

#include <QString>

class QWidget;

QString showNewFileDialog(QWidget *parent, QString &currentOpenFolder);

#endif // NEWFILEDIALOG_H