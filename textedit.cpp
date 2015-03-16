#include "textedit.h"

#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QTextCodec>
#include <QtCore/QtDebug>
				
#ifdef WITH_QT5
	#include <QtPrintSupport/QPrintDialog>
	#include <QtPrintSupport/QPrinter>
	#include <QtPrintSupport/QPrintPreviewDialog>

	#include <QtGui/QClipboard>
	#include <QtGui/QFontDatabase>
	#include <QtGui/QTextCursor>
	#include <QtGui/QTextDocumentWriter>
	#include <QtGui/QTextList>
	#include <QtGui/QCloseEvent>
				
	#include <QtWidgets/QAction>
	#include <QtWidgets/QApplication>
	#include <QtWidgets/QColorDialog>
	#include <QtWidgets/QComboBox>
	#include <QtWidgets/QFontComboBox>
	#include <QtWidgets/QFileDialog>
	#include <QtWidgets/QMenu>
	#include <QtWidgets/QMenuBar>
	#include <QtWidgets/QTextEdit>
	#include <QtWidgets/QPlainTextEdit>
	#include <QtWidgets/QToolBar>
	#include <QtWidgets/QMessageBox>
#else
	#include <QtGui/QAction>
	#include <QtGui/QApplication>
	#include <QtGui/QClipboard>
	#include <QtGui/QColorDialog>
	#include <QtGui/QComboBox>
	#include <QtGui/QFontComboBox>
	#include <QtGui/QFileDialog>
	#include <QtGui/QFontDatabase>
	#include <QtGui/QMenu>
	#include <QtGui/QMenuBar>
	#include <QtGui/QPrintDialog>
	#include <QtGui/QPrinter>
	#include <QtGui/QTextEdit>
	#include <QtGui/QPlainTextEdit>
	#include <QtGui/QToolBar>
	#include <QtGui/QTextCursor>
	#include <QtGui/QTextDocumentWriter>
	#include <QtGui/QTextList>
	#include <QtGui/QCloseEvent>
	#include <QtGui/QMessageBox>
	#include <QtGui/QPrintPreviewDialog>
#endif

#include "FileChkSum.h"

const QString rsrcPath = ":/ted_images";

// ---------------------------------------------------------------------------
TextEdit::TextEdit(QWidget * parent)
    : QMainWindow(parent, Qt::Dialog)
{
	setWindowModality (Qt::ApplicationModal);
	
    setupFileActions();
    setupEditActions();


    {
        QMenu *helpMenu = new QMenu(tr("Help"), this);
        menuBar()->addMenu(helpMenu);
        helpMenu->addAction(tr("About"), this, SLOT(about()));
    }

    textEdit = new QPlainTextEdit(this);
//	textEdit->setLineWrapMode(QTextEdit::NoWrap);

    connect(textEdit, SIGNAL(currentCharFormatChanged(const QTextCharFormat &)),
            this, SLOT(currentCharFormatChanged(const QTextCharFormat &)));

    connect(textEdit, SIGNAL(cursorPositionChanged()),
            this, SLOT(cursorPositionChanged()));

    setCentralWidget(textEdit);
    textEdit->setFocus();
    setCurrentFileName(QString());


    connect(textEdit->document(), SIGNAL(modificationChanged(bool)),
            actionSave, SLOT(setEnabled(bool)));
    connect(textEdit->document(), SIGNAL(modificationChanged(bool)),
            this, SLOT(setWindowModified(bool)));
    connect(textEdit->document(), SIGNAL(undoAvailable(bool)),
            actionUndo, SLOT(setEnabled(bool)));
    connect(textEdit->document(), SIGNAL(redoAvailable(bool)),
            actionRedo, SLOT(setEnabled(bool)));

    setWindowModified(textEdit->document()->isModified());
    actionSave->setEnabled(textEdit->document()->isModified());
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

    connect(QApplication::clipboard(), SIGNAL(dataChanged()), this, SLOT(clipboardDataChanged()));

    QString initialFile = ":/example.html";
    const QStringList args = QCoreApplication::arguments();
    if (args.count() == 2)
        initialFile = args.at(1);

    if (!load(initialFile))
        fileNew();
}
// ---------------------------------------------------------------------------
void TextEdit::closeEvent(QCloseEvent *e)
{
    if (maybeSave())
        e->accept();
    else
        e->ignore();
}
// ---------------------------------------------------------------------------
void 
TextEdit::setupFileActions()
{
    QToolBar *tb = new QToolBar(this);
    tb->setWindowTitle(tr("File Actions"));
    addToolBar(tb);

    QMenu *menu = new QMenu(tr("&File"), this);
    menuBar()->addMenu(menu);

    QAction *a;

    a = new QAction(QIcon(rsrcPath + "/filenew.png"), tr("&New"), this);
    a->setShortcut(QKeySequence::New);
    connect(a, SIGNAL(triggered()), this, SLOT(fileNew()));
    tb->addAction(a);
    menu->addAction(a);

    a = new QAction(QIcon(rsrcPath + "/fileopen.png"), tr("&Open..."), this);
    a->setShortcut(QKeySequence::Open);
    connect(a, SIGNAL(triggered()), this, SLOT(fileOpen()));
    tb->addAction(a);
    menu->addAction(a);

    menu->addSeparator();

    actionSave = a = new QAction(QIcon(rsrcPath + "/filesave.png"), tr("&Save"), this);
    a->setShortcut(QKeySequence::Save);
    connect(a, SIGNAL(triggered()), this, SLOT(fileSave()));
    a->setEnabled(false);
    tb->addAction(a);
    menu->addAction(a);

    a = new QAction(tr("Save &As..."), this);
    connect(a, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
    menu->addAction(a);
    menu->addSeparator();

#ifndef QT_NO_PRINTER
    a = new QAction(QIcon(rsrcPath + "/fileprint.png"), tr("&Print..."), this);
    a->setShortcut(QKeySequence::Print);
    connect(a, SIGNAL(triggered()), this, SLOT(filePrint()));
    tb->addAction(a);
    menu->addAction(a);

    a = new QAction(QIcon(rsrcPath + "/fileprint.png"), tr("Print Preview..."), this);
    connect(a, SIGNAL(triggered()), this, SLOT(filePrintPreview()));
    menu->addAction(a);

    a = new QAction(QIcon(rsrcPath + "/exportpdf.png"), tr("&Export PDF..."), this);
    a->setShortcut(Qt::CTRL + Qt::Key_D);
    connect(a, SIGNAL(triggered()), this, SLOT(filePrintPdf()));
    tb->addAction(a);
    menu->addAction(a);

    menu->addSeparator();
#endif

    a = new QAction(tr("&Quit"), this);
    a->setShortcut(Qt::CTRL + Qt::Key_Q);
    connect(a, SIGNAL(triggered()), this, SLOT(close()));
    menu->addAction(a);
}
// ---------------------------------------------------------------------------
void 
TextEdit::setupEditActions()
{
    QToolBar *tb = new QToolBar(this);
    tb->setWindowTitle(tr("Edit Actions"));
    addToolBar(tb);

    QMenu *menu = new QMenu(tr("&Edit"), this);
    menuBar()->addMenu(menu);

    QAction *a;
    a = actionUndo = new QAction(QIcon(rsrcPath + "/editundo.png"), tr("&Undo"), this);
    a->setShortcut(QKeySequence::Undo);
    tb->addAction(a);
    menu->addAction(a);
    a = actionRedo = new QAction(QIcon(rsrcPath + "/editredo.png"), tr("&Redo"), this);
    a->setShortcut(QKeySequence::Redo);
    tb->addAction(a);
    menu->addAction(a);
    menu->addSeparator();
    a = actionCut = new QAction(QIcon(rsrcPath + "/editcut.png"), tr("Cu&t"), this);
    a->setShortcut(QKeySequence::Cut);
    tb->addAction(a);
    menu->addAction(a);
    a = actionCopy = new QAction(QIcon(rsrcPath + "/editcopy.png"), tr("&Copy"), this);
    a->setShortcut(QKeySequence::Copy);
    tb->addAction(a);
    menu->addAction(a);
    a = actionPaste = new QAction(QIcon(rsrcPath + "/editpaste.png"), tr("&Paste"), this);
    a->setShortcut(QKeySequence::Paste);
    tb->addAction(a);
    menu->addAction(a);
    actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
}
// ---------------------------------------------------------------------------
bool TextEdit::load(const QString &f)
{
    if (!QFile::exists(f))
        return false;

    QFile file(f);
    if (!file.open(QFile::ReadOnly))
        return false;

    QByteArray data = file.readAll();
    textEdit->setPlainText(data);

    setCurrentFileName(f);
    return true;
}
// ---------------------------------------------------------------------------
bool 
TextEdit::maybeSave()
{
    if (!textEdit->document()->isModified())
        return true;

    if (fileName.startsWith(QLatin1String(":/")))
        return true;

    QMessageBox::StandardButton ret;
    ret = QMessageBox::warning(this, tr("Application"),
                               tr("The document has been modified.\n"
                                  "Do you want to save your changes?"),
                               QMessageBox::Save | QMessageBox::Discard
                               | QMessageBox::Cancel);

    if (ret == QMessageBox::Save)
        return fileSave();
    else 
	if (ret == QMessageBox::Cancel)
        return false;
    
	return true;
}
// ---------------------------------------------------------------------------
void 
TextEdit::setCurrentFileName(const QString &fileName)
{
    this->fileName = fileName;
    textEdit->document()->setModified(false);

    QString shownName;
    if (fileName.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = QFileInfo(fileName).fileName();

    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("Rich Text")));
    setWindowModified(false);
}
// ---------------------------------------------------------------------------
void 
TextEdit::fileNew()
{
    if (maybeSave()) 
	{
        textEdit->clear();
        setCurrentFileName(QString());
    }
}
// ---------------------------------------------------------------------------
void 
TextEdit::fileOpen()
{
    QString fn = QFileDialog::getOpenFileName(this, tr("Open File..."),
                                              QString(), tr("HTML-Files (*.htm *.html);;All Files (*)"));
    if (!fn.isEmpty())
        load(fn);
}
// ---------------------------------------------------------------------------
bool 
TextEdit::fileSave()
{
    if (fileName.isEmpty())
        return fileSaveAs();

    QTextDocumentWriter writer(fileName);
    bool success = writer.write(textEdit->document());
    if (success)
        textEdit->document()->setModified(false);
		
    return success;
}
// ---------------------------------------------------------------------------
bool 
TextEdit::fileSaveAs()
{
    QString fn = QFileDialog::getSaveFileName(this, tr("Save as..."),
                                              QString(),  tr("TXT files (*.txt);;All Files (*)"));
    if (fn.isEmpty())
        return false;

    if (! fn.endsWith(".txt", Qt::CaseInsensitive) )
        fn += ".txt"; // default

    setCurrentFileName(fn);
    return fileSave();
}
// ---------------------------------------------------------------------------
void 
TextEdit::filePrint()
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (textEdit->textCursor().hasSelection())
        dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
    dlg->setWindowTitle(tr("Print Document"));
    if (dlg->exec() == QDialog::Accepted) {
        textEdit->print(&printer);
    }
    delete dlg;
