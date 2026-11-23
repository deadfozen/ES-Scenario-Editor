#include "menubar.h"

#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QKeySequence>

MenuActions createMenuBar(QMainWindow *w) {
    MenuActions a;
    QMenuBar *menuBar = w->menuBar();

    QMenu *fileMenu = menuBar->addMenu("Файл");

    a.openprojectAction = fileMenu->addAction("Открыть проект");
    a.openprojectAction->setShortcut(QKeySequence("Ctrl+Shift+O"));

    a.newprojectAction = fileMenu->addAction("Создать проект");
    a.newprojectAction->setShortcut(QKeySequence("Ctrl+Shift+N"));

    fileMenu->addSeparator();

    a.newfileAction = fileMenu->addAction("Создать файл");
    a.newfileAction->setShortcut(QKeySequence::New);

    a.openfileAction = fileMenu->addAction("Открыть файл");
    a.openfileAction->setShortcut(QKeySequence::Open);

    fileMenu->addSeparator();

    a.saveAction = fileMenu->addAction("Сохранить файл");
    a.saveAction->setShortcut(QKeySequence::Save);

    a.saveasAction = fileMenu->addAction("Сохранить файл как");
    a.saveasAction->setShortcut(QKeySequence::SaveAs);

    fileMenu->addSeparator();

    a.exitAction = fileMenu->addAction("Выход");
    a.exitAction->setShortcut(QKeySequence::Quit);

    // при возможности потом добавить закрытие файла или активного окна, но пока так

    QMenu *editMenu = menuBar->addMenu("Правка");

    a.undoAction = editMenu->addAction("Отменить");
    a.undoAction->setShortcut(QKeySequence::Undo);

    a.redoAction = editMenu->addAction("Повторить");
    a.redoAction->setShortcut(QKeySequence::Redo);

    editMenu->addSeparator();

    a.cutAction = editMenu->addAction("Вырезать");
    a.cutAction->setShortcut(QKeySequence::Cut);

    a.copyAction = editMenu->addAction("Копировать");
    a.copyAction->setShortcut(QKeySequence::Copy);

    a.pasteAction = editMenu->addAction("Вставить");
    a.pasteAction->setShortcut(QKeySequence::Paste);

    editMenu->addSeparator();

    a.selectAction = editMenu->addAction("Выделить");
    a.selectAction->setShortcut(QKeySequence::SelectAll);

    a.moreAction = editMenu->addAction("Дополнительно");

    editMenu->addSeparator();

    a.settingsAction = editMenu->addAction("Настройки");
    a.settingsAction->setShortcut(QKeySequence("Ctrl+,"));

    QMenu *viewMenu = menuBar->addMenu("Вид");

    a.panelAction = viewMenu->addAction("Показать панель проекта");
    a.panelAction->setShortcut(QKeySequence("Ctrl+1"));
    a.panelAction->setCheckable(true);
    a.panelAction->setChecked(true);

    a.assetsAction = viewMenu->addAction("Показать ассеты");
    a.assetsAction->setShortcut(QKeySequence("Ctrl+2"));
    a.assetsAction->setCheckable(true);
    a.assetsAction->setChecked(true);

    a.statusAction = viewMenu->addAction("Показать строку состояния");
    a.statusAction->setShortcut(QKeySequence("Ctrl+3"));
    a.statusAction->setCheckable(true);
    a.statusAction->setChecked(true);

    QMenu *helpMenu = menuBar->addMenu("Справка");

    a.aboutAction = helpMenu->addAction("О программе");
    a.aboutAction->setShortcut(QKeySequence::HelpContents);

    a.docAction = helpMenu->addAction("Документация по созданию модов");
    a.docAction->setShortcut(QKeySequence("F2"));

    return a;
}