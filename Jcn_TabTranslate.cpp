/*

	handlers for Translate Tab
*/

#include <unistd.h>
#include <iostream>
#include <QtCore/QSysInfo>
#include <QtGui/QtGui>

#ifdef WITH_QT5
	#include <QtWidgets/QMessageBox>
	#include <QtPrintSupport/QPrintDialog>
#else
	#include <QtGui/QMessageBox>
#endif

#include "tdp.h"

#include "DD.h"
#include "Jcn.h"
#include "JcnCalc.h"
#include "JcnSqlModel.h"


// ------------------------------------------------------------------
void
Jcn::translate_text_changed()
{
	QString dispStr =_ui.jcnText_LE->toPlainText();
	
	if ( ! dispStr.contains('\n') )
		return;
	
	dispStr.remove('\n');
	_ui.jcnText_LE->setHtml(dispStr);

	translate_clicked();
}
// ------------------------------------------------------------------
void 
Jcn::translate_clicked()
{
	if (jcndbg > 10)
		std::cout << "translate_clicked" << std::endl;

	QString dispStr = _ui.jcnText_LE->toPlainText();
	
	QByteArray workStr = _ui.jcnText_LE->toPlainText().toLatin1();
	workStr = workStr.simplified();

	int doDate = 0;
	if ( _ui.actionDateOrdinal->isChecked() )
		doDate = 2;
	else
	if ( _ui.actionDateCardinal->isChecked() )
		doDate = 1;

//std::cout << "ttt: " << _ui.actionTranslate_Coordinates->isChecked() << std::endl;
		
	n2t::enhStr(workStr, 
				_ui.actionTranslate_Numbers->isChecked(),
				doDate,
				_ui.actionTranslate_Roman_Num->isChecked(),
				_ui.actionTranslate_Phone->isChecked(),
				_ui.actionTranslate_Coordinates->isChecked(),				
				dispStr
				);
	
	workStr.prepend(_ui.jcnStr_LIST->currentItem()->text().toLatin1() + " ");
	workStr = workStr.simplified();
		
	if (workStr.length()==0) return;
	
	// update string with potential tags
	_ui.jcnText_LE->setHtml(dispStr);
	
	JcnCalc calc(workStr);
	
	_ui.jcnResult_TE->setText(
								calc.toHtml(_ui.showFwa_TB->isChecked(), 
											_ui.showBwa_TB->isChecked(),
											_ui.useRoman_TB->isChecked()
											)
								);
}
// --------------------------------------------------------------------
// this is executed from translate tab
// --------------------------------------------------------------------
void 
Jcn::add_to_db_clicked()
{
	if (jcndbg > 10)
		std::cout << "add_to_db_clicked" << std::endl;

	QString dispStr =_ui.jcnText_LE->toPlainText();
	
	QByteArray workStr = _ui.jcnText_LE->toPlainText().toLatin1();
	workStr = workStr.simplified().toUpper();

	if (workStr.length() == 0)	return;

	if (! isLineValid(workStr) )
	{
		QMessageBox msgBox;
		msgBox.setText("Invalid data: " + workStr);
		msgBox.exec();
		return;
	}	

	QByteArray logText = "add_to_db_clicked: " + workStr;
	JCNLOG (logText);

	int doDate = 0;
	if ( _ui.actionDateOrdinal->isChecked() )
		doDate = 2;
	else
	if ( _ui.actionDateCardinal->isChecked() )
		doDate = 1;

	n2t::enhStr(workStr, 
				_ui.actionTranslate_Numbers->isChecked(),
				doDate,
				_ui.actionTranslate_Roman_Num->isChecked(),
				_ui.actionTranslate_Phone->isChecked(),
				_ui.actionTranslate_Coordinates->isChecked(),
				dispStr
				);

	workStr.prepend(_ui.jcnStr_LIST->currentItem()->text().toLatin1() + " ");
	workStr = workStr.simplified().toUpper();

	if (workStr.length() == 0)	return;

	if (! isLineValid(workStr) )
	{
		QMessageBox msgBox;
		msgBox.setText("Invalid data: " + workStr);
		msgBox.exec();
		return;
	}	


	// update string with potential tags
	_ui.jcnText_LE->setHtml(dispStr);

	JcnCalc calc(workStr);

	// lets add to SQLDB
	QSqlQuery query  = _db.exec(calc.toSqlInsert());
	if ( query.lastError().isValid() )
	{
		std::cout << "QRY Error: " << query.lastError().databaseText() << std::endl;
		std::cout <<  "   " << calc.toSqlInsert() << std::endl;

		QByteArray logText = "QRY Error: ";
		logText += " dbt=" + query.lastError().databaseText().toLatin1();
		logText += " drt=" + query.lastError().driverText().toLatin1();
		logText += " n=" + QByteArray::number(query.lastError().number());
		logText += " [" + calc.text() + "]";
		JCNLOG(logText);

		if ( query.lastError().number() == 19) // dupe
		{
			QMessageBox msgBox;
			msgBox.setText("This string is already in your database: " + calc.text());
			msgBox.exec();				
		}
		else
		{
			QMessageBox msgBox;
			msgBox.setText("QRY Error: " + 
								query.lastError().databaseText() + " " + 
								query.lastError().driverText().toLatin1() + " " +
								QByteArray::number(query.lastError().number())
								);
			msgBox.exec();
		}
	}
	else  // no errors - write to text file
	{
		// also write to file
   		QFile file(_jcnTxtDbFn);
   		if(! file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append))
   		{
   			QMessageBox msgBox;
			msgBox.setText("Can't open to file: " + _jcnTxtDbFn);
			msgBox.exec();
			return;
		}

		file.write("\r\n" + calc.text() + "\r\n"); // use CR and NL front and back to make sure
		file.close();
	}
}
// --------------------------------------------------------------------
void 
Jcn::find_clicked()
{
	if (jcndbg > 10)
		std::cout << "find_clicked" << std::endl;

	_ui.main_TW->setCurrentIndex(2);

	QString dispStr =_ui.jcnText_LE->toPlainText();

	QByteArray workStr = _ui.jcnText_LE->toPlainText().toLatin1();
	workStr = workStr.simplified();

	int doDate = 0;
	if ( _ui.actionDateOrdinal->isChecked() )
		doDate = 2;
	else
	if ( _ui.actionDateCardinal->isChecked() )
		doDate = 1;

	n2t::enhStr(workStr, 
				_ui.actionTranslate_Numbers->isChecked(),
				doDate,
				_ui.actionTranslate_Roman_Num->isChecked(),
				_ui.actionTranslate_Phone->isChecked(),
				_ui.actionTranslate_Coordinates->isChecked(),
				dispStr
				);
	
	workStr.prepend(_ui.jcnStr_LIST->currentItem()->text().toLatin1() + " ");
	workStr = workStr.simplified();

	if (workStr.length()==0) return;
		
	// set search string
	_ui.find_jcnText_LE->setText(QString(workStr));
	
	// click button to execute it
	_ui.find_calc_PB->click();
}
// ------------------------------------------------------------------
void 
Jcn::print_result_clicked()
{
QString logStr;

	logStr = "start - print_result_clicked()";
	dbglog(logStr, 10);

    QPrinter printer;

    QPrintDialog *dialog = new QPrintDialog(&printer, this);
    dialog->setWindowTitle(tr("Print Document"));

    if (_ui.jcnResult_TE->textCursor().hasSelection())
    	dialog->addEnabledOption(QAbstractPrintDialog::PrintSelection);

	if (dialog->exec() != QDialog::Accepted)
    	return;

	_ui.jcnResult_TE->print(&printer);

	logStr = "finish - print_result_clicked()";
	dbglog(logStr, 10);
}
// ------------------------------------------------------------------