#endif
}
// ---------------------------------------------------------------------------
void 
TextEdit::filePrintPreview()
{
#ifndef QT_NO_PRINTER
    QPrinter printer(QPrinter::HighResolution);
    QPrintPreviewDialog preview(&printer, this);
    connect(&preview, SIGNAL(paintRequested(QPrinter *)), SLOT(printPreview(QPrinter *)));
    preview.exec();
#endif
}
// ---------------------------------------------------------------------------
/*
void 
TextEdit::printPreview(QPrinter * printer)
{
#ifdef QT_NO_PRINTER
    Q_UNUSED(printer);
#else
    textEdit->print(printer);
#endif
}
*/
// ---------------------------------------------------------------------------
void 
TextEdit::filePrintPdf()
{
#ifndef QT_NO_PRINTER
//! [0]
    QString fileName = QFileDialog::getSaveFileName(this, "Export PDF",
                                                    QString(), "*.pdf");
    if (!fileName.isEmpty()) {
        if (QFileInfo(fileName).suffix().isEmpty())
            fileName.append(".pdf");
        QPrinter printer(QPrinter::HighResolution);
        printer.setOutputFormat(QPrinter::PdfFormat);
        printer.setOutputFileName(fileName);
        textEdit->document()->print(&printer);
    }
//! [0]
#endif
}
// ---------------------------------------------------------------------------
void 
TextEdit::clipboardDataChanged()
{
    actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
}
// ---------------------------------------------------------------------------
void 
TextEdit::about()
{
    QMessageBox::about(this, tr("About"), tr("JCN File Edtor"));
}
// ---------------------------------------------------------------------------
