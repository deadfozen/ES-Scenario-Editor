#include "codeeditor.h"
#include "renpylexer.h"

#include <QFont>

CodeEditor::CodeEditor(QWidget *parent) : QsciScintilla(parent)
{
    setupEditor();
}

void CodeEditor::setupEditor()
{
    QFont editorFont("Consolas", 12);
    editorFont.setStyleHint(QFont::Monospace);

    setFont(editorFont);
    setMarginsFont(editorFont);

    setMarginType(0, QsciScintilla::NumberMargin);
    setMarginLineNumbers(0, true);
    setMarginWidth(0, "00000");

    setPaper(QColor("#1a1a1a"));
    setColor(QColor("#d4d4d4"));
    setMarginsBackgroundColor(QColor("#202020"));
    setMarginsForegroundColor(QColor("#858585"));
    setCaretLineVisible(true);
    setCaretLineBackgroundColor(QColor("#242424"));

    setSelectionBackgroundColor(QColor("#2a2a2a"));
    setCaretForegroundColor(QColor("#fff"));

    setIndentationsUseTabs(false);
    setTabWidth(4);
    setAutoIndent(true);

    setWrapMode(QsciScintilla::WrapWord);

    setFrameShape(QFrame::NoFrame);

    setText(
        "# Сначала выберите папку проекта,\n"
        "# затем создайте или откройте файл"
    );

//    setLexer(new RenPyLexer(this));
}