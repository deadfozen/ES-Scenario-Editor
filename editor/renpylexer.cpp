
#include "renpylexer.h"

#include <Qsci/qsciscintilla.h>

#include <QColor>
#include <QFont>
#include <QRegularExpression>
#include <QByteArray>
#include <QVector>

RenPyLexer::RenPyLexer(QObject *parent)
    : QsciLexerCustom(parent)
{
}

const char *RenPyLexer::language() const
{
    return "Ren'Py";
}

QString RenPyLexer::description(int style) const
{
    switch (style) {
    case 0: return "Default";
    case 1: return "Keyword";
    case 2: return "String";
    case 3: return "Comment";
    case 4: return "Label";
    case 5: return "Number";
    case 6: return "Dollar";
    default: return "";
    }
}

QColor RenPyLexer::defaultColor(int style) const
{
    switch (style) {
    case 1: return QColor(86, 156, 214);
    case 2: return QColor(206, 145, 120);
    case 3: return QColor(87, 166, 74);
    case 4: return QColor(220, 220, 170);
    case 5: return QColor(181, 206, 168);
    case 6: return QColor(206, 145, 120);
    default: return QColor(212, 212, 212);
    }
}

QFont RenPyLexer::defaultFont(int style) const
{
    QFont font("Consolas", 12);
    font.setStyleHint(QFont::Monospace);

    if (style == 1 || style == 4)
        font.setBold(true);

    return font;
}

void RenPyLexer::styleText(int start, int end)
{
    if (start >= end)
        return;

    const QByteArray bytes = editor()->bytes(start, end);
    const int length = bytes.size();

    if (length == 0)
        return;

    QVector<int> styles(length, 0);

    const QString text = QString::fromUtf8(bytes);

    QVector<int> bytePositions(text.size() + 1);
    for (int i = 0; i <= text.size(); ++i)
        bytePositions[i] = text.left(i).toUtf8().size();

    auto applyMatches = [&](const QRegularExpression &regex,
                            int style) {
        auto iterator = regex.globalMatch(text);

        while (iterator.hasNext()) {
            const auto match = iterator.next();

            const int charStart = match.capturedStart();
            const int charEnd =
                charStart + match.capturedLength();

            if (charStart < 0 || charEnd > text.size())
                continue;

            const int byteStart = bytePositions[charStart];
            const int byteEnd = bytePositions[charEnd];

            for (int i = byteStart; i < byteEnd && i < length; ++i)
                styles[i] = style;
        }
    };

    applyMatches(
        QRegularExpression(
            "\"(?:\\\\.|[^\"\\\\])*\"|'(?:\\\\.|[^'\\\\])*'"
            ),
        2
        );

    const QRegularExpression commentRegex(
        "(?<![\\w\"'])#.*$",
        QRegularExpression::MultilineOption
        );

    auto comments = commentRegex.globalMatch(text);
    while (comments.hasNext()) {
        const auto match = comments.next();

        const int charStart = match.capturedStart();
        const int charEnd =
            charStart + match.capturedLength();

        if (charStart < 0 || charEnd > text.size())
            continue;

        const int byteStart = bytePositions[charStart];
        const int byteEnd = bytePositions[charEnd];

        bool insideString = false;
        for (int i = byteStart; i < byteEnd && i < length; ++i) {
            if (styles[i] == 2) {
                insideString = true;
                break;
            }
        }

        if (!insideString) {
            for (int i = byteStart; i < byteEnd && i < length; ++i)
                styles[i] = 3;
        }
    }

    const QStringList keywords = {
        "define", "scene", "show", "play", "return",
        "label", "if", "elif", "else", "while", "for"
    };

    for (const QString &keyword : keywords) {
        applyMatches(
            QRegularExpression(
                "\\b" + QRegularExpression::escape(keyword) + "\\b"
                ),
            1
            );
    }

    applyMatches(
        QRegularExpression("\\$\\s*\\w+"),
        6
        );

    applyMatches(
        QRegularExpression("^\\s*label\\s+\\w+:"),
        4
        );

    applyMatches(
        QRegularExpression("\\b\\d+\\b"),
        5
        );

    startStyling(start);

    int runStart = 0;
    int currentStyle = styles[0];

    for (int i = 1; i < length; ++i) {
        if (styles[i] != currentStyle) {
            setStyling(i - runStart, currentStyle);
            runStart = i;
            currentStyle = styles[i];
        }
    }

    setStyling(length - runStart, currentStyle);
}
