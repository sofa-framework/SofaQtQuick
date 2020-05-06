#pragma once

#include <QSyntaxHighlighter>

namespace sofaqtquick
{

class HighlightComponent : public QObject
{
Q_OBJECT
    //@formatter:off
    Q_PROPERTY(QString text
                       READ getText
                       WRITE setText
                       NOTIFY textChanged)

    //@formatter:on
    using inherited = QObject;
public:
    Q_PROPERTY(QString syntax
                       READ getSyntax
                       WRITE setSyntax
                       NOTIFY syntaxChanged)

    explicit HighlightComponent(QObject* parent = nullptr);

    static void registerQmlType();

    const QString& getText()
    {
        return m_text;
    }
    const QString& getSyntax()
    {
        return m_syntax;
    }

    void setText(const QString& newText);
    void setSyntax(const QString& newSyntax);

    Q_INVOKABLE void onCompleted();

signals:
    void textChanged();
    void syntaxChanged();

private:
    QSyntaxHighlighter* m_highlight;
    QString m_text = "";
    QString m_syntax = "py";
};




}  // namespace sofaqtquick

