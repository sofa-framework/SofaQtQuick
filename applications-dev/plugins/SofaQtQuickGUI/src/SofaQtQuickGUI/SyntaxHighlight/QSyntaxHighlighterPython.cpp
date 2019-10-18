#include "QSyntaxHighlighterPython.h"
#include <iostream>
#include <string>
namespace sofa::qtquick {

QSyntaxHighlighterPython::QSyntaxHighlighterPython(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    const QString keywordPatterns[] = {
        QStringLiteral("\\bFalse\\b"), QStringLiteral("\\bNone\\b"), QStringLiteral("\\bTrue\\b"),
        QStringLiteral("\\band\\b"), QStringLiteral("\\bas\\b"), QStringLiteral("\\bassert\\b"),
        QStringLiteral("\\bbreak\\b"), QStringLiteral("\\bclass\\b"), QStringLiteral("\\bcontinue\\b"),
        QStringLiteral("\\bdef\\b"), QStringLiteral("\\bdel\\b"), QStringLiteral("\\belif\\b"),
        QStringLiteral("\\belse\\b"), QStringLiteral("\\bexcept\\b"), QStringLiteral("\\bfinally\\b"),
        QStringLiteral("\\bfor\\b"), QStringLiteral("\\bfrom\\b"), QStringLiteral("\\bglobal\\b"),
        QStringLiteral("\\bif\\b"), QStringLiteral("\\bimport\\b"), QStringLiteral("\\bin\\b"),
        QStringLiteral("\\bis\\b"), QStringLiteral("\\blambda\\b"), QStringLiteral("\\bnonlocal\\b"),
        QStringLiteral("\\bnot\\b"), QStringLiteral("\\bor\\b"), QStringLiteral("\\bpass\\b"),
        QStringLiteral("\\braise\\b"), QStringLiteral("\\breturn\\b"), QStringLiteral("\\btry\\b"),
        QStringLiteral("\\bwhile\\b"), QStringLiteral("\\bwith\\b"), QStringLiteral("\\byield\\b"),
    };
    for (const QString &pattern : keywordPatterns)
    {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression(QStringLiteral("\\bQ[A-Za-z]+\\b"));
    rule.format = classFormat;
    highlightingRules.append(rule);

    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
    rule.format = quotationFormat;
    highlightingRules.append(rule);
    rule.pattern = QRegularExpression(QStringLiteral("\'.*\'"));
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
    rule.format = functionFormat;
    highlightingRules.append(rule);
    rule.pattern = QRegularExpression(QStringLiteral(":"));
    rule.format = functionFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setFontItalic(true);
    singleLineCommentFormat.setForeground(Qt::gray);
    rule.pattern = QRegularExpression(QStringLiteral("#.*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setFontItalic(true);
    multiLineCommentFormat.setForeground(Qt::gray);

    commentStartExpression = QRegularExpression(QStringLiteral("\"\"\""));
    commentEndExpression = QRegularExpression(QStringLiteral("\"\"\""));
}


void QSyntaxHighlighterPython::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : qAsConst(highlightingRules))
    {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext())
        {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0)
    {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex+3);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1)
        {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        }
        else
        {
            commentLength = endIndex - startIndex
                            + match.capturedLength();
            setCurrentBlockState(0);
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}

}  // namespace sofa::qtquick
