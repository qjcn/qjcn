#ifndef TEXTEDIT_H
#define TEXTEDIT_H


#include <QtCore/QMap>
#include <QtCore/QPointer>

#ifdef WITH_QT5
	#include <QtWidgets/QMainWindow>
#else
	#include <QtGui/QMainWindow>
#endif


QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QComboBox)
QT_FORWARD_DECLARE_CLASS(QFontComboBox)
QT_FORWARD_DECLARE_CLASS(QTextEdit)
QT_FORWARD_DECLARE_CLASS(QPlainTextEdit)
QT_FORWARD_DECLARE_CLASS(QTextCharFormat)
QT_FORWARD_DECLARE_CLASS(QMenu)

class TextEdit : public QMainWindow
{
    Q_OBJECT

public:
    TextEdit(QWidget *parent = 0);

    bool load(const QString &f);
	
protected:
    virtual void closeEvent(QCloseEvent *e);

private:
    void setupFileActions();
    void setupEditActions();
    void setupTextActions();

    bool maybeSave();
    void setCurrentFileName(const QString &fileName);

private slots:
    void fileNew();
    void fileOpen();
    bool fileSave();
    bool fileSaveAs();
    void filePrint();
    void filePrintPreview();
    void filePrintPdf();

    void clipboardDataChanged();
    void about();

//    void printPreview(QPrinter *);

private:
    QAction *actionSave,
        *actionUndo,
        *actionRedo,
        *actionCut,
        *actionCopy,
        *actionPaste;

    QToolBar *			tb;
    QString				fileName;
    QPlainTextEdit *	textEdit;
};

#endif
