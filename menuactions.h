    #ifndef MENUACTIONS_H
    #define MENUACTIONS_H

    #include <QAction>

    struct MenuActions {
        QAction *openprojectAction;
        QAction *newprojectAction;
        QAction *newfileAction;
        QAction *openfileAction;
        QAction *saveAction;
        QAction *saveasAction;
        QAction *exitAction;
        QAction *undoAction;
        QAction *redoAction;
        QAction *cutAction;
        QAction *copyAction;
        QAction *pasteAction;
        QAction *selectAction;
        QAction *moreAction;
        QAction *settingsAction;
        QAction *panelAction;
        QAction *assetsAction;
        QAction *statusAction;
        QAction *aboutAction;
        QAction *docAction;
    };

    #endif