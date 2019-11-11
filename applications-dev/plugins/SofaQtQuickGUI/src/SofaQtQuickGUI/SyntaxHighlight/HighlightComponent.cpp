#include "HighlightComponent.h"
#include "QSyntaxHighlighterPython.h"
#include "QSyntaxHighlighterCpp.h"
#include <QQuickTextDocument>

namespace sofaqtquick {


HighlightComponent::HighlightComponent(QObject* parent)
        : inherited(parent)
{
}

void HighlightComponent::onCompleted()
{
    auto property = parent()->property("textDocument");
    auto textDocument = property.value<QQuickTextDocument*>();
    QTextDocument* document = textDocument->textDocument();

    if (m_syntax == "py")
        m_highlight = new QSyntaxHighlighterPython(document);
    else if (m_syntax == "cpp")
        m_highlight = new QSyntaxHighlighterCpp(document);
}

void HighlightComponent::setText(const QString &newText)
{
    if (newText != m_text)
    {
        m_text = newText;
        emit textChanged();
    }
}

void HighlightComponent::setSyntax(const QString &newSyntax)
{
    if (newSyntax!= m_syntax)
    {
        m_syntax = newSyntax;
        emit textChanged();
    }
}

} // namespace sofaqtquick
