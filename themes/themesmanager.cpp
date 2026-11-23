
#include "thememanager.h"
#include "../editor/codeeditor.h"

#include <QApplication>
#include <QSettings>

QString ThemeManager::m_currentTheme = "dark";

QString ThemeManager::currentTheme()
{
    QSettings settings;
    return settings.value("appearance/theme", "dark").toString();
}

void ThemeManager::setTheme(const QString &themeId)
{
    if (!availableThemes().contains(themeId))
        return;

    m_currentTheme = themeId;

    QSettings settings;
    settings.setValue("appearance/theme", themeId);
}

QStringList ThemeManager::availableThemes()
{
    return {
        "dark",
        "light",
        "anime_sakura",
        "anime_night",
        "anime_neon"
    };
}

QString ThemeManager::themeName(const QString &themeId)
{
    if (themeId == "dark") return "Тёмная";
    if (themeId == "light") return "Светлая";
    if (themeId == "anime_sakura") return "Сакура";
    if (themeId == "anime_night") return "Ночное небо";
    if (themeId == "anime_neon") return "Неон";

    return "Неизвестная тема";
}

ThemeColors ThemeManager::colors(const QString &themeId)
{
    if (themeId == "light") {
        return {
            "#f0f0f0", "#ffffff", "#ffffff",
            "#222222", "#777777", "#008b8b",
            "#e5e5e5", "#cce8e8", "#777777", "#e8f3f3"
        };
    }

    if (themeId == "anime_sakura") {
        return {
            "#251b25", "#352638", "#2b202d",
            "#ffeafa", "#cdaec8", "#f28ab2",
            "#493447", "#70435e", "#c49bb9", "#493044"
        };
    }

    if (themeId == "anime_night") {
        return {
            "#111b2b", "#18263b", "#142238",
            "#e1edff", "#9eb4d4", "#63b8ff",
            "#263b58", "#31577c", "#7895b8", "#1c3450"
        };
    }

    if (themeId == "anime_neon") {
        return {
            "#191329", "#251c3a", "#20172f",
            "#f0e8ff", "#b9a9d8", "#bd83ff",
            "#382952", "#543b78", "#a18bc4", "#302044"
        };
    }

    // Тёмная тема по умолчанию.
    return {
        "#1a1a1a", "#1c1c1c", "#1a1a1a",
        "#d4d4d4", "#888888", "#008b8b",
        "#2a2a2a", "#333333", "#858585", "#242424"
    };
}

void ThemeManager::apply(QApplication *app, CodeEditor *editor)
{
    if (!app)
        return;

    const QString themeId = currentTheme();
    const ThemeColors c = colors(themeId);

    QString style = QString(R"(
        QMainWindow, QDialog, QWidget {
            background-color: %1;
            color: %2;
        }

        QTreeView, QTreeWidget, QListWidget {
            background-color: %3;
            color: %2;
            border: none;
            outline: none;
        }

        QTreeView::item:hover,
        QTreeWidget::item:hover,
        QListWidget::item:hover {
            background-color: %4;
        }

        QTreeView::item:selected,
        QTreeWidget::item:selected,
        QListWidget::item:selected {
            background-color: %8;
            color: %2;
        }

        QLabel {
            color: %2;
        }

        QPushButton {
            background-color: %4;
            color: %2;
            border: 1px solid %4;
            border-radius: 5px;
            padding: 6px 12px;
        }

        QPushButton:hover {
            border-color: %6;
        }

        QLineEdit, QTextEdit, QPlainTextEdit, QComboBox {
            background-color: %3;
            color: %2;
            border: 1px solid %4;
            border-radius: 4px;
            padding: 5px;
        }

        QStatusBar {
            background-color: %3;
            color: %2;
        }

        QSplitter::handle {
            background-color: %1;
        }
    )")
                        .arg(c.window.name())
                        .arg(c.text.name())
                        .arg(c.panel.name())
                        .arg(c.hover.name())
                        .arg(c.secondaryText.name())
                        .arg(c.accent.name())
                        .arg(c.lineNumber.name())
                        .arg(c.selection.name());

    app->setStyleSheet(style);

    if (editor) {
        editor->setPaper(c.editor);
        editor->setColor(c.text);
        editor->setMarginsBackgroundColor(c.panel);
        editor->setMarginsForegroundColor(c.lineNumber);
        editor->setCaretLineBackgroundColor(c.caretLine);
        editor->setSelectionBackgroundColor(c.selection);
        editor->setCaretForegroundColor(c.text);
    }
}
