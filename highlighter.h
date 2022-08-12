#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    Highlighter(QTextDocument *parent = 0);

    void setDefault();
    void changeStyle(const QString &kw,
                     const QString &classColor,
                     const QString &funcColor,
                     const QString &commsColor);
    void loadStyle();
    void changePattern(const QString &pattern);

protected:
    void highlightBlock(const QString &text) override;

private:
    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    struct HighlightingStyle
    {
        QString keywordColor;
        QString classColor;
        QString funcColor;
        QString commsColor;
    };

    QVector<HighlightingRule> highlightingRules;
    HighlightingStyle style;

    QStringList keywordPatterns;
    QString lang;
    bool isOn;

    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLineCommentFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;
};

#endif // HIGHLIGHTER_H
