#ifndef PROJECTCREATEDIALOG_H
#define PROJECTCREATEDIALOG_H

#include <QDialog>
#include <QString>

class QLineEdit;
class QCheckBox;

class CreateProjectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CreateProjectDialog(QWidget *parent = nullptr);

    QString projectPath() const;

private slots:
    void chooseLocation();
    void createProject();

private:
    QLineEdit *m_nameEdit = nullptr;
    QLineEdit *m_locationEdit = nullptr;

    QCheckBox *m_imagesCheck = nullptr;
    QCheckBox *m_soundsCheck = nullptr;
    QCheckBox *m_musicsCheck = nullptr;
    QCheckBox *m_scriptsCheck = nullptr;

    QCheckBox *m_bgCheck = nullptr;
    QCheckBox *m_cgCheck = nullptr;
    QCheckBox *m_spritesCheck = nullptr;

    QString m_projectPath;
};

#endif // PROJECTCREATEDIALOG_H