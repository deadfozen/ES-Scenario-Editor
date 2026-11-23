#include "docsdialog.h"

#include <QMainWindow>
#include <QDialog>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QFile>

void showDocDialog(QMainWindow *parent) {
    QDialog dlg(parent);
    dlg.setWindowTitle("Документация по созданию модов");
    dlg.resize(900, 550);

    QTextEdit *text = new QTextEdit(&dlg);
    text->setReadOnly(true);

    QFile file(":/doc/start.md");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        const QString md = QString::fromUtf8(file.readAll());
        text->document()->setMarkdown(md);
        file.close();
    } else {
        text->setPlainText(
            "Не удалось открыть документацию:\n"
            "Проверьте, что файл docs/start.md добавлен в resources.qrc"
            );
    }

    QDialogButtonBox *btns =
        new QDialogButtonBox(QDialogButtonBox::Close, &dlg);

    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    layout->addWidget(text);
    layout->addWidget(btns);

    QObject::connect(btns, &QDialogButtonBox::rejected,
                     &dlg, &QDialog::reject);
    QObject::connect(btns, &QDialogButtonBox::clicked,
                     &dlg, &QDialog::accept);

    dlg.exec();
}