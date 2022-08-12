#include "mainwindow.h"

#include <QMenuBar>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextDocumentWriter>
#include <QFileDialog>
#include <QAction>
#include <QTextCodec>
#include <QIcon>
#include <QDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFontDialog>
#include <QLabel>
#include <QStatusBar>
#include <QColorDialog>
#include <QToolBar>
#include <QToolButton>
#include <QDateTime>

#include <QDebug>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    createMenu();
    createStatusBar();
    textEdit = new TextEdit(this);
    setCentralWidget(textEdit);
    //textEdit->setTabStopWidth(12);
    textEdit->setTabStopDistance(12);
    setName(QString());
    synt="C++20";

    connect(textEdit->document(), SIGNAL(modificationChanged(bool)),
            this, SLOT(setWindowModified(bool)));
    connect(textEdit->document(), SIGNAL(undoAvailable(bool)),
            actionUndo, SLOT(setEnabled(bool)));
    connect(textEdit->document(), SIGNAL(redoAvailable(bool)),
            actionRedo, SLOT(setEnabled(bool)));

    setWindowModified(textEdit->document()->isModified());

    actionUndo->setEnabled(textEdit->document()->isUndoAvailable());
    actionRedo->setEnabled(textEdit->document()->isRedoAvailable());

    connect(actionUndo, SIGNAL(triggered()), textEdit, SLOT(undo()));
    connect(actionRedo, SIGNAL(triggered()), textEdit, SLOT(redo()));

    actionCut->setEnabled(false);
    actionCopy->setEnabled(false);

    connect(actionCut, SIGNAL(triggered()), textEdit, SLOT(cut()));
    connect(actionCopy, SIGNAL(triggered()), textEdit, SLOT(copy()));
    connect(actionPaste, SIGNAL(triggered()), textEdit, SLOT(paste()));

    connect(textEdit, SIGNAL(copyAvailable(bool)), actionCut, SLOT(setEnabled(bool)));
    connect(textEdit, SIGNAL(copyAvailable(bool)), actionCopy, SLOT(setEnabled(bool)));

    highlighter = new Highlighter(textEdit->document());
    synt="C++20";

    defKeywordColor="darkBlue",
    defClassColor="darkMagenta",
    defFuncColor="blue",
    defCommsColor="darkGreen";

    connect(textEdit, SIGNAL(cursorPositionChanged()), this, SLOT(mousePos()));
    connect(textEdit, SIGNAL(textChanged()), this, SLOT(timeChanged()));
    connect(textEdit, SIGNAL(textChanged()), this, SLOT(metaNums()));
}

MainWindow::~MainWindow()
{
    
}

void MainWindow::createMenu(){
    fileMenu = menuBar()->addMenu(tr("&Файл"));
    correctionMenu = menuBar()->addMenu(tr("&Правка"));
    formatMenu = menuBar()->addMenu(tr("&Формат"));
    viewMenu = menuBar()->addMenu(tr("&Вид"));
    helpMenu = menuBar()->addMenu(tr("&Справка"));

    tb = new QToolBar(this);
    addToolBar(tb);

    actionMenu = new QAction(QIcon::fromTheme("document-new", QIcon(":/images/filenew.png")),
                             tr("&Новый"),this);
    actionMenu->setShortcut(QKeySequence::New);
    connect(actionMenu, SIGNAL(triggered()), this, SLOT(menuNew()));
    fileMenu->addAction(actionMenu);
    tb->addAction(actionMenu);

    actionMenu = new QAction(QIcon::fromTheme("document-open", QIcon(":/images/fileopen.png")),
                             tr("&Открыть"), this);
    actionMenu->setShortcut(QKeySequence::Open);
    connect(actionMenu, SIGNAL(triggered()), this, SLOT(menuOpen()));
    fileMenu->addAction(actionMenu);
    tb->addAction(actionMenu);

    actionMenu = new QAction(QIcon::fromTheme("document-save", QIcon(":/images/filesave.png")),
                             tr("&Сохранить"), this);
    actionMenu->setShortcut(QKeySequence::Save);
    connect(actionMenu, SIGNAL(triggered()), this, SLOT(menuSave()));
    fileMenu->addAction(actionMenu);
    tb->addAction(actionMenu);

    actionMenu = new QAction(QIcon::fromTheme("document-saveas", QIcon(":/images/filesave.png")),
                             tr("&Сохранить как"), this);
    actionMenu->setShortcut(QKeySequence::SaveAs);
    connect(actionMenu, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
    fileMenu->addAction(actionMenu);

    actionMenu = new QAction(QIcon::fromTheme("document-exit", QIcon(":/images/fileexit.png")),
                             tr("&Выход"), this);
    actionMenu->setShortcut(QKeySequence::Quit);
    connect(actionMenu, SIGNAL(triggered()), this, SLOT(close()));
    fileMenu->addAction(actionMenu);

    actionUndo = new QAction(QIcon::fromTheme("edit-undo", QIcon(":/images/editundo.png")),
                             tr("&Отменить"), this);
    actionUndo->setShortcut(QKeySequence::Undo);
    correctionMenu->addAction(actionUndo);

    tb->addSeparator();
    tb->addAction(actionUndo);

    actionRedo = new QAction(QIcon::fromTheme("edit-redo", QIcon(":/images/editredo.png")),
                             tr("&Повторить"), this);
    actionRedo->setShortcut(QKeySequence::Redo);
    correctionMenu->addAction(actionRedo);
    tb->addAction(actionRedo);

    actionCopy = new QAction(QIcon::fromTheme("edit-copy", QIcon(":/images/editcopy.png")),
                                      tr("&Копировать"), this);
    actionCopy->setPriority(QAction::LowPriority);
    actionCopy->setShortcut(QKeySequence::Copy);
    correctionMenu->addAction(actionCopy);
    tb->addAction(actionCopy);

    actionCut = new QAction(QIcon::fromTheme("edit-cut", QIcon(":/images/editcut.png")),
                                             tr("&Вырезать"), this);
    actionCut->setPriority(QAction::LowPriority);
    actionCut->setShortcut(QKeySequence::Cut);
    correctionMenu->addAction(actionCut);
    tb->addAction(actionCut);

    actionPaste = new QAction(QIcon::fromTheme("edit-paste", QIcon(":/images/editpaste.png")),
                                  tr("&Вставить"), this);
    actionPaste->setPriority(QAction::LowPriority);
    actionPaste->setShortcut(QKeySequence::Paste);
    correctionMenu->addAction(actionPaste);
    tb->addAction(actionPaste);

    actionFind = new QAction(QIcon::fromTheme("document-find", QIcon(":/images/find.png")),
                             tr("&Найти"), this);
    actionFind->setShortcut(QKeySequence::Find);
    correctionMenu->addAction(actionFind);
    connect(actionFind, SIGNAL(triggered()), this, SLOT(menuSearch()));

    actionReplace = new QAction(QIcon::fromTheme("document-findreplace", QIcon(":/images/findreplace.png")),
                                tr("&Найти и заменить"), this);
    actionReplace->setShortcut(QKeySequence::Replace);
    correctionMenu->addAction(actionReplace);
    connect(actionReplace, SIGNAL(triggered()), this, SLOT(menuReplace()));

    actionSelectAll = new QAction(QIcon::fromTheme("document-selectall", QIcon(":/images/selectall.png")),
                                  tr("&Выделить все"), this);
    actionSelectAll->setShortcut(QKeySequence::SelectAll);
    correctionMenu->addAction(actionSelectAll);
    connect(actionSelectAll, SIGNAL(triggered()), this, SLOT(selectAll()));

    actionLineBreak = new QAction(tr("&Перенос по словам"), this);
    actionLineBreak->setCheckable(true);
    actionLineBreak->setChecked(true);
    formatMenu->addAction(actionLineBreak);
    connect(actionLineBreak, SIGNAL(triggered()), this, SLOT(toggleLineBreak()));

    actionMenu = new QAction(tr("&Выбор шрифта"), this);
    formatMenu->addAction(actionMenu);
    connect(actionMenu, SIGNAL(triggered()), this, SLOT(onFont()));

    actionMenu = new QAction(tr("&Выбор цвета фона"), this);
    viewMenu->addAction(actionMenu);
    connect(actionMenu, SIGNAL(triggered()), this, SLOT(backgroundColor()));

    actionMenu = new QAction(tr("&Выбор цвета текущей строки"), this);
    viewMenu->addAction(actionMenu);
    connect(actionMenu, SIGNAL(triggered()), this, SLOT(lineColor()));

    actionLineArea = new QAction(tr("&Вкл/Выкл отображения нумерации строк"), this);
    actionLineArea->setCheckable(true);
    actionLineArea->setChecked(true);
    viewMenu->addAction(actionLineArea);
    connect(actionLineArea, SIGNAL(triggered()), this, SLOT(toggleLineNumber()));

    tb->addSeparator();
    QMenu *searchMenu = new QMenu(tr("&Найти"), this);
    searchMenu->addAction(actionFind);
    searchMenu->addAction(actionReplace);
    QToolButton *btn1 = new QToolButton(this);
    btn1->setFocusPolicy(Qt::NoFocus);
    btn1->setMenu(searchMenu);
    btn1->setDefaultAction(actionFind);
    btn1->setPopupMode(QToolButton::InstantPopup);
    tb->addWidget(btn1);

    actionToolBar = new QAction(tr("&Вкл/Выкл отображения панели инструментов"), this);
    actionToolBar->setCheckable(true);
    actionToolBar->setChecked(true);
    viewMenu->addAction(actionToolBar);
    connect(actionToolBar, SIGNAL(triggered()), this, SLOT(toggleToolBar()));

    actionStatusBar = new QAction(tr("&Вкл/Выкл отображения строки состояния"), this);
    actionStatusBar->setCheckable(true);
    actionStatusBar->setChecked(true);
    viewMenu->addAction(actionStatusBar);
    connect(actionStatusBar, SIGNAL(triggered()), this, SLOT(toggleStatusBar()));

    actionSyntax = new QAction(tr("&Вкл/Выкл подсветки синтаксиса"), this);
    actionSyntax->setCheckable(true);
    actionSyntax->setChecked(true);
    viewMenu->addAction(actionSyntax);
    connect(actionSyntax, SIGNAL(triggered()), this, SLOT(toggleSyntax()));

    QMenu *menuHighlight = new QMenu(tr("&Синтаксис"), this);
    actionHg1 = new QAction(tr("C++20"), this);
    actionHg2 = new QAction(tr("C++03"), this);
    actionHg3 = new QAction(tr("C89"), this);
    actionHg1->setCheckable(true);
    actionHg2->setCheckable(true);
    actionHg3->setCheckable(true);
    actionHg1->setChecked(true);
    menuHighlight->addAction(actionHg1);
    menuHighlight->addAction(actionHg2);
    menuHighlight->addAction(actionHg3);
    connect(actionHg1, SIGNAL(triggered()), this, SLOT(toggleHighlight1()));
    connect(actionHg2, SIGNAL(triggered()), this, SLOT(toggleHighlight2()));
    connect(actionHg3, SIGNAL(triggered()), this, SLOT(toggleHighlight3()));
    viewMenu->addMenu(menuHighlight);

    QMenu *menuStyle = new QMenu(tr("&Стиль"), this);
    actionStyle1 = new QAction(tr("Изменить стиль"), this);
    actionStyle2 = new QAction(tr("Загрузить стиль из файла"), this);
    actionStyle3 = new QAction(tr("Default"), this);
    actionMenu = new QAction(tr("Открыть папку с доступными стилями"), this);

    connect(actionStyle3, SIGNAL(triggered()), this, SLOT(setDef()));
    connect(actionStyle2, SIGNAL(triggered()), this, SLOT(setCust()));
    connect(actionStyle1, SIGNAL(triggered()), this, SLOT(customStyle()));
    connect(actionMenu, SIGNAL(triggered()), this, SLOT(setCust()));
    menuStyle->addAction(actionStyle1);
    menuStyle->addAction(actionStyle2);
    menuStyle->addAction(actionStyle3);
    menuStyle->addAction(actionMenu);
    viewMenu->addMenu(menuStyle);

    actionMenu = new QAction(tr("О программе"), this);
    connect(actionMenu, SIGNAL(triggered()), this, SLOT(about()));
    helpMenu->addAction(actionMenu);
}

void MainWindow::menuNew(){
    if(warningSave()){
        textEdit->clear();
        setName(QString());
    }
}

void MainWindow::setName(const QString &a){
    this->fileName = a;
    textEdit->document()->setModified(false);

    QString shownName;
    if (fileName.isEmpty())
        shownName = "Безымянный";
    else
        shownName = QFileInfo(fileName).fileName();

    if(shownName.size()>32) shownName = shownName.left(32) + "...";

    setWindowTitle("[*]"+shownName);
    setWindowModified(false);
}

bool MainWindow::warningSave(){
    if (!textEdit->document()->isModified())
        return true;
    if (fileName.startsWith(QLatin1String(":/")))
        return true;
    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, tr("Редактор"),
                               tr("Сохранить изменения?"),
                               QMessageBox::Save | QMessageBox::Discard
                               | QMessageBox::Cancel);
    if (ret == QMessageBox::Save)
        return menuSave();
    else if (ret == QMessageBox::Cancel)
        return false;
    return true;
}

bool MainWindow::menuSave(){
    if(fileName.isEmpty())
        return fileSaveAs();

    QTextDocumentWriter writer(fileName);
    bool success = writer.write(textEdit->document());
    if (success)
        textEdit->document()->setModified(false);
    return success;
}

bool MainWindow::fileSaveAs(){
    QString fn = QFileDialog::getSaveFileName(this, tr("Save as..."),
                                              QString(), tr("TXT files (*.txt);;All Files (*)"));
    if(fn.isEmpty())
        return false;
    if(!(fn.endsWith(".txt", Qt::CaseInsensitive)))
        fn += ".txt";
    setName(fn);
    return menuSave();
}

void MainWindow::menuOpen(){
    QString fn = QFileDialog::getOpenFileName(this, tr("Open File..."),
                                              QString(), tr("Header Files (*.h);;C Files (*.c);;CPP Files (*.cpp);;TXT Files (*.txt);;All Files (*)"));
    if (!fn.isEmpty())
        load(fn);
}

bool MainWindow::load(const QString &f){
    if (!QFile::exists(f))
        return false;
    QFile file(f);
    if (!file.open(QFile::ReadOnly))
        return false;

    QByteArray data = file.readAll();
    QTextCodec *codec = QTextCodec::codecForName("KOI8-R");
    QString str = codec->toUnicode(data);

    str = QString::fromLocal8Bit(data);
    textEdit->setPlainText(str);


    setName(f);
    return true;
}

void MainWindow::menuSearch(){
    QDialog *qd = new QDialog(this);
    qd->setWindowTitle("Поиск");

    QHBoxLayout *searchBox = new QHBoxLayout(qd);
    textSearch = new QLineEdit(qd);
    buttonSearch = new QPushButton("&Найти", qd);
    searchBox->addWidget(textSearch);
    searchBox->addWidget(buttonSearch);

    textEdit->moveCursor(QTextCursor::Start);

    connect(buttonSearch, SIGNAL(clicked()), this, SLOT(highlightSearch()));
    qd->show();
}

void MainWindow::highlightSearch(){
    QString s = textSearch->text();
    if(!textEdit->find(s)){
        textEdit->moveCursor(QTextCursor::Start);
        textEdit->find(s);
    }
}

void MainWindow::menuReplace(){
    QDialog *qd = new QDialog(this);
    qd->setWindowTitle("Поиск и замена");

    QHBoxLayout *searchBox = new QHBoxLayout(qd);
    textSearch = new QLineEdit(qd);
    textReplace = new QLineEdit(qd);
    buttonSearch = new QPushButton("&Найти и заменить", qd);
    searchBox->addWidget(textSearch);
    searchBox->addWidget(textReplace);
    searchBox->addWidget(buttonSearch);

    connect(buttonSearch, SIGNAL(clicked()), this, SLOT(highlightReplace()));
    qd->show();
}

void MainWindow::highlightReplace(){
    textEdit->moveCursor(QTextCursor::Start);
    while(textEdit->find(textSearch->text())){
        textEdit->textCursor().insertText(textReplace->text());
    }
    textEdit->moveCursor(QTextCursor::End);
}

void MainWindow::selectAll(){
    textEdit->selectAll();
}

void MainWindow::toggleLineBreak(){
    if(actionLineBreak->isChecked())
        textEdit->setLineWrapMode(QPlainTextEdit::LineWrapMode::WidgetWidth);
    else
        textEdit->setLineWrapMode(QPlainTextEdit::LineWrapMode::NoWrap);
}

void MainWindow::onFont(){
    bool ok;
    QFont font = QFontDialog::getFont(&ok, QFont("Arial", 12), this, tr("Выберите шрифт"));
    if(ok){
        textEdit->setFont(font);
    }
}

