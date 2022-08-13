#include "highlighter.h"
#include <QDialog>

Highlighter::Highlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{  
    lang="C++20";
    isOn=true;

    keywordPatterns = {
        QString("\\bauto\\b"), QString("\\bbreak\\b"), QString("\\bcase\\b"),
        QString("\\bchar\\b"), QString("\\bconst\\b"), QString("\\bcontinue\\b"),
        QString("\\bdefault\\b"), QString("\\bdo\\b"), QString("\\bdouble\\b"),
        QString("\\belse\\b"), QString("\\benum\\b"), QString("\\bextern\\b"),
        QString("\\bfloat\\b"), QString("\\bfor\\b"), QString("\\bgoto\\b"),
        QString("\\bif\\b"), QString("\\bint\\b"), QString("\\blong\\b"),
        QString("\\bregister\\b"), QString("\\breturn\\b"), QString("\\bshort\\b"),
        QString("\\bsigned\\b"), QString("\\bsizeof\\b"), QString("\\bstatic\\b"),
        QString("\\bstruct\\b"), QString("\\bswitch\\b"), QString("\\btypedef\\b"),
        QString("\\bunion\\b"), QString("\\bunsigned\\b"), QString("\\bvoid\\b"),
        QString("\\bvolatile\\b"), QString("\\bwhile\\b"), QString("\\binclude\\b"),
        QString("\\band\\b"), QString("\\band_eq\\b"), QString("\\basm\\b"),
        QString("\\bbitand\\b"), QString("\\bbitor\\b"), QString("\\bbool\\b"),
        QString("\\bcatch\\b"), QString("\\bchar\\b"), QString("\\bclass\\b"),
        QString("\\bcompl\\b"), QString("\\bconst_cast\\b"), QString("\\bdelete\\b"),
        QString("\\bdynamic_cast\\b"), QString("\\bexplicit\\b"), QString("\\bexport\\b"),
        QString("\\bfalse\\b"), QString("\\bfriend\b"), QString("\\bgoto\\b"),
        QString("\\binline\\b"), QString("\\bmutable\\b"), QString("\\bnamespace\\b"),
        QString("\\bnew\\b"), QString("\\bnot\\b"), QString("\\bnot_eq\\b"),
        QString("\\boperator\\b"), QString("\\bor\\b"), QString("\\bor_eq\\b"),
        QString("\\bprivate\\b"), QString("\\bprotected\\b"), QString("\\bpublic\\b"),
        QString("\\breinterpret_cast\\b"), QString("\\bstatic_cast\\b"), QString("\\bvolatile\\b"),
        QString("\\btemplate\\b"), QString("\\bthis\\b"), QString("\\bthrow\\b"),
        QString("\\btrue\\b"), QString("\\btry\\b"), QString("\\bunsigned\\b"),
        QString("\\btypeid\\b"), QString("\\btypename\\b"), QString("\\busing\\b"),
        QString("\\bvirtual\\b"), QString("\\bwchar_t\\b"), QString("\\bxor\\b"),
        QString("\\bxor_eq\\b"), QString("\\balignas\\b"), QString("\\balignof\\b"),
        QString("\\bchar8_t\\b"), QString("\\bchar16_t\\b"), QString("\\bchar32_t\\b"),
        QString("\\bconcept\\b"), QString("\\bconsteval\\b"), QString("\\bconstexpr\\b"),
        QString("\\bconstinit\\b"), QString("\\bco_await\\b"), QString("\\bco_return\\b"),
        QString("\\bco_yield\\b"), QString("\\bdecltype\\b"), QString("\\brequires\\b"),
        QString("\\bstatic_assert\\b"), QString("\\bthread_local\\b")
    };

    setDefault();
}

