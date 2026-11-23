#ifndef RENPYLEXER_H
#define RENPYLEXER_H

#include <Qsci/qscilexercustom.h>
#include <QString>

class RenPyLexer : public QsciLexerCustom
{
    Q_OBJECT

public:
    explicit RenPyLexer(QObject *parent = nullptr);

    const char *language() const override;
    QString description(int style) const override;
    QColor defaultColor(int style) const override;
    QFont defaultFont(int style) const override;

protected:
    void styleText(int start, int end) override;
};

#endif // RENPYLEXER_H