void MainWindow::createStatusBar(){
    statusBar()->setSizeGripEnabled(false);
    //int s = textEdit->textCursor().position();
    //qDebug() << s;
    mPos = new QLabel("0 : 0", this);
    mTime = new QLabel(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm"), this);
    mMeta = new QLabel("0 | 0 | 0 | 0 KB", this);

    statusBar()->addWidget(mPos);
    statusBar()->addWidget(mTime);
    statusBar()->addWidget(mMeta);
}

void MainWindow::backgroundColor(){
    QColor col = QColorDialog::getColor(Qt::white, this);
    if(!col.isValid())
        return;
    QPalette p = textEdit->palette();
    p.setColor(QPalette::Base, col);
    textEdit->setPalette(p);
}

void MainWindow::lineColor(){
    QColor col = QColorDialog::getColor(Qt::yellow, this);
    if(!col.isValid())
        return;
    textEdit->changeLineColor(col);
}

void MainWindow::toggleLineNumber(){
    if(actionLineArea->isChecked())
        textEdit->showLineArea();
    else
        textEdit->hideLineArea();
}

void MainWindow::toggleToolBar(){
    if(actionToolBar->isChecked())
        tb->show();
    else
        tb->hide();
}

void MainWindow::toggleStatusBar(){
    if(actionStatusBar->isChecked())
        statusBar()->show();
    else
        statusBar()->hide();
}

void MainWindow::toggleSyntax(){
    if(actionSyntax->isChecked()){
        highlighter->changePattern(synt);
    }
    else
        highlighter->changePattern("");
}

void MainWindow::toggleHighlight1(){
    actionHg1->setChecked(true);
    actionHg2->setChecked(false);
    actionHg3->setChecked(false);
    synt = "C++20";
    if(actionSyntax->isChecked())   highlighter->changePattern(synt);


}
void MainWindow::toggleHighlight2(){
    actionHg2->setChecked(true);
    actionHg1->setChecked(false);
    actionHg3->setChecked(false);
    synt = "C++03";
    if(actionSyntax->isChecked())   highlighter->changePattern(synt);
}
void MainWindow::toggleHighlight3(){
    actionHg3->setChecked(true);
    actionHg1->setChecked(false);
    actionHg2->setChecked(false);
    synt = "C89";
    if(actionSyntax->isChecked())   highlighter->changePattern(synt);
}

void MainWindow::setDef(){
    highlighter->setDefault();
}

void MainWindow::setCust(){
    QDir dir = QDir::current();
    dir.cdUp();
    dir.cd("lab12/styles");
    QString fn = QFileDialog::getOpenFileName(this, tr("Open File..."),
                                              dir.absolutePath(), tr("Styles in ini format (*.ini);;All Files (*)"));
    if(fn.isEmpty())
        return;

    if (!QFile::exists(fn))
        return;
    QFile file(fn);
    if(!file.open(QFile::ReadOnly))
        return;

    QTextStream in(&file);
    QString kw="",cl="",fc="",cm="";
    while(!in.atEnd()){
        QString line = in.readLine();
        if(line.contains("keywordColor:")){
            QString ln = line.mid(14,line.size());
            ln.chop(1);
            kw=ln;
        }
        else if(line.contains("classColor:")){
            QString ln = line.mid(12,line.size());
            ln.chop(1);
            cl=ln;
        }
        else if(line.contains("funcColor:")){
            QString ln = line.mid(11,line.size());
            ln.chop(1);
            fc=ln;
        }
        else if(line.contains("commentsColor:")){
            QString ln = line.mid(15,line.size());
            ln.chop(1);
            cm=ln;
        }
    }
    highlighter->changeStyle(kw,cl,fc,cm);
}

void MainWindow::customStyle(){
    QDialog *qd = new QDialog(this);
    qd->setWindowTitle("Изменить стиль");

    QVBoxLayout *styleVBox = new QVBoxLayout(qd);
    QHBoxLayout *styleHBox = new QHBoxLayout();
    bKeywords = new QPushButton("&Цвет ключевых слов", qd);
    bClass = new QPushButton("&Цвет Qt классов", qd);
    bFunc = new QPushButton("&Цвет функций", qd);
    bComms = new QPushButton("&Цвет комментариев", qd);

    styleVBox->addLayout(styleHBox);

    styleHBox->addWidget(bKeywords);
    styleHBox->addWidget(bClass);
    styleHBox->addWidget(bFunc);
    styleHBox->addWidget(bComms);

    bSave = new QPushButton("&Сохранить стиль", qd);
    styleVBox->addWidget(bSave);
    connect(bKeywords, SIGNAL(clicked()), this, SLOT(getKeywordColor()));
    connect(bClass, SIGNAL(clicked()), this, SLOT(getClassColor()));
    connect(bFunc, SIGNAL(clicked()), this, SLOT(getFuncColor()));
    connect(bComms, SIGNAL(clicked()), this, SLOT(getCommsColor()));
    connect(bSave, SIGNAL(clicked()), this, SLOT(saveStyle()));
    qd->exec();
}

void MainWindow::saveStyle(){
    QDir dir = QDir::current();
    dir.cdUp();
    dir.cd("lab12/styles");
    QString fn = QFileDialog::getSaveFileName(this, tr("Save as..."),
                                              dir.absolutePath(), tr("Styles in ini format (*.ini);;All Files (*)"));

    if(fn.isEmpty())
        return;
    fn=fn+".ini";


    QFile file(fn);
    if(!file.open(QFile::WriteOnly))
        return;
    QTextStream stream(&file);
    QString line = "keywordColor: " + defKeywordColor + ";\n";
    stream << line;
    line = "classColor: " + defClassColor + ";\n";
    stream << line;
    line = "funcColor: " + defFuncColor + ";\n";
    stream << line;
    line = "commentsColor: " + defCommsColor + ";";
    stream << line;

    file.close();

    highlighter->changeStyle(defKeywordColor, defClassColor,
                             defFuncColor, defCommsColor);
}

void MainWindow::getKeywordColor(){
    QColor col = QColorDialog::getColor(Qt::darkBlue, this);
    if(!col.isValid())
        return;
    defKeywordColor = col.name();
}

void MainWindow::getClassColor(){
    QColor col = QColorDialog::getColor(Qt::darkMagenta, this);
    if(!col.isValid())
        return;
    defClassColor = col.name();
}
void MainWindow::getFuncColor(){
    QColor col = QColorDialog::getColor(Qt::blue, this);
    if(!col.isValid())
        return;
    defFuncColor = col.name();
}
void MainWindow::getCommsColor(){
    QColor col = QColorDialog::getColor(Qt::darkGreen, this);
    if(!col.isValid())
        return;
    defCommsColor = col.name();
}

void MainWindow::about(){
    QDialog *qd = new QDialog(this);
    qd->setWindowTitle("О программе");

    QVBoxLayout *styleVBox = new QVBoxLayout(qd);
    QPixmap mad(":/images/photo.png");
    QLabel *label = new QLabel("photo", qd);
    label->setPixmap(mad.scaled(150,140));
    styleVBox->addWidget(label);
    QString date = "Дата сборки программы: ";
    date.append(__DATE__);
    QLabel *label3 = new QLabel(date, qd);
    date = "Версия QT с которой собиралось: ";
    date.append(QT_VERSION_STR);
    QLabel *label4 = new QLabel(date, qd);
    date = "Версия QT с которой запущено: ";
    date.append(QT_VERSION_STR);
    QLabel *label5 = new QLabel(date, qd);

    styleVBox->addWidget(label3);
    styleVBox->addWidget(label4);
    styleVBox->addWidget(label5);
    QPushButton *btn1 = new QPushButton("Закрыть", qd);
    connect(btn1,SIGNAL(clicked()), qd, SLOT(close()));
    styleVBox->addWidget(btn1);
    styleVBox->setAlignment(label, Qt::AlignHCenter);


    qd->exec();
}

void MainWindow::mousePos(){
    QString s = QString::number(textEdit->textCursor().blockNumber()+1);
    QString s2 = QString::number(textEdit->textCursor().positionInBlock());
    s.append(" : ");
    s.append(s2);
    mPos->setText(s);
}

void MainWindow::timeChanged(){
    mTime->setText(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm"));
}

void MainWindow::metaNums(){
    QString s = QString::number(textEdit->document()->lineCount());
    //int wordCount = textEdit->toPlainText().split(QRegExp("(\\s|\\n|\\r)+"),
    //                                              QString::SkipEmptyParts).count();
    int wordCount = textEdit->toPlainText().split(QRegularExpression("(\\s|\\n|\\r)+"),
                                                  Qt::SkipEmptyParts).count();
    s.append(" | ");
    s.append(QString::number(wordCount));
    s.append(" | ");
    s.append(QString::number(textEdit->toPlainText().length()));
    s.append(" | ");
    float f = textEdit->toPlainText().toUtf8().size()/1024.0;
    s.append(QString::number(f));
    s.append(" KB");
    mMeta->setText(s);
}
