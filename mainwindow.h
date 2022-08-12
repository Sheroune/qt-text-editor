#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

#include "textedit.h"
#include "highlighter.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    QMenu *fileMenu;
    QMenu *correctionMenu;
    QMenu *formatMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;
    QToolBar *tb;

    QAction *actionMenu;
    QAction *actionUndo;
    QAction *actionRedo;
    QAction *actionCopy;
    QAction *actionCut;
    QAction *actionPaste;
    QAction *actionFind;
    QAction *actionReplace;
    QAction *actionSelectAll;
    QAction *actionLineBreak;
    QAction *actionLineArea;
    QAction *actionToolBar;
    QAction *actionStatusBar;
    QAction *actionSyntax;
    QAction *actionHg1;
    QAction *actionHg2;
    QAction *actionHg3;
    QAction *actionStyle1;
    QAction *actionStyle2;
    QAction *actionStyle3;

    QLineEdit *textSearch;
    QLineEdit *textReplace;
    QPushButton *buttonSearch;

    TextEdit *textEdit;
    QString fileName;
    QString synt;

    Highlighter *highlighter;

    QPushButton *bKeywords;
    QPushButton *bClass;
    QPushButton *bFunc;
    QPushButton *bComms;
    QPushButton *bSave;

    QString defKeywordColor,
            defClassColor,
            defFuncColor,
            defCommsColor;

    QLabel *mPos;
    QLabel *mTime;
    QLabel *mMeta;

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void createMenu();
    void createStatusBar();
    void setName(const QString &a);
    bool warningSave();
    bool load(const QString &f);

public slots:
    void menuNew();
    void menuOpen();
    bool menuSave();
    bool fileSaveAs();
    void menuSearch();
    void menuReplace();
    void highlightSearch();
    void highlightReplace();
    void backgroundColor();
    void lineColor();
    void selectAll();
    void toggleLineBreak();
    void onFont();
    void toggleLineNumber();
    void toggleToolBar();
    void toggleStatusBar();
    void toggleSyntax();
    void toggleHighlight1();
    void toggleHighlight2();
    void toggleHighlight3();
    void setDef();
    void setCust();
    void customStyle();
    void saveStyle();
    void getKeywordColor();
    void getClassColor();
    void getFuncColor();
    void getCommsColor();
    void about();
    void mousePos();
    void timeChanged();
    void metaNums();

};

#endif // MAINWINDOW_H
