#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QHash>
#include <QString>

class QListWidget;
class QStackedWidget;
class QPushButton;

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

signals:
    void themeChanged(const QString &themeId);

private:
    void setupUi();
    void setupThemesPage();

    void selectTheme(const QString &themeId);
    void updateThemeCards();

    QListWidget *m_navigation = nullptr;
    QStackedWidget *m_pages = nullptr;

    QHash<QString, QPushButton *> m_themeCards;
    QString m_selectedTheme;
};

#endif
