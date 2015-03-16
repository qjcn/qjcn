#include <unistd.h>
#include <iostream>
#include <QtCore/QRegExp>
#include <QtCore/QSysInfo>
#include <QtGui/QtGui>

#ifdef WITH_QT5
	#include <QtWidgets/QInputDialog>
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

#include "jcn_data.h"
#include "n2t.h"


// ------------------------------------------------------------------
// ------------------------------------------------------------------
// db tab slots
// ------------------------------------------------------------------
// ------------------------------------------------------------------

void 
Jcn::add_new_to_db_clicked()
{
QString logStr;

	logStr = "start - add_new_to_db_clicked()";
	dbglog(logStr, 10);

	bool ok;
    QString dispStr = QInputDialog::getText(this, tr("Add Entry"),
                                          tr("Text:"), QLineEdit::Normal,
                                          "", &ok);
	if (ok && !dispStr.isEmpty())
	{	
		QByteArray workStr = dispStr.toLatin1().simplified().toUpper();
		
		if (workStr.length() == 0)	return;

		if (! isLineValid(workStr) )
		{
			QMessageBox msgBox;
			msgBox.setText("Invalid data: " + workStr);
			msgBox.exec();
			return;
		}

		QByteArray logText = "add_new_to_db_clicked: " + workStr;
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

		if (workStr.length() == 0)	return;

		if (! isLineValid(workStr) )
		{
			QMessageBox msgBox;
			msgBox.setText("Invalid data: " + workStr);
			msgBox.exec();
			return;
		}	

		JcnCalc calc(workStr);

		// add to sql db
		QSqlQuery query  = _db.exec(calc.toSqlInsert());
		if ( query.lastError().isValid() )  // there is an error
		{
			std::cout << "QRY Error: " << query.lastError().databaseText() << std::endl;
			std::cout << "   " << calc.toSqlInsert() << std::endl;
			QByteArray logText = "QRY Error: ";
			logText += " dbt=" + query.lastError().databaseText().toLatin1();
			logText += " drt=" + query.lastError().driverText().toLatin1();
			logText += " n=" + QByteArray::number(query.lastError().number());
			logText += " [" + calc.text() + "]";
			JCNLOG (logText);

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
		else // no errors add to text file
		{
		   	QFile file(_jcnTxtDbFn);
		   	if(! file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
		   	{
	   			QMessageBox msgBox;
	 			msgBox.setText("Can't open to file: " + _jcnTxtDbFn);
 				msgBox.exec();
				return;
			}
		
			file.write("\r\n" + calc.text() + "\r\n"); // use CR and NL			
			file.close();
			
			_sqlDbModel->load();  // reload model
		}
	}	 		

	logStr = "finish - add_new_to_db_clicked()";
	dbglog(logStr, 10);
}
// --------------------------------------------------------------------
void 
Jcn::rmv_from_db_clicked()
{
QString logStr;

	logStr = "start - rmv_from_db_clicked()";
	dbglog(logStr, 10);

	JcnSqlModel * m =	(JcnSqlModel*)_ui.db_View->model();

	QList<QModelIndex> list = _ui.db_View->selectionModel()->selectedRows(0);
	if (list.size() == 0) 
	{
		std::cout << "nothing selected - return" << std::endl;
		return; // nothing selected
	}
	
	// only chose one entry in case it is a multi selection
	// safer this way
	int itemRow = list.at(0).row();
	QVariant var = m->data(m->index(itemRow, 0, QModelIndex()));

	// extract text from this entry, clean it up and convert
	QByteArray workStr = var.toString().toLatin1().simplified().toUpper();
	if (workStr.length() == 0)	return;
	
	// std::cout << "[" << workStr << "]" << std::endl;

	JCNLOG("rmv_from_db_clicked " + workStr);
	
	JcnCalc calc(workStr);
	QSqlQuery query  = _db.exec(calc.toSqlDelete());
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

		QMessageBox msgBox;
		msgBox.setText("DELETE QRY Error: " + query.lastError().databaseText());
		msgBox.exec();
	}
	else // no errors - delete from text file
	{
		// save current db as a bak file
		QString backfn = _jcnTxtDbFn+".bak."+QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
   		QFile tmpfile(_jcnTxtDbFn);
		if ( ! tmpfile.rename(backfn) )
		{
			QMessageBox msgBox;
			JCNLOG("Can backup to file: " + backfn.toLatin1());
 			msgBox.setText("Can backup to file: <b>" + backfn + "<b>");
 			msgBox.exec();
			return;
		}
		
		// lets open new db file
		// lets start reading backup and wring into new db
   		QFile dbfile(_jcnTxtDbFn);
   		if(! dbfile.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append))
   		{
   			QMessageBox msgBox;

			JCNLOG("Can't open file: " + _jcnTxtDbFn);
			msgBox.setText("Can't open file: " + _jcnTxtDbFn);
			msgBox.exec();
			
			// we failed - let's return backup to original db
	   		QFile tmpfile(backfn);
			tmpfile.rename(_jcnTxtDbFn);
			return;
		}


   		QFile bak_dbfile(backfn);
   		if(! bak_dbfile.open(QIODevice::ReadOnly | QIODevice::Text ) )
   		{
   			QMessageBox msgBox;
			JCNLOG("Can't open file: " + backfn.toLatin1());
			msgBox.setText("Can't open file: " + backfn);
			msgBox.exec();

			// we failed - let's return backup to original db
	   		QFile tmpfile(backfn);
			tmpfile.rename(_jcnTxtDbFn);
			return;
		}

		QByteArray line;
		while( !bak_dbfile.atEnd() )
		{
			// lets read until we find match
	   		// strip leading and trailing spaces, convert to upper case
			line = bak_dbfile.readLine().simplified().toUpper();
			if (line[0] == '#' || line.length() == 0) continue;

			if (line == workStr)
				continue;

			// check line for invalid chars
			if (! isLineValid(line) )
			{
				continue;
			}
			
			dbfile.write(line + "\r\n");
		}
		bak_dbfile.close();
		dbfile.close();
		
		// reload sql db
		_sqlDbModel->load();  // reload model
	}

	logStr = "finish - rmv_from_db_clicked()";
	dbglog(logStr, 10);
}
// --------------------------------------------------------------------
void 
Jcn::find_db_clicked()
{
QString logStr;

	logStr = "start - find_db_clicked()";
	dbglog(logStr, 10);

	JcnSqlModel * m =	(JcnSqlModel*)_ui.db_View->model();

	QList<QModelIndex> list = _ui.db_View->selectionModel()->selectedRows(0);
	if (list.size() == 0) return; // nothing selected

	// switch tab to find compare
	_ui.main_TW->setCurrentIndex(2);
	
	// only chose one
	QVariant var = m->data(m->index(list.at(0).row(), 0, QModelIndex()));
	QByteArray workStr = var.toString().toLatin1().simplified();

	// set search string
	_ui.find_jcnText_LE->setText(QString(workStr));
	
	// click button to execute it
	_ui.find_calc_PB->click();

	logStr = "finish - find_db_clicked()";
	dbglog(logStr, 10);

}
// ------------------------------------------------------------------
void 
Jcn::search_textChanged(const QString & str)
{
	JcnSqlModel * mdl = (JcnSqlModel*)_ui.db_View->model();

//std::cout << "search_textChanged: " << str << std::endl;

	int rows = mdl->rowCount();
	for (int i=0; i < rows; ++i)
	{
		QString s = mdl->record(i).value("jcntext").toString();
//std::cout << s << std::endl;
		if ( s.contains( str.toUpper()) )
		{
			_ui.db_View->clearSelection();
			_ui.db_View->selectRow(i);
			break;
		}
	}
}
// --------------------------------------------------------------------
void 
Jcn::db_filters_clicked()
{
	if (jcndbg)
		std::cout << "find_options_clicked" << std::endl;

	if ( _ui.filter_Frame->isHidden() )
	{
		_ui.filter_Frame->show();
		_ui.filter_Frame->setVisible(true);
	}
	else
	{
		_ui.filter_Frame->hide();
		_ui.filter_Frame->setVisible(false);
	}
}
// ------------------------------------------------------------------
void 
Jcn::textFilter_textChanged(const QString & str)
{
	if (jcndbg)
		std::cout << "textFilter_textChanged: " << std::endl;

	_ui.numFilterDb_LE->setText("");
	_ui.sqlText_LE->setText("");
}
// ------------------------------------------------------------------
void 
Jcn::numFilter_textChanged(const QString & str)
{
	if (jcndbg)
		std::cout << "numFilter_textChanged: " << std::endl;

	_ui.textFilterDb_LE->setText("");
	_ui.sqlText_LE->setText("");
}
// ------------------------------------------------------------------
void 
Jcn::expFilter_textChanged(const QString & str)
{
	if (jcndbg)
		std::cout << "expFilter_textChanged: " << std::endl;

	_ui.textFilterDb_LE->setText("");
	_ui.numFilterDb_LE->setText("");
/*	
	if (str.simplified().size() == 0) // fill in sql text
	{
		 _ui.sqlText_LE->setText("select * from jcn_table ");
	}
*/
}
// ------------------------------------------------------------------
void 
Jcn::applyFilterDb_clicked()
{
	if (jcndbg)
		std::cout << "applyFilterDb_clicked: " << std::endl;

//	_tmpSqlDbModel->clearAndReset();

	// simple text filter only		
	QString text_str = _ui.textFilterDb_LE->text();
	if ( text_str != "")  // text filter
	{
		_ui.db_View->setModel(NULL);
		if (_ui.exactFilterDb_CB->isChecked())
			_tmpSqlDbModel->setQuery("SELECT * FROM jcn_table WHERE jcntext ='" + text_str + "'");
		else
			_tmpSqlDbModel->setQuery("SELECT * FROM jcn_table WHERE jcntext like '%" + text_str + "%'");

		if ( _tmpSqlDbModel->lastError().isValid() )
		{
			std::cout << _tmpSqlDbModel->lastError().text() << std::endl;
		}
		
		_ui.db_View->setModel(_tmpSqlDbModel);
		_tmpSqlDbModel->setHeaders();
	}


	// simple single number match - use expression
	QString num_str	= _ui.numFilterDb_LE->text();
	if ( num_str != "")	// num filter
	{
		_ui.db_View->setModel(NULL);

		QString qry_str = "SELECT * FROM jcn_table WHERE ";
		for (int c=1; c < JcnDD_Last; ++c)
		{
			if (_ui.exactFilterDb_CB->isChecked())
				qry_str += QString(JcnDD[c].sql_tag) + "='" + num_str + "'";
			else
				qry_str += QString(JcnDD[c].sql_tag) + " like '%" + num_str + "%'";

			if ( c < JcnDD_Last - 1) 
				qry_str += " or ";
		}
		
		_tmpSqlDbModel->setQuery(qry_str);
		if ( _tmpSqlDbModel->lastError().isValid() )
		{
			std::cout << _tmpSqlDbModel->lastError().text() << std::endl;
			std::cout << qry_str << std::endl;
		}
		
		_ui.db_View->setModel(_tmpSqlDbModel);
		_tmpSqlDbModel->setHeaders();
	}


	//----------------------------------------------------------------
	// general sql
	//----------------------------------------------------------------		
	QString sql_str	= _ui.sqlText_LE->text();
	if ( sql_str != "")	// general sql
	{
		sql_str = "select * from jcn_table where " + sql_str;

		_tmpSqlDbModel->setQuery(sql_str);

		if ( _tmpSqlDbModel->lastError().isValid() )
		{
			std::cout << _tmpSqlDbModel->lastError().text() << std::endl;

		    QMessageBox::StandardButton ret;

		    ret = QMessageBox::warning(this, tr("Invalid Query"),
                               _tmpSqlDbModel->lastError().text(),
                               QMessageBox::Ok);

	 	}
		else
		{
			_ui.db_View->setModel(_tmpSqlDbModel);
			_tmpSqlDbModel->setHeaders();
		}
	}

}
// ------------------------------------------------------------------
void 
Jcn::resetFilterDb_clicked()
{
	if (jcndbg)
		std::cout << "resetFilterDb_clicked: " << std::endl;
	
	_ui.textFilterDb_LE->setText("");
	_ui.numFilterDb_LE->setText("");
	_ui.sqlText_LE->setText("");
	
//	_tmpSqlDbModel->clearAndReset();
	_ui.db_View->setModel(_sqlDbModel);
	_sqlDbModel->setHeaders();
}
// ------------------------------------------------------------------
void 
Jcn::print_db_clicked()
{
	if (jcndbg)
		std::cout << "print db" << std::endl;
		
	QPrinter printer(QPrinter::HighResolution);
	printer.setOrientation ( QPrinter::Landscape );
//	printer.setPageMargins ( 0.05, 0.1, 0.05, 0.1, QPrinter::Inch );
	TDP * tdp = new TDP(_qapp, _ui.db_View, &printer, this);

	QByteArray workStr;
	QString text_str = _ui.textFilterDb_LE->text();
	QString num_str	 = _ui.numFilterDb_LE->text();
	QString exp_str	 = _ui.sqlText_LE->text();
	if ( text_str != "")  // text filter
	{
		workStr = text_str.toLatin1();
	}
	else
	if ( num_str != "")  // text filter
	{
		workStr = num_str.toLatin1();
	}
	else
	if ( exp_str != "")  // text filter
	{
		workStr = exp_str.toLatin1();
	}	
	
	tdp->setHeaderText("Filter: " + workStr);
	tdp->setGridMode(TDP::NormalGrid);
	tdp->exec();
	
	if (tdp->result() == QDialog::Accepted)
		tdp->print();
		
	delete tdp;
}
// --------------------------------------------------------------------