void Highlighter::highlightBlock(const QString &text)
{
    if(!isOn)
        return;

    /*for (const HighlightingRule &rule : highlightingRules) {
        int pos=0;
        //while((pos = rule.pattern.indexIn(text, pos)) != -1){
        while((pos = text.lastIndexOf(rule.pattern)) != -1){
            setFormat(pos, rule.pattern.captureCount(), rule.format);
            pos+=rule.pattern.captureCount();
            //setFormat(pos, rule.pattern.matchedLength(), rule.format);
            //pos+=rule.pattern.matchedLength();
        }

    }*/

    for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        //int endIndex = text.lastIndexOf(commentEndExpression);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            //+ commentEndExpression.matchedLength(); //match.capturedLength();
                            + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}

void Highlighter::setDefault(){
    HighlightingRule rule;
    highlightingRules.clear();
    style.keywordColor="darkBlue";
    style.classColor="darkMagenta";
    style.funcColor="blue";
    style.commsColor="darkGreen";

    keywordFormat.setForeground(QColor(style.keywordColor));
    keywordFormat.setFontWeight(QFont::Bold);

    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(QColor(style.classColor));

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(QColor(style.funcColor));

    singleLineCommentFormat.setForeground(QColor(style.commsColor));
    multiLineCommentFormat.setForeground(QColor(style.commsColor));

    quotationFormat.setForeground(QColor(style.commsColor));

    for (const QString &pattern : keywordPatterns){
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    rule.pattern = QRegularExpression(QString("\\bQ[A-Za-z]+\\b"));
    rule.format = classFormat;
    highlightingRules.append(rule);

    rule.pattern = QRegularExpression(QString("//[^\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    rule.pattern = QRegularExpression(QString("\".*\""));
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    rule.pattern = QRegularExpression(QString("\\b[A-Za-z0-9_]+(?=\\()"));
    rule.format = functionFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegularExpression(QString("/\\*"));
    commentEndExpression = QRegularExpression(QString("\\*/"));

    rehighlight();
}

void Highlighter::loadStyle(){

}

void Highlighter::changePattern(const QString &pattern){
    if(pattern==""){
        isOn=false;
        rehighlight();
        return;
    }

    lang = pattern;
    isOn = true;
    HighlightingRule rule;
    highlightingRules.clear();
    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);

    if(lang=="C89"){
    keywordPatterns = {
        QString("\\bauto\\b"), QString("\\bbreak\\b"), QString("\\bcase\\b"),
        QString("\\bchar\\b"), QString("\\bconst\\b"), QString("\\bcontinue\\b"),
        QString("\\bdefault\\b"), QString("\\bdo\\b"), QString("\\bdouble\\b"),
        QString("\\belse\\b"), QString("\\benum\\b"), QString("\\bextern\\b"),
        QString("\\bfloat\\b"), QString("\\bfor\\b"), QString("\\bgoto\\b"),
        QString("\\bif\\b"), QString("\\bint\\b"), QString("\\blong\\b"),
        QString("\\bregister\\b"), QString("\\breturn\\b"), QString("\\bshort\\b"),
        QString("\\bsigned\\b"), QString("\\bsizeof\\b"), QString("\\bstatic\\b"),
        QString("\\bstruct\\b"), QString("\\bswitch\\b"), QString("\\btypedef\\b"),
        QString("\\bunion\\b"), QString("\\bunsigned\\b"), QString("\\bvoid\\b"),
        QString("\\bvolatile\\b"), QString("\\bwhile\\b"), QString("\\binclude\\b")
    };
    }
    else if(lang=="C++03"){
    keywordPatterns = {
        QString("\\bauto\\b"), QString("\\bbreak\\b"), QString("\\bcase\\b"),
        QString("\\bchar\\b"), QString("\\bconst\\b"), QString("\\bcontinue\\b"),
        QString("\\bdefault\\b"), QString("\\bdo\\b"), QString("\\bdouble\\b"),
        QString("\\belse\\b"), QString("\\benum\\b"), QString("\\bextern\\b"),
        QString("\\bfloat\\b"), QString("\\bfor\\b"), QString("\\bgoto\\b"),
        QString("\\bif\\b"), QString("\\bint\\b"), QString("\\blong\\b"),
        QString("\\bregister\\b"), QString("\\breturn\\b"), QString("\\bshort\\b"),
        QString("\\bsigned\\b"), QString("\\bsizeof\\b"), QString("\\bstatic\\b"),
        QString("\\bstruct\\b"), QString("\\bswitch\\b"), QString("\\btypedef\\b"),
        QString("\\bunion\\b"), QString("\\bunsigned\\b"), QString("\\bvoid\\b"),
        QString("\\bvolatile\\b"), QString("\\bwhile\\b"), QString("\\binclude\\b"),
        QString("\\band\\b"), QString("\\band_eq\\b"), QString("\\basm\\b"),
        QString("\\bbitand\\b"), QString("\\bbitor\\b"), QString("\\bbool\\b"),
        QString("\\bcatch\\b"), QString("\\bchar\\b"), QString("\\bclass\\b"),
        QString("\\bcompl\\b"), QString("\\bconst_cast\\b"), QString("\\bdelete\\b"),
        QString("\\bdynamic_cast\\b"), QString("\\bexplicit\\b"), QString("\\bexport\\b"),
        QString("\\bfalse\\b"), QString("\\bfriend\b"), QString("\\bgoto\\b"),
        QString("\\binline\\b"), QString("\\bmutable\\b"), QString("\\bnamespace\\b"),
        QString("\\bnew\\b"), QString("\\bnot\\b"), QString("\\bnot_eq\\b"),
        QString("\\boperator\\b"), QString("\\bor\\b"), QString("\\bor_eq\\b"),
        QString("\\bprivate\\b"), QString("\\bprotected\\b"), QString("\\bpublic\\b"),
        QString("\\breinterpret_cast\\b"), QString("\\bstatic_cast\\b"), QString("\\bvolatile\\b"),
        QString("\\btemplate\\b"), QString("\\bthis\\b"), QString("\\bthrow\\b"),
        QString("\\btrue\\b"), QString("\\btry\\b"), QString("\\bunsigned\\b"),
        QString("\\btypeid\\b"), QString("\\btypename\\b"), QString("\\busing\\b"),
        QString("\\bvirtual\\b"), QString("\\bwchar_t\\b"), QString("\\bxor\\b"),
        QString("\\bxor_eq\\b")
    };
    }
    else if(lang=="C++20"){
    keywordPatterns = {
        QString("\\bauto\\b"), QString("\\bbreak\\b"), QString("\\bcase\\b"),
        QString("\\bchar\\b"), QString("\\bconst\\b"), QString("\\bcontinue\\b"),
        QString("\\bdefault\\b"), QString("\\bdo\\b"), QString("\\bdouble\\b"),
        QString("\\belse\\b"), QString("\\benum\\b"), QString("\\bextern\\b"),
        QString("\\bfloat\\b"), QString("\\bfor\\b"), QString("\\bgoto\\b"),
        QString("\\bif\\b"), QString("\\bint\\b"), QString("\\blong\\b"),
        QString("\\bregister\\b"), QString("\\breturn\\b"), QString("\\bshort\\b"),
        QString("\\bsigned\\b"), QString("\\bsizeof\\b"), QString("\\bstatic\\b"),
        QString("\\bstruct\\b"), QString("\\bswitch\\b"), QString("\\btypedef\\b"),
        QString("\\bunion\\b"), QString("\\bunsigned\\b"), QString("\\bvoid\\b"),
        QString("\\bvolatile\\b"), QString("\\bwhile\\b"), QString("\\binclude\\b"),
        QString("\\band\\b"), QString("\\band_eq\\b"), QString("\\basm\\b"),
        QString("\\bbitand\\b"), QString("\\bbitor\\b"), QString("\\bbool\\b"),
        QString("\\bcatch\\b"), QString("\\bchar\\b"), QString("\\bclass\\b"),
        QString("\\bcompl\\b"), QString("\\bconst_cast\\b"), QString("\\bdelete\\b"),
        QString("\\bdynamic_cast\\b"), QString("\\bexplicit\\b"), QString("\\bexport\\b"),
        QString("\\bfalse\\b"), QString("\\bfriend\b"), QString("\\bgoto\\b"),
        QString("\\binline\\b"), QString("\\bmutable\\b"), QString("\\bnamespace\\b"),
        QString("\\bnew\\b"), QString("\\bnot\\b"), QString("\\bnot_eq\\b"),
        QString("\\boperator\\b"), QString("\\bor\\b"), QString("\\bor_eq\\b"),
        QString("\\bprivate\\b"), QString("\\bprotected\\b"), QString("\\bpublic\\b"),
        QString("\\breinterpret_cast\\b"), QString("\\bstatic_cast\\b"), QString("\\bvolatile\\b"),
        QString("\\btemplate\\b"), QString("\\bthis\\b"), QString("\\bthrow\\b"),
        QString("\\btrue\\b"), QString("\\btry\\b"), QString("\\bunsigned\\b"),
        QString("\\btypeid\\b"), QString("\\btypename\\b"), QString("\\busing\\b"),
        QString("\\bvirtual\\b"), QString("\\bwchar_t\\b"), QString("\\bxor\\b"),
        QString("\\bxor_eq\\b"), QString("\\balignas\\b"), QString("\\balignof\\b"),
        QString("\\bchar8_t\\b"), QString("\\bchar16_t\\b"), QString("\\bchar32_t\\b"),
        QString("\\bconcept\\b"), QString("\\bconsteval\\b"), QString("\\bconstexpr\\b"),
        QString("\\bconstinit\\b"), QString("\\bco_await\\b"), QString("\\bco_return\\b"),
        QString("\\bco_yield\\b"), QString("\\bdecltype\\b"), QString("\\brequires\\b"),
        QString("\\bstatic_assert\\b"), QString("\\bthread_local\\b")
    };
    }

    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression(QString("\\bQ[A-Za-z]+\\b"));
    rule.format = classFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::red);
    rule.pattern = QRegularExpression(QString("//[^\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::red);

    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression(QString("\".*\""));
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegularExpression(QString("\\b[A-Za-z0-9_]+(?=\\()"));
    rule.format = functionFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegularExpression(QString("/\\*"));
    commentEndExpression = QRegularExpression(QString("\\*/"));

    rehighlight();
}

void Highlighter::changeStyle(const QString &kw,
                                     const QString &classColor,
                                     const QString &funcColor,
                                     const QString &commsColor){

    if(kw!="")          style.keywordColor=kw;
    if(classColor!="")  style.classColor=classColor;
    if(funcColor!="")   style.funcColor=funcColor;
    if(commsColor!="")  style.commsColor=commsColor;
    HighlightingRule rule;
    highlightingRules.clear();

    keywordFormat.setForeground(QColor(style.keywordColor));
    keywordFormat.setFontWeight(QFont::Bold);
    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }
    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(QColor(style.classColor));
    rule.pattern = QRegularExpression(QString("\\bQ[A-Za-z]+\\b"));
    rule.format = classFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(QColor(style.commsColor));
    rule.pattern = QRegularExpression(QString("//[^\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(QColor(style.commsColor));

    quotationFormat.setForeground(QColor(style.commsColor));
    rule.pattern = QRegularExpression(QString("\".*\""));
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(QColor(style.funcColor));
    rule.pattern = QRegularExpression(QString("\\b[A-Za-z0-9_]+(?=\\()"));
    rule.format = functionFormat;
    highlightingRules.append(rule);

    commentStartExpression = QRegularExpression(QString("/\\*"));
    commentEndExpression = QRegularExpression(QString("\\*/"));

    rehighlight();
}
