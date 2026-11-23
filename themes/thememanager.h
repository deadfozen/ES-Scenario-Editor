
#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QString>
#include <QColor>

class QApplication;
class CodeEditor;

struct ThemeColors
{
    QColor window;
    QColor panel;
    QColor editor;
    QColor text;
    QColor secondaryText;
    QColor accent;
    QColor hover;
    QColor selection;
    QColor lineNumber;
    QColor caretLine;
};

class ThemeManager
{
public:
    static QString currentTheme();
    static void setTheme(const QString &themeId);

    static QStringList availableThemes();
    static QString themeName(const QString &themeId);
    static ThemeColors colors(const QString &themeId);

    static void apply(QApplication *app, CodeEditor *editor);

private:
    static QString m_currentTheme;
};

#endif
