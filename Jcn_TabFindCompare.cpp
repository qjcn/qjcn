#include <unistd.h>
#include <iostream>
#include <QtCore/QSysInfo>
#include <QtGui/QtGui>

#ifdef WITH_QT5
	#include <QtWidgets/QProgressDialog>
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

//#include "jcn_data.h"
//#include "Express.h"
//#include "n2t.h"


// --------------------------------------------------------------
// --------------------------------------------------------------
// find/compare tab
// --------------------------------------------------------------


#define CREATE_MATCH_ALGO_MASK(k, ret) \
	(_ui.k ## _GB->isChecked() && _ui.k ## _exact_CB->isChecked() ? ret|= JcnMatchAlgo[JcnMacthAlgo_exact].mask : ret); \
	(_ui.k ## _GB->isChecked() && _ui.k ## _rev_CB->isChecked() ? ret|= JcnMatchAlgo[JcnMacthAlgo_rev].mask : ret);   \
	(_ui.k ## _GB->isChecked() && _ui.k ## _ign0_CB->isChecked() ? ret|= JcnMatchAlgo[JcnMacthAlgo_ign0].mask : ret);  \
	(_ui.k ## _GB->isChecked() && _ui.k ## _mul2_CB->isChecked() ? ret|= JcnMatchAlgo[JcnMacthAlgo_mul2].mask : ret);  \
	(_ui.k ## _GB->isChecked() && _ui.k ## _mul3_CB->isChecked() ? ret|= JcnMatchAlgo[JcnMacthAlgo_mul3].mask : ret);  \
	(_ui.k ## _GB->isChecked() && _ui.k ## _plus1_CB->isChecked() ? ret|= JcnMatchAlgo[JcnMacthAlgo_plus1].mask : ret);
						
// --------------------------------------------------------------

#define xstr(s)			zstr(s)
#define zstr(s)			#s
#define xcat2(a,b)		zstr(a) zstr(b)
#define xcat3(a,b,c)	zstr(a) zstr(b) zstr(c)

#define EXPR_MASK(g, k) \
	if (_ui.g ## _ ## k ## _CB->isChecked()) \
	{ \
		ret += xcat2(g,k); \
		ret += "="; \
		ret += t[JcnDD_ ## g ## _ ## k]; \
		ret += ","; \
	}

// --------------------------------------------------------------
QByteArray
Jcn::createExpressMatchFilter(JcnModel * mdl, const QByteArray & flag)
{
QByteArray ret="";

	if (mdl->rowCount() == 0) return ret;
	
	// create array of column tags
	QByteArray t[JcnDD_Last];
	for (int c=1; c < JcnDD_Last; ++c)
	{
		QStandardItem * oneItem = mdl->item(mdl->rowCount()-1, c);
		t[c]=oneItem->text().toLatin1();
	}


	if ( _ui.f_GB->isChecked() && flag == "f" )
	{
		EXPR_MASK(f,h);
		EXPR_MASK(f,v);
		EXPR_MASK(f,hv);
		EXPR_MASK(f,c);
		EXPR_MASK(f,p);
		EXPR_MASK(f,pc);
		EXPR_MASK(f,cl);
	}

	if ( _ui.b_GB->isChecked() && flag == "b"  )
	{
		EXPR_MASK(b,h);
		EXPR_MASK(b,v);
		EXPR_MASK(b,hv);
		EXPR_MASK(b,c);
		EXPR_MASK(b,p);
		EXPR_MASK(b,pc);
		EXPR_MASK(b,cl);
	}

	if ( _ui.fb_GB->isChecked() && flag == "fb"  )
	{
		EXPR_MASK(fb,h);
		EXPR_MASK(fb,v);
		EXPR_MASK(fb,hv);
		EXPR_MASK(fb,c);
		EXPR_MASK(fb,p);
		EXPR_MASK(fb,pc);
		EXPR_MASK(fb,cl);
	}

	if (jcndbg > 10)
		std::cout << "Expr: " << ret << std::endl;

	return ret;
}

// ---------------------------------------------------------------
// ---------------------------------------------------------------
// ---------------------------------------------------------------
void
Jcn::find_calc_text_changed()
{
	QString dispStr =_ui.find_jcnText_LE->toPlainText();

	if ( ! dispStr.contains('\n') )
		return;
	
	dispStr.remove('\n');
	_ui.jcnText_LE->setHtml(dispStr);
	find_calc_clicked();
}

// ---------------------------------------------------------------
// main entry into serach/compare
// ---------------------------------------------------------------
void 
Jcn::find_calc_clicked(bool clearit)
{
	if (jcndbg > 10)
		std::cout << "find_calc_clicked" << std::endl;

	_revMode = false;
	
	sql_find_calc_clicked(clearit);
}
// --------------------------------------------------------------------
// SQLDB
// --------------------------------------------------------------------
void 
Jcn::sql_find_calc_clicked(bool clearit)
{
	if (jcndbg > 10)
		std::cout << "sql_find_calc_clicked" << std::endl;
	
	QString dispStr =_ui.find_jcnText_LE->toPlainText();
	
	QByteArray workStr;
	workStr = _ui.find_jcnText_LE->toPlainText().toLatin1();
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

	
	if (workStr.length() == 0) return;

	// update string with potential tags
	_ui.find_jcnText_LE->setHtml(dispStr);

	if (jcndbg)
		std::cout << "Start Match: " << workStr << std::endl;



	JcnCalc workCalc(workStr);
	QSqlQuery countQuery("SELECT count(*) from jcn_table");
	int countRec;
	while (countQuery.next()) 
	{
         countRec = countQuery.value(0).toInt();
		 if (jcndbg)
			 std::cout << "countRec=" << countRec << std::endl;
    }	

	QProgressDialog progress("Loading ...", "Cancel", 0, countRec, this);
	

	// set results of what we wnat to search for	
	// first clear this view
	if (clearit)
	{
		_findModel1->clearAndReset();
		_findModel2->clearAndReset();
	}

	_findModel1->addEntry(workCalc);
	_ui.find_View1->setModel(_findModel1);
	
	QSqlQuery qry;
	QByteArray bq = build_sql_query(workCalc);
	if (bq == "")
		return;
		
	if ( ! qry.exec( bq ) )
	{
		std::cout << qry.lastError().databaseText() << std::endl;
		std::cout << "   " << workCalc.text() << std::endl;
		std::cout << "   " << bq << std::endl;

		QMessageBox msgBox;
		msgBox.setText("Error executing SQL: " + bq);
		msgBox.exec();

		return;
	}

	int f_matchMask=0, b_matchMask=0, fb_matchMask=0;
	CREATE_MATCH_ALGO_MASK(f, f_matchMask);
	CREATE_MATCH_ALGO_MASK(b, b_matchMask);
	CREATE_MATCH_ALGO_MASK(fb, fb_matchMask);

	
	int count=0;
	while ( qry.next() )
	{
	    progress.setValue(++count);

		JcnCalc c; // create new calc from query
		c.fromSqlQuery(qry);

		if (c.text().simplified() == workCalc.text().simplified() ) continue; // skip itself
		
		_findModel2->addEntry(c, workCalc, f_matchMask, b_matchMask, fb_matchMask);		
	}
	
	_ui.find_View2->setModel(_findModel2); // reload model again
	_findModel2->setHeaders();
}

// --------------------------------------------------------------------

#define JCN_SQL_EXPR(g, k) \
\
	if (_ui.g ## _ ## k ## _CB->isChecked()) \
	{ \
		ret += " ("; \
		\
		if (_ui.g ## _exact_CB->isChecked() ) \
		{\
			ret += xcat3(g,_,k); \
			ret += "="; \
			ret += QByteArray::number(c.g ## _ ## k()); \
		}\
		else \
		{\
			ret += xcat3(g,_,k); \
			ret += " LIKE \"%"; \
			ret += QByteArray::number(c.g ## _ ## k()); \
			ret += "%\""; \
		}\
		\
		\
		if (_ui.g ## _rev_CB->isChecked() ) \
		{\
			ret += " OR "; \
			int rev_res = rev(c.g ## _ ## k()); \
			if (_ui.g ## _exact_CB->isChecked() ) \
			{\
				ret += xcat3(g,_,k); \
				ret += "="; \
				ret += QByteArray::number(rev_res); \
			}\
			else \
			{\
				ret += xcat3(g,_,k); \
				ret += " LIKE \"%"; \
				ret += QByteArray::number(rev_res); \
				ret += "%\""; \
			}\
		}\
		\
		\
		if (_ui.g ## _ign0_CB->isChecked() ) \
		{\
			ret += " OR "; \
			int ign0_res = noz(c.g ## _ ## k()); \
			if (_ui.g ## _exact_CB->isChecked() ) \
			{\
				ret += xcat3(g,_,k); \
				ret += "="; \
				ret += QByteArray::number(ign0_res); \
			}\
			else \
			{\
				ret += xcat3(g,_,k); \
				ret += " LIKE \"%"; \
				ret += QByteArray::number(ign0_res); \
				ret += "%\""; \
			}\
		}\
		\
		\
		if (_ui.g ## _mul2_CB->isChecked() ) \
		{\
			ret += " OR "; \
			ret += xcat3(g,_,k); \
			ret += "="; \
			ret += QByteArray::number((int)(c.g ## _ ## k() * 2)); \
			\
			ret += " OR "; \
			ret += xcat3(g,_,k); \
			ret += "="; \
			ret += QByteArray::number((int)(c.g ## _ ## k() / 2)); \
		}\
		\
		\
		if (_ui.g ## _mul3_CB->isChecked() ) \
		{\
			ret += " OR "; \
			ret += xcat3(g,_,k); \
			ret += "="; \
			ret += QByteArray::number((int)(c.g ## _ ## k() * 3)); \
			\
			ret += " OR "; \
			ret += xcat3(g,_,k); \
			ret += "="; \
			ret += QByteArray::number((int)(c.g ## _ ## k() / 3)); \
		}\
		\
		\
		if (_ui.g ## _plus1_CB->isChecked() ) \
		{\
			ret += " OR "; \
			ret += xcat3(g,_,k); \
			ret += "="; \
			ret += QByteArray::number(c.g ## _ ## k() + 1); \
			\
			ret += " OR "; \
			ret += xcat3(g,_,k); \
			ret += "="; \
			ret += QByteArray::number(c.g ## _ ## k() - 1); \
		}\
		\
		ret += ") "; \
		\
		\
		if (_ui.g ## _all_RB->isChecked() ) \
			ret += " AND "; \
		else \
			ret += " OR "; \
	}

QByteArray
rmv_last_op(QByteArray & s)
{
	QByteArray ret="";
	QList<QByteArray> tokens = s.split(' ');
	for (int i=0; i < tokens.size() - 2; ++i)
	{
		QByteArray token = tokens[i].simplified();
		if (token.size() == 0) continue;
//		std::cout << "[" << token << "]" << std::endl;
		ret += token  + " ";
	}
	return ret;
}
// --------------------------------------------------------------------
QByteArray
Jcn::build_sql_query(JcnCalc & c)
{
	QByteArray  sqlStr = "SELECT * FROM jcn_table WHERE \n";

	QByteArray	ret = "";
	
	if ( _ui.f_GB->isChecked() )
	{
		ret += " ( ";
		JCN_SQL_EXPR(f,h);
		JCN_SQL_EXPR(f,v);
		JCN_SQL_EXPR(f,hv);
		JCN_SQL_EXPR(f,c);
		JCN_SQL_EXPR(f,p);
		JCN_SQL_EXPR(f,pc);
		JCN_SQL_EXPR(f,cl);
		ret = rmv_last_op(ret);
		ret += " ) \n ";
	}

	
	if ( _ui.b_GB->isChecked() )
	{
		if ( _ui.f_GB->isChecked() ) // add op if prior is checked
			ret += _ui.op1_TB->text();
			
		ret += " ( ";
		JCN_SQL_EXPR(b,h);
		JCN_SQL_EXPR(b,v);
		JCN_SQL_EXPR(b,hv);
		JCN_SQL_EXPR(b,c);
		JCN_SQL_EXPR(b,p);
		JCN_SQL_EXPR(b,pc);
		JCN_SQL_EXPR(b,cl);
		ret = rmv_last_op(ret);
		ret += " ) \n ";
	}


	if ( _ui.fb_GB->isChecked() )
	{
		if ( _ui.f_GB->isChecked() ||  _ui.b_GB->isChecked() ) // add op if prior is checked
			ret += _ui.op2_TB->text();

		ret += " ( ";
		JCN_SQL_EXPR(fb,h);
		JCN_SQL_EXPR(fb,v);
		JCN_SQL_EXPR(fb,hv);
		JCN_SQL_EXPR(fb,c);
		JCN_SQL_EXPR(fb,p);
		JCN_SQL_EXPR(fb,pc);
		JCN_SQL_EXPR(fb,cl);
		ret = rmv_last_op(ret);
		ret += " ) \n ";
	}

	if (ret=="")
		sqlStr = ret;
	else
		sqlStr += ret;
				
	if (jcndbg)
		std::cout << sqlStr << std::endl;

	return sqlStr;
}
// --------------------------------------------------------------------
void 
Jcn::find_options_clicked()
{
	if (jcndbg)
		std::cout << "find_options_clicked" << std::endl;

	if ( _ui.findChoice_Frame->isHidden() )
	{
		_ui.findChoice_Frame->show();
		_ui.findChoice_Frame->setVisible(true);
//		std::cout << "show" << std::endl;
	}
	else
	{
		_ui.findChoice_Frame->hide();
		_ui.findChoice_Frame->setVisible(false);
//		std::cout << "hide" << std::endl;
	}
}
// --------------------------------------------------------------------
void 
Jcn::find_find_clicked()
{
QString logStr;

	logStr = "start - find_find_clicked()";
	dbglog(logStr, 10);


	if (jcndbg)
		std::cout << "find_find_clicked" << std::endl;

	JcnModel * m =	(JcnModel*)_ui.find_View2->model();

	QList<QModelIndex> list = _ui.find_View2->selectionModel()->selectedRows(0);
	if (list.size() == 0) return; // nothing selected
	
	// only chose one
	QVariant var = m->data(m->index(list.at(0).row(), 0, QModelIndex()));
	QByteArray workStr = var.toString().toLatin1().simplified();

	// set search string
	_ui.find_jcnText_LE->setText(QString(workStr));
	
	// click button to execute it
	_ui.find_calc_PB->click();

	logStr = "finish - find_find_clicked()";
	dbglog(logStr, 10);
}
// --------------------------------------------------------------------
void 
Jcn::find_full_clicked()
{
QString logStr;

	logStr = "start - find_full_clicked()";
	dbglog(logStr, 10);

	QList<QTreeWidgetItem *> wi = _ui.fullMatch_TW->selectedItems ();

	if (wi.size() == 0) return;

	_ui.main_TW->setCurrentIndex(2);
	
	QByteArray workStr = wi.at(0)->text(0).toLatin1().simplified();

	if (jcndbg)
		std::cout << "find_full_clicked: "  << workStr << std::endl;

	// set search string
	_ui.find_jcnText_LE->setText(QString(workStr));
	
	// click button to execute it
	_ui.find_calc_PB->click();

	logStr = "finish - find_full_clicked()";
	dbglog(logStr, 10);
}
// ---------------------------------------------------------------
void 
Jcn::find_op1_clicked()
{
QString logStr;

	logStr = "start - find_op1_clicked()";
	dbglog(logStr, 10);

	static int count=0;

	count++;
	
	if (count==1)
		_ui.op1_TB->setText("AND");
	else
	if (count==2)
	{
		count=0;
		_ui.op1_TB->setText("OR");
	}

	logStr = "finish - find_op1_clicked()";
	dbglog(logStr, 10);

}
// ---------------------------------------------------------------
void 
Jcn::find_op2_clicked()
{
QString logStr;

	logStr = "start - find_op2_clicked()";
	dbglog(logStr, 10);

	static int count=0;

	count++;
	
	if (count==1)
		_ui.op2_TB->setText("AND");
	else
	if (count==2)
	{
		count=0;
		_ui.op2_TB->setText("OR");
	}

	logStr = "finish - find_op2_clicked()";
	dbglog(logStr, 10);

}
// ------------------------------------------------------------------
void
Jcn::find_f_GB_clicked(bool f)
{
QString logStr;

	logStr = "start - find_f_GB_clicked()";
	dbglog(logStr, 10);

	_ui.f_h_CB->setChecked(f);
	_ui.f_v_CB->setChecked(f);
	_ui.f_hv_CB->setChecked(f);
	_ui.f_all_RB->setChecked(f);
		
	if (!f)
	{
		_ui.f_c_CB->setChecked(f);
		_ui.f_p_CB->setChecked(f);
		_ui.f_pc_CB->setChecked(f);
		_ui.f_cl_CB->setChecked(f);
		_ui.f_all_RB->setChecked(f);
	}

	logStr = "finish - find_f_GB_clicked()";
	dbglog(logStr, 10);

}
// ------------------------------------------------------------------
void
Jcn::find_b_GB_clicked(bool f)
{
QString logStr;

	logStr = "start - find_b_GB_clicked()";
	dbglog(logStr, 10);

	_ui.b_h_CB->setChecked(f);
	_ui.b_v_CB->setChecked(f);
	_ui.b_hv_CB->setChecked(f);
	_ui.b_all_RB->setChecked(f);
		
	if (!f)
	{
		_ui.b_c_CB->setChecked(f);
		_ui.b_p_CB->setChecked(f);
		_ui.b_pc_CB->setChecked(f);
		_ui.b_cl_CB->setChecked(f);
		_ui.b_all_RB->setChecked(f);
	}

	logStr = "finish - find_b_GB_clicked()";
	dbglog(logStr, 10);
}
// ------------------------------------------------------------------
void
Jcn::find_fb_GB_clicked(bool f)
{
QString logStr;

	logStr = "start - find_fb_GB_clicked()";
	dbglog(logStr, 10);

	_ui.fb_h_CB->setChecked(f);
	_ui.fb_v_CB->setChecked(f);
	_ui.fb_hv_CB->setChecked(f);
	_ui.fb_all_RB->setChecked(f);
		
	if (!f)
	{
		_ui.fb_c_CB->setChecked(f);
		_ui.fb_p_CB->setChecked(f);
		_ui.fb_pc_CB->setChecked(f);
		_ui.fb_cl_CB->setChecked(f);
		_ui.fb_all_RB->setChecked(f);
	}

	logStr = "finish - find_fb_GB_clicked()";
	dbglog(logStr, 10);

}
// ------------------------------------------------------------------
void 
Jcn::print_find_clicked()
{
	QPrinter printer(QPrinter::HighResolution);
	printer.setOrientation ( QPrinter::Landscape );
//	printer.setPageMargins ( 0.05, 0.1, 0.05, 0.1, QPrinter::Inch );
	TDP * tdp = new TDP(_qapp, _ui.find_View2, &printer, this);

	QString dispStr = _ui.find_jcnText_LE->toPlainText();
		
	QByteArray workStr;
	workStr = _ui.find_jcnText_LE->toPlainText().toLatin1();
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

	if (workStr.length()==0) return;

	// update string with potential tags
	_ui.find_jcnText_LE->setHtml(dispStr);

	JcnCalc calc(workStr);
	tdp->setHeaderText(calc.toText(_revMode));
	tdp->setGridMode(TDP::NormalGrid);
	tdp->exec();
	
	if (tdp->result() == QDialog::Accepted)
		tdp->print();
		
	delete tdp;
}
// ------------------------------------------------------------------
void 
Jcn::rev_find_clicked()
{
	if (! _ui.revFind_PB->isChecked() )
	{
		find_calc_clicked();
		return;
	}
	
	_revMode = true;
	
	_findModel1_rev->clearAndReset();	
	_findModel2_rev->clearAndReset();

	_ui.find_View1->setModel(NULL);
	_ui.find_View2->setModel(NULL);

	for (int r = 0; r < _findModel1->rowCount(); ++r)
	{
		for (int c = 0; c < _findModel1->columnCount(); ++c)
		{
			QStandardItem * oneItem = _findModel1->item(r,c);
			QStandardItem * rItem;
			if (c==0)
				rItem = new QStandardItem( oneItem->text() ); // do not reverse text
			else
				rItem = new QStandardItem( rev_qstr(oneItem->text()) );

			rItem->setFont(oneItem->font());
			rItem->setForeground(oneItem->foreground());
			rItem->setBackground(QBrush(QColor("lightGray")));
			_findModel1_rev->setItem(r, c, rItem);
		}
	}
		
	for (int r = 0; r < _findModel2->rowCount(); ++r)
	{
		for (int c = 0; c < _findModel2->columnCount(); ++c)
		{
			QStandardItem * oneItem = _findModel2->item(r,c);
			QStandardItem * rItem;
			if (c==0)
				rItem = new QStandardItem( oneItem->text() ); // do not reverse text
			else
				rItem = new QStandardItem( rev_qstr(oneItem->text()) );

			rItem->setFont(oneItem->font());
			rItem->setForeground(oneItem->foreground());
			rItem->setBackground(QBrush(QColor("lightGray")));
			_findModel2_rev->setItem(r, c, rItem);
		}
	}


	for (int i=0; i< JcnDD_Last; ++i)
	{
		_ui.find_View2->setColumnWidth(i, JcnDD[i].w);
	}

	_ui.find_View1->setModel(_findModel1_rev);
	_findModel1_rev->setHeaders();

	_ui.find_View2->setModel(_findModel2_rev);
	_findModel2_rev->setHeaders();

}

// ------------------------------------------------------------------
void 
Jcn::roman_find_clicked()
{
//	_revMode = true;

	if (! _ui.romanFind_PB->isChecked() )
	{
		find_calc_clicked();
		return;
	}
	
	_findModel1_rev->clearAndReset();	
	_findModel2_rev->clearAndReset();

	_ui.find_View1->setModel(NULL);
	_ui.find_View2->setModel(NULL);

	for (int r = 0; r < _findModel1->rowCount(); ++r)
	{
		for (int c = 0; c < _findModel1->columnCount(); ++c)
		{
			QStandardItem * oneItem = _findModel1->item(r,c);
			QStandardItem * rItem;
			if (c==0)
				rItem = new QStandardItem( oneItem->text() ); // do not reverse text
			else
				rItem = new QStandardItem( n2t::d2roman(oneItem->text()) );

			rItem->setFont(oneItem->font());
			rItem->setForeground(oneItem->foreground());
			rItem->setBackground(QBrush(QColor("cyan")));
			_findModel1_rev->setItem(r, c, rItem);
		}
	}
		
	for (int r = 0; r < _findModel2->rowCount(); ++r)
	{
		for (int c = 0; c < _findModel2->columnCount(); ++c)
		{
			QStandardItem * oneItem = _findModel2->item(r,c);
			QStandardItem * rItem;
			if (c==0)
				rItem = new QStandardItem( oneItem->text() ); // do not reverse text
			else
				rItem = new QStandardItem( n2t::d2roman(oneItem->text()) );

			rItem->setFont(oneItem->font());
			rItem->setForeground(oneItem->foreground());
			rItem->setBackground(QBrush(QColor("cyan")));
			_findModel2_rev->setItem(r, c, rItem);
		}
	}


	for (int i=0; i< JcnDD_Last; ++i)
	{
		_ui.find_View2->setColumnWidth(i, JcnDD[i].w);
	}

	_ui.find_View1->setModel(_findModel1_rev);
	_findModel1_rev->setHeaders();

	_ui.find_View2->setModel(_findModel2_rev);
	_findModel2_rev->setHeaders();

}
// --------------------------------------------------------------------
