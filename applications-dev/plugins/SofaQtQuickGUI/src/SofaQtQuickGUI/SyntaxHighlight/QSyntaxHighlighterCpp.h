#pragma once

#include <QSyntaxHighlighter>
#include <QRegularExpression>

namespace sofaqtquick
{

class QSyntaxHighlighterCpp : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    QSyntaxHighlighterCpp(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> highlightingRules;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};

}  // namespace sofaqtquick
