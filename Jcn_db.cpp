#include <unistd.h>
#include <iostream>
#include <QtCore/QSysInfo>
#include <QtGui/QtGui>
#include <QtGui/QMessageBox>


#include "tdp.h"

#include "DD.h"
#include "Jcn.h"
#include "JcnCalc.h"
#include "JcnModel.h"
#include "JcnSqlModel.h"

#include "jcn_data.h"
#include "Express.h"

#include "guide.h"
#include "jcn.rcc"

#include "textedit.h"

#include "FileChkSum.h"

#include "n2t.h"

#include <set>


#define TL(text) { log_file.write(QString(text).toAscii() + "\n"); log_file.flush(); }

// --------------------------------------------------------------------
Jcn::Jcn(QApplication * a, QMainWindow * parent):
    QMainWindow(parent),
    _qapp(a),
	_findModel1_rev(NULL),
	_findModel2_rev(NULL),
	_revMode(false)
{
	check_dir();

	dbglog("setup ui");
		
    _ui.setupUi(this);

	dbglog("create menus");
	
	// menu file
    connect(_ui.actionCreateDb, SIGNAL(triggered()), this, SLOT(createDb()));
    connect(_ui.actionOpenDb, SIGNAL(triggered()), this, SLOT(openDb()));
    connect(_ui.actionImport, SIGNAL(triggered()), this, SLOT(importDb()));
	connect(_ui.actionExport, SIGNAL(triggered()), this, SLOT(exportDb()));
	connect(_ui.actionEdit, SIGNAL(triggered()), this, SLOT(editDb()));
    connect(_ui.actionExit, SIGNAL(triggered()), this, SLOT(exitApp()));

	// menu misc
    connect(_ui.actionGenerate, SIGNAL(triggered()), this, SLOT(generate()));
    connect(_ui.actionFull_Matches, SIGNAL(triggered()), this, SLOT(fullMatches()));
	
	// menu help
    connect(_ui.actionAbout, SIGNAL(triggered()), this, SLOT(aboutApp()));
    connect(_ui.actionNotes, SIGNAL(triggered()), this, SLOT(notesApp()));
   	   
	// for clean exit
    connect(_qapp, SIGNAL(lastWindowClosed()), this, SLOT(exitApp()));

	// main tab
    connect( _ui.main_TW, SIGNAL(selected(QString)), this, SLOT(tab_selected(QString)) );
	

	dbglog("create tabs");
	//====================================================================
	// translate tab
	//====================================================================
	_ui.jcnText_LE->verticalScrollBar()->hide();
	_ui.jcnText_LE->horizontalScrollBar()->hide();
    connect( _ui.calc_PB, SIGNAL(clicked()), this, SLOT(translate_clicked()) );
	//connect( _ui.jcnText_LE, SIGNAL(returnPressed()), this,  SLOT(translate_clicked()));
	connect( _ui.jcnText_LE, SIGNAL(textChanged()), this,  SLOT(translate_text_changed()));	
    connect( _ui.addToDb_PB, SIGNAL(clicked()), this, SLOT(add_to_db_clicked()) );
    connect( _ui.find_PB, SIGNAL(clicked()), this, SLOT(find_clicked()) );
    connect( _ui.printResult_PB, SIGNAL(clicked()), this, SLOT(print_result_clicked()) );

   	connect( _ui.showFwa_TB, SIGNAL(clicked()), this, SLOT(translate_clicked()) );
   	connect( _ui.showBwa_TB, SIGNAL(clicked()), this, SLOT(translate_clicked()) );
   	connect( _ui.useRoman_TB, SIGNAL(clicked()), this, SLOT(translate_clicked()) );
		   

	//====================================================================	
	// databse tab
	//====================================================================
	connect( _ui.addNewToDb_PB, SIGNAL(clicked()), this, SLOT(add_new_to_db_clicked()) );    
    connect( _ui.rmvFromDb_PB, SIGNAL(clicked()), this, SLOT(rmv_from_db_clicked()) );
    connect( _ui.findDb_PB, SIGNAL(clicked()), this, SLOT(find_db_clicked()) );
    connect( _ui.printDb_PB, SIGNAL(clicked()), this, SLOT(print_db_clicked()) );
	connect( _ui.searchDb_LE, SIGNAL(textEdited(const QString&)), this,  SLOT(search_textChanged(const QString&)));

	connect( _ui.dbFilters_PB, SIGNAL(clicked()), this, SLOT(db_filters_clicked()) );


	// db filters
	connect( _ui.textFilterDb_LE, SIGNAL(textEdited(const QString&)), this,  SLOT(textFilter_textChanged(const QString&)));
	connect( _ui.textFilterDb_LE, SIGNAL(returnPressed()), this,  SLOT(applyFilterDb_clicked()));
	
	connect( _ui.numFilterDb_LE, SIGNAL(textEdited(const QString&)), this,  SLOT(numFilter_textChanged(const QString&)));
	connect( _ui.numFilterDb_LE, SIGNAL(returnPressed()), this,  SLOT(applyFilterDb_clicked()));
	
	connect( _ui.sqlText_LE, SIGNAL(textEdited(const QString&)), this,  SLOT(expFilter_textChanged(const QString&)));
	connect( _ui.sqlText_LE, SIGNAL(returnPressed()), this,  SLOT(applyFilterDb_clicked()));
	
	connect( _ui.applyFilterDb_PB, SIGNAL(clicked()), this, SLOT(applyFilterDb_clicked()) );
	connect( _ui.resetFilterDb_PB, SIGNAL(clicked()), this, SLOT(resetFilterDb_clicked()) );


	QString ttmsg = "Enter a valid SQL command in a folowing form: <br>";
	ttmsg += "  <u><b>select * from jcn_table where tag=value and tag=value or tag=value</b></u>    etc...<br><br>";
	ttmsg += "Valid tags are:<br><b> ";

	for (int c=0; c < JcnDD_Last; ++c)
	{
		ttmsg += JcnDD[c].sql_tag;
		ttmsg += ", ";
		if (c%7==0) ttmsg += "<br>";
	}
	ttmsg += "</b><br>";

	ttmsg += "SQL Examples: <br>";
	ttmsg += "  select * from jcn_table where f_h=324<br>";		
	ttmsg += "  select * from jcn_table b_hv=88 and fb_cl=1008<br>";
	ttmsg += "  select * from jcn_table jcntext like '%dec%'<br>";
	ttmsg += "  select * from jcn_table jcntext='dec'<br>";
		
	_ui.sqlText_LE->setToolTip(ttmsg);

	// sqlite db
	_db = QSqlDatabase::addDatabase("QSQLITE");


	//====================================================================
	// find compare tab
	//====================================================================
	_ui.find_jcnText_LE->verticalScrollBar()->hide();
	_ui.find_jcnText_LE->horizontalScrollBar()->hide();
	connect( _ui.find_calc_PB, SIGNAL(clicked()), this, SLOT(find_calc_clicked()) );
//	connect( _ui.find_jcnText_LE, SIGNAL(returnPressed()), this,  SLOT(find_calc_clicked()));
	connect( _ui.find_jcnText_LE, SIGNAL(textChanged()), this,  SLOT(find_calc_text_changed()));
	connect( _ui.findOptions_PB, SIGNAL(clicked()), this, SLOT(find_options_clicked()) );
    connect( _ui.printFind_PB, SIGNAL(clicked()), this, SLOT(print_find_clicked()) );
    connect( _ui.revFind_PB, SIGNAL(clicked()), this, SLOT(rev_find_clicked()) );
    connect( _ui.romanFind_PB, SIGNAL(clicked()), this, SLOT(roman_find_clicked()) );
		
	connect (_ui.f_GB, SIGNAL(clicked(bool)), this, SLOT(find_f_GB_clicked(bool) ));
	connect (_ui.b_GB, SIGNAL(clicked(bool)), this, SLOT(find_b_GB_clicked(bool) ));
	connect (_ui.fb_GB, SIGNAL(clicked(bool)), this, SLOT(find_fb_GB_clicked(bool) ));

    connect( _ui.op1_TB, SIGNAL(clicked()), this, SLOT(find_op1_clicked()) );
    connect( _ui.op2_TB, SIGNAL(clicked()), this, SLOT(find_op2_clicked()) );


				
	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	//_ui.tabWidget->setTabText(3,""); // flex
	//_ui.tabWidget->setTabEnabled(3,false); // flex
	_ui.main_TW->setCurrentIndex(0); // translate

	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//	_ui.f_GB->hide();	// 
//	_ui.b_GB->hide();	// 
//	_ui.fb_GB->hide();	// 
			
	_ui.findChoice_Frame->hide();	// make search choice invisible
	_ui.filter_Frame->hide();		// make filter options invisible
		
//	_ui.addNewToDb_PB->setDisabled(true);
//	_ui.rmvFromDb_PB->setDisabled(true);
	
	dbglog("loads");
		
	load_jcn_strings();	

	create_jcn_db_view();
	create_find_view();
	
	load_jcn_db();

	createCtxMenu();
}

// -------------------------------------------------------------
void
Jcn::dbglog(const QString & s, int lev)
{
	if (lev <= jcndbg)
		std::cout << s << std::endl;
}

// -------------------------------------------------------------
// data files will always be in this dir
// -------------------------------------------------------------
void
Jcn::check_dir()
{
	dbglog("start - check_dir");
	
#ifdef Q_WS_WIN  // if we run under windows
	_jcnRootDir = jcn_dir_win_def;
#else // unix
	_jcnRootDir = jcn_dir_unix_def;
#endif	

	dbglog(" checking path: " + _jcnRootDir);

	QDir d(_jcnRootDir);

	// if dir does not exist then make a new one
	// this must be a first run - create sample database
	if (! d.exists() )
	{
		dbglog(" creating path: " + _jcnRootDir);
		if ( ! d.mkpath(_jcnRootDir) ) // create it
		{
			dbglog("  can't create path: " + _jcnRootDir);

	   		QMessageBox msgBox;
 			msgBox.setText("Can't create path: " + _jcnRootDir);
			TL("Can't create path: " + _jcnRootDir);
			msgBox.exec();
			exit(-1);
		}
	}

	// create log file
	dbglog(" creating log file: " + _jcnRootDir  + "/" + jcn_log_fn);
	log_file.setFileName(_jcnRootDir  + "/" + jcn_log_fn);
	if (! log_file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate) )
	{
		dbglog("  can't create log file: " + _jcnRootDir  + "/" + jcn_log_fn);

		QMessageBox msgBox;
		msgBox.setText("Can't create log file: " + _jcnRootDir  + "/" + jcn_log_fn);
		msgBox.exec();
		exit(-1);
	}
	
	dbglog(" checking file: " + _jcnRootDir  + "/" + jcn_str_fn_def);
	// check jcn_strings.txt
	QFile q_file(_jcnRootDir  + "/" + jcn_str_fn_def);
	if (! q_file.exists() )
	{
		dbglog(" creating file: " + _jcnRootDir  + "/" + jcn_str_fn_def);
		// create default jcn_strings.txt
   		if(! q_file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
   		{
			dbglog("  can't create file: " + _jcnRootDir  + "/" + jcn_str_fn_def);

   			QMessageBox msgBox;
			msgBox.setText("Can't create file: " + _jcnRootDir  + "/" + jcn_str_fn_def);
			TL("Can't create file: " + _jcnRootDir  + "/" + jcn_str_fn_def);
			msgBox.exec();
			exit(-1);
		}

		QMessageBox msgBox;
		msgBox.setText("Creating default JCN questions file: " + _jcnRootDir  + "/" + jcn_str_fn_def);
		TL("Creating default JCN questions file: " + _jcnRootDir  + "/" + jcn_str_fn_def);
		msgBox.exec();
		
		int count=0;
		while (jcn_strings_data[count])
		{
			QByteArray test = jcn_strings_data[count];
			if (test == ".") // reached end
				break;
			
			q_file.write(QByteArray(jcn_strings_data[count]) + "\r\n"); // use CR and NL
//			TL("Add def question: " + QByteArray(jcn_strings_data[count]));
			count++;
		}
		q_file.flush();
		q_file.close();
	}

	dbglog(" checking file: " + _jcnRootDir  + "/" + jcn_db_fn_def);
	// check jcn_db.txt
	QFile s_file(_jcnRootDir  + "/" + jcn_db_fn_def);
	if (! s_file.exists() )
	{
		dbglog(" creating file: " + _jcnRootDir  + "/" + jcn_db_fn_def);
		QFile s_file(_jcnRootDir  + "/" + jcn_db_fn_def);
   		if(! s_file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
   		{
			dbglog("  can't create file: " + _jcnRootDir  + "/" + jcn_db_fn_def);

   			QMessageBox msgBox;
			msgBox.setText("Can't create file: " + _jcnRootDir  + "/" + jcn_db_fn_def);
			TL("Can't create file: " + _jcnRootDir  + "/" + jcn_db_fn_def);			
			msgBox.exec();
			exit(-1);
		}

		QMessageBox msgBox;
		msgBox.setText("Creating default JCN DB file: " + _jcnRootDir  + "/" + jcn_db_fn_def);
		TL("Creating default JCN DB file: " + _jcnRootDir  + "/" + jcn_db_fn_def);
		msgBox.exec();
		
		int count=0;
		while (jcn_db_data[count])
		{
			QByteArray test = jcn_db_data[count];
			if (test == ".") // reached end
				break;
		
			s_file.write(QByteArray(jcn_db_data[count]) + "\r\n"); // use CR and NL
//			TL("Add def data: " + QByteArray(jcn_db_data[count]));
			count++;
		}
		s_file.flush();
		s_file.close();
	}

	dbglog("finish - check_dir");
}
// -------------------------------------------------------------
void
Jcn::check_sql_db(const QByteArray & fname)
{
	QString logStr;
	
	dbglog("start - check_sql_db(fname): " + fname);
	
	TL("Check SQLDB: ");
	dbglog("Check SQLDB: ");
		
	//	let's open this database
	QSqlQuery qry;

	QByteArray dbFileName = fname + ".sql";
	dbglog("Open SQLDB: " + dbFileName);
	TL("Open SQLDB: " + dbFileName);

   	_db.setDatabaseName(dbFileName);
    if ( ! _db.open() )
   	{
		dbglog("Can't ppen SQLDB: " + dbFileName);
		TL("Can not open SQLDB: " + dbFileName);

		QMessageBox msgBox;
		msgBox.setText("Can not open SQLDB: " + dbFileName + " Exit.");
		msgBox.exec();
		exit(-1);

		return;
	}


	// load in database from text file
	// in case where we use disk database it will need to be done only in case or record number mismatch
	
	dbglog("Testing SQLDB and TextDB");
		
	TL("Testing SQLDB and TextDB.");

	QString logText;
	QByteArray textDbFn = fname;
   	QFile file(textDbFn);
   	if(! file.open(QIODevice::ReadOnly | QIODevice::Text))
   	{
		dbglog("Can't open file: " + textDbFn);
		QMessageBox msgBox;
		TL("Can't open file: " + textDbFn);
		msgBox.setText("Can not open file: " + textDbFn);
		msgBox.exec();
		exit(-1);
	}

	// std::cout << "LOAD MAP" << std::endl;
	// load map to check for number of unique entries
	// this is needed in case we want to rebuild SQL database
	std::map<QByteArray, QByteArray, cmp_qbar> dbmap;
	QByteArray line;
	int count_lines=0;
	while( !file.atEnd() )
	{
		line = file.readLine().simplified().toUpper();
		if (line[0] == '#' || line.length() == 0) continue;

		// check line for invalid chars
		if (! isLineValid(line) )
		{
			continue;
		}

		line = line.simplified(); // make sure again

		dbmap[line] = line;
		count_lines++;
	}
	file.reset();  // for another run

	logStr = "Get record count from  disk SQLDB";
	dbglog(logStr);
	TL(logStr);

	// let see if numbers of records match
	qry  = _db.exec("SELECT count(*) from jcn_table");
	if ( qry.lastError().isValid() )
	{
		logStr = "QRY Error: " + qry.lastError().databaseText();
		dbglog(logStr);
		TL(logStr);

		// lets try to recreate db
		goto DB_CREATE;
	}

	int countRec;
	while (qry.next()) 
	{
        countRec = qry.value(0).toInt();
    }

	logStr = "SQLDB countRec=" + QString::number(countRec) + " dbmap size=" + QString::number(dbmap.size());
	dbglog(logStr);
	TL(logStr);

	if ( countRec != dbmap.size() )
	{
		goto DB_CREATE;
	}
	else
	{
		goto DB_GOOD;
	}

	DB_CREATE:
	{		
		logStr = "Creating new SQLDB: ";
		dbglog(logStr);		
		TL(logStr);
		
		// drop table just in case - if does not exist its ok
		qry  = _db.exec("drop table jcn_table"); // just in case

		logStr = "Creating schema: " + JcnCalc::toSqlCreate();
		dbglog(logStr);		
		TL(logStr);

		// re create schema
		qry  = _db.exec(JcnCalc::toSqlCreate());
		if ( qry.lastError().isValid() )
		{
			std::cout << qry.lastError().databaseText() << std::endl;
			std::cout << "Failed to create schema: " << JcnCalc::toSqlCreate() << std::endl;
			TL("Failed to create schema: " + qry.lastError().databaseText().toAscii() );

			QMessageBox msgBox;
			msgBox.setText("Can not create schema: " + qry.lastError().databaseText() + " Exit.");
			msgBox.exec();
			exit(-1);
		}
	}

	DB_RELOAD:
	{		
		bool dataValid = true;
		
		QProgressDialog progress("Recreating SQLDB ...", "", 0, count_lines, this);

		int count = 0;

		if ( ! _db.transaction() )
		{
			std::cout << "Failed to start txn: " << std::endl;
			TL( "Failed to start txn: " );

			QMessageBox msgBox;
			msgBox.setText("Failed to start txn: Exit.");
			msgBox.exec();
			exit(-1);

			return;
		}

		std::cout << "LOAD FROM DISK" << std::endl;

		while( !file.atEnd() )
		{
    		// strip leading and trailing spaces
			line = file.readLine().simplified().toUpper();
			if (line[0] == '#' || line.length() == 0) continue;

			// check line for invalid chars
			if (! isLineValid(line) )
			{
				dataValid = false;
				continue;
			}

			line = line.simplified(); // make sure again

			count++;
			JcnCalc c(line);

			qry  = _db.exec(c.toSqlInsert());
			if ( qry.lastError().isValid() )
			{
	// mostly dupes
	//			std::cout << "QRY Error: " << qry.lastError().databaseText() << std::endl;
	//			std::cout << "   " << count << "  line=" << line << std::endl;

				QByteArray logText = "QRY Error: ";
				logText += " dbt=" + qry.lastError().databaseText().toAscii();
				logText += " drt=" + qry.lastError().driverText().toAscii();
				logText += " n=" + QByteArray::number(qry.lastError().number());
				logText += " [" + c.text() + "]";
				TL (logText);
			}

    		progress.setValue(count);
        	_qapp->processEvents();
		}

		if ( ! _db.commit() )
		{
			std::cout << "Failed to commit txn:" << std::endl;
			TL( "Failed to commit txn:" );

			QMessageBox msgBox;
			msgBox.setText("Failed to commit txn: Exit.");
			msgBox.exec();
			exit(-1);

			return;
		}
		
		if (!dataValid)
		{
			QMessageBox msgBox;
			msgBox.setText("Some data contains inalid symbols and ommited:  Check  c:\\qjcn\\jcn_log.txt for errors. ");
			msgBox.exec();
//			exit(-1);
		}
	}

	DB_GOOD:
	{
		dbglog("SQLDB OK");
		TL( "SQLDB OK" );
	}

	dbglog("finish - check_sql_db(fname): " + fname);
}
// -------------------------------------------------------------
bool
Jcn::isLineValid(QByteArray & line)
{	
	line.replace('"',' ');  // make sure we do not have double quotes embedded
	line.replace('\'',' ');  // make sure we do not have single quotes embedded

	bool ret = true;
	for (int i=0; i < line.length(); ++i)
	{
		char ch = line[i];

		if (
			(ch >= '0' && ch <= '9') ||    	// numbers are OK
			(ch >= 'A' && ch <= 'Z') ||		// lower case ketters are OK
			(ch >= 'a' && ch <= 'z') ||		// upper case ketters are OK
			(ch == ' ')	 ||					// space is OK
			(ch == '.')	 ||					// dot is OK
			(ch == ',')	 ||					// comma is OK
			(ch == '-')	 ||					// dash is OK
			(ch == ';')	 ||					// semi is OK
			(ch == ':')	 ||					// col is OK
			(ch == '?')	 ||					// ques is OK
			(ch == ')')	 ||					// par is OK
			(ch == '(')	 ||					// par is OK
			(ch == '\\')  ||					// sla is OK
			(ch == '/')						// backsla is OK
			)
		{
			
		}
		else // no good
		{
			//std::cout << "INVALID LINE: " << line << std::endl;
			TL("INVALID CHAR IN LINE: " + line);
			ret = false;
			break;
		}
	}
	
	return ret;
}
// -------------------------------------------------------------
void 
Jcn::createCtxMenu()
{
	QAction * a1 = new QAction("Find/Compare", this);
	_dbCtxMenu.addAction(a1);
	_ui.db_View->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(this, SIGNAL(sig_dbFindCmp(void)), this, SLOT(find_db_clicked()));

	connect( _ui.db_View,
			SIGNAL(customContextMenuRequested (const QPoint &)), 
			this, 
			SLOT(dbCtxMenuEvent(const QPoint &))
			);


	QAction * a2 = new QAction("Find/Compare", this);
	_findCtxMenu.addAction(a2);
	_ui.find_View2->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(this, SIGNAL(sig_findFindCmp(void)), this, SLOT(find_find_clicked()));
	
	connect( _ui.find_View2,
			SIGNAL(customContextMenuRequested (const QPoint &)), 
			this, 
			SLOT(findCtxMenuEvent(const QPoint &))
			);

	QAction * a3 = new QAction("Find/Compare", this);
	_fullMatchCtxMenu.addAction(a3);
	_ui.fullMatch_TW->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(this, SIGNAL(sig_findFindCmp(void)), this, SLOT(find_full_clicked()));
	
	connect( _ui.fullMatch_TW,
			SIGNAL(customContextMenuRequested (const QPoint &)), 
			this, 
			SLOT(findCtxMenuEvent(const QPoint &))
			);
}
// -------------------------------------------------------------
void 
Jcn::dbCtxMenuEvent(const QPoint & pos)
{
	_dbCtxMenu.exec(QCursor::pos());
	emit sig_dbFindCmp();
}
// -------------------------------------------------------------
void 
Jcn::findCtxMenuEvent(const QPoint & pos)
{
	_findCtxMenu.exec(QCursor::pos());
	emit sig_findFindCmp();
}
// --------------------------------------------------------------------
void
Jcn::load_jcn_strings(const QByteArray & fname)
{
	dbglog("start - load_jcn_strings()");
	
	_jcnStrFn = _jcnRootDir + "/" + fname;
	QString logText;

	dbglog("Check file: " + _jcnStrFn);
   	QFile file(_jcnStrFn);
   	if(! file.open(QIODevice::ReadOnly | QIODevice::Text))
   	{

//		logText = "can't open file " + _jcnStrFn;
//		std::cout << logText << std::endl;
//		return;
	}

	// add dummy blank at the beginning
	_ui.jcnStr_LIST->addItem("");

	QByteArray line;
	int count_lines=0;
	while( !file.atEnd() )
	{
		line = file.readLine().simplified();
		if (line[0] == '#' || line.length() == 0)	continue;
		
		// check line for invalid chars
		if (! isLineValid(line) )
		{
			continue;
		}

		count_lines++;
	}

	file.reset();	
	QProgressDialog progress("Loading JCN Strings...", "Ok", 0, count_lines, this);
	int count = 0;
	QList<QListWidgetItem *> fi_list;
	while( !file.atEnd() )
	{
   		// strip leading and trailing spaces
		line = file.readLine().simplified();
		if (line[0] == '#' || line.length() == 0)	continue;

		// check line for invalid chars
		if (! isLineValid(line) )
		{
			continue;
		}

		// make sure we do not already have it
		fi_list = _ui.jcnStr_LIST->findItems(line.toUpper(), Qt::MatchExactly);
		if (fi_list.size()==0)
			_ui.jcnStr_LIST->addItem(line.toUpper());

    	progress.setValue(count++);			
        _qapp->processEvents();
	}
	
	// force it to blank
	_ui.jcnStr_LIST->setCurrentRow(0);	

	dbglog("finish - load_jcn_strings()");
}
// -------------------------------------------------------------
void
Jcn::create_jcn_db_view()
{
	dbglog("start - create_jcn_db_view()");
		
	for (int i=0; i< JcnDD_Last; ++i)
	{
		_ui.db_View->setColumnWidth(i, JcnDD[i].w);
	}

//	_ui.db_View->setWordWrap(true);

//@@@
//	_tmpDbModel = new JcnModel(_ui.db_View, this);
//	_dbModel = new JcnModel(_ui.db_View, this);

	_tmpSqlDbModel = new JcnSqlModel(_ui.db_View, this);
	_sqlDbModel = new JcnSqlModel(_ui.db_View, this);

	_ui.db_View->setModel(_sqlDbModel);
	_ui.db_View->setSortingEnabled(true);

//@@@
//	_dbModel->setSortRole(Qt::DisplayRole);

	dbglog("finish - create_jcn_db_view()");
}
// -------------------------------------------------------------
void
Jcn::create_find_view()
{
	dbglog("start - create_find_view()");
	
	_findModel1 = new JcnModel(_ui.find_View1, this);
	_ui.find_View1->setModel(_findModel1);
	_ui.find_View1->setSortingEnabled(false);


	for (int i=0; i< JcnDD_Last; ++i)
	{
		_ui.find_View1->setColumnWidth(i, JcnDD[i].w);
	}

	_findModel1_rev = new JcnModel(_ui.find_View1, this);

		
	_findModel2 = new JcnModel(_ui.find_View2, this);
	_ui.find_View2->setModel(_findModel2);

	_ui.find_View2->setSortingEnabled(true);
	_findModel2->setSortRole(Qt::DisplayRole);

	for (int i=0; i< JcnDD_Last; ++i)
	{
		_ui.find_View2->setColumnWidth(i, JcnDD[i].w);
	}

	_findModel2_rev = new JcnModel(_ui.find_View2, this);

	dbglog("finish - create_find_view()");
}
// --------------------------------------------------------------------
//   load dadabase
// --------------------------------------------------------------------
void
Jcn::load_jcn_db(const QByteArray & fname, bool force)
{
	dbglog("start - load_jcn_db(fname): " + fname);
	
	QString logText;

	if (force)
		_jcnDbFn = fname;
	else
		_jcnDbFn = _jcnRootDir + "/" + fname;


   	QFile file(_jcnDbFn);
   	if(! file.open(QIODevice::ReadOnly | QIODevice::Text))
   	{
		QMessageBox msgBox;
		msgBox.setText("Can't open file: " + _jcnDbFn);
		msgBox.exec();
		exit(-1);
	}


	file.reset();
	QByteArray line;
	int count_lines=0;
	while( !file.atEnd() )
	{
		line = file.readLine().simplified();
		if (line=="#" || line.size()==0) continue;

		// check line for invalid chars
		if (! isLineValid(line) )
		{
			continue;
		}

		count_lines++;
	}

	_ui.db_View->setModel(NULL); // disable it for performance reasons

	check_sql_db(_jcnDbFn);

	if (Jcn::dbg)
		std::cout << "Count qry" << std::endl;

	QSqlQuery qry  = _db.exec("SELECT count(*) from jcn_table");
	if ( qry.lastError().isValid() )
	{
		std::cout << "QRY Error: " << qry.lastError().databaseText() << std::endl;
		QMessageBox msgBox;
		msgBox.setText("QRY Error: " + qry.lastError().databaseText());
		msgBox.exec();
		exit(-1);
	}

	int countRec;
	while (qry.next()) 
	{
       	countRec = qry.value(0).toInt();
		if (Jcn::dbg)
			std::cout << "countRec=" << countRec << std::endl;
   	}

	QProgressDialog progress("Loading JCN SQL Database ...", "OK", 0, countRec, this);

	QByteArray titleText = "JCN Tool (FELIX): txtdb=" + _jcnDbFn;
	titleText += "   sqldb="+_jcnDbFn+".sql";
	setWindowTitle(titleText);

	
	_sqlDbModel = new JcnSqlModel(_ui.db_View, this);
	_sqlDbModel->setQuery("SELECT * from jcn_table");
	_ui.db_View->setModel(_sqlDbModel);
	_sqlDbModel->setHeaders();

	dbglog("finish - load_jcn_db(fname): ");
}
// --------------------------------------------------------------------
void 
Jcn::closeEvent(QCloseEvent * e)
{
    exitApp();
}

// ------------------------------------------------------------------
void 
Jcn::tab_selected(QString s)
{
	if (Jcn::dbg > 10)
		std::cout << "tab_selected" << std::endl;
}

// ------------------------------------------------------------------
// ------------------------------------------------------------------
// calc tab slots
// ------------------------------------------------------------------
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
	if (Jcn::dbg > 10)
		std::cout << "translate_clicked" << std::endl;

	QString dispStr = _ui.jcnText_LE->toPlainText();
	
	QByteArray workStr = _ui.jcnText_LE->toPlainText().toAscii();
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
	
	workStr.prepend(_ui.jcnStr_LIST->currentItem()->text().toAscii() + " ");
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
	if (Jcn::dbg > 10)
		std::cout << "add_to_db_clicked" << std::endl;

	QString dispStr =_ui.jcnText_LE->toPlainText();
	
	QByteArray workStr = _ui.jcnText_LE->toPlainText().toAscii();
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
	TL (logText);

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

	workStr.prepend(_ui.jcnStr_LIST->currentItem()->text().toAscii() + " ");
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
	if ( _dbModel->addEntry(calc) ) // good add
	{
		// lets add to SQLDB if sql is on
		QSqlQuery query  = _db.exec(calc.toSqlInsert());
		if ( query.lastError().isValid() )
		{
			std::cout << "QRY Error: " << query.lastError().databaseText() << std::endl;
			std::cout <<  "   " << calc.toSqlInsert() << std::endl;

			QByteArray logText = "QRY Error: ";
			logText += " dbt=" + query.lastError().databaseText().toAscii();
			logText += " drt=" + query.lastError().driverText().toAscii();
			logText += " n=" + QByteArray::number(query.lastError().number());
			logText += " [" + calc.text() + "]";
			TL (logText);

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
									query.lastError().driverText().toAscii() + " " +
									QByteArray::number(query.lastError().number())
									);
				msgBox.exec();
			}
		}

		// also write to file
   		QFile file(_jcnDbFn);
   		if(! file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append))
   		{
   			QMessageBox msgBox;
			msgBox.setText("Can't open to file: " + _jcnDbFn);
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
	if (Jcn::dbg > 10)
		std::cout << "find_clicked" << std::endl;

	_ui.main_TW->setCurrentIndex(2);

	QString dispStr =_ui.jcnText_LE->toPlainText();

	QByteArray workStr = _ui.jcnText_LE->toPlainText().toAscii();
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
	
	workStr.prepend(_ui.jcnStr_LIST->currentItem()->text().toAscii() + " ");
	workStr = workStr.simplified();

	if (workStr.length()==0) return;
		
	// set search string
	_ui.find_jcnText_LE->setText(QString(workStr));
	
	// click button to execute it
	_ui.find_calc_PB->click();
}
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
		t[c]=oneItem->text().toAscii();
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

	if (Jcn::dbg > 10)
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
	if (Jcn::dbg > 10)
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
	if (Jcn::dbg > 10)
		std::cout << "sql_find_calc_clicked" << std::endl;
	
	QString dispStr =_ui.find_jcnText_LE->toPlainText();
	
	QByteArray workStr;
	workStr = _ui.find_jcnText_LE->toPlainText().toAscii();
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

	if (Jcn::dbg)
		std::cout << "Start Match: " << workStr << std::endl;



	JcnCalc workCalc(workStr);
	QSqlQuery countQuery("SELECT count(*) from jcn_table");
	int countRec;
	while (countQuery.next()) 
	{
         countRec = countQuery.value(0).toInt();
		 if (Jcn::dbg)
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
				
	if (Jcn::dbg)
		std::cout << sqlStr << std::endl;

	return sqlStr;
}
// --------------------------------------------------------------------
void 
Jcn::find_options_clicked()
{
	if (Jcn::dbg)
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


	if (Jcn::dbg)
		std::cout << "find_find_clicked" << std::endl;

	JcnModel * m =	(JcnModel*)_ui.find_View2->model();

	QList<QModelIndex> list = _ui.find_View2->selectionModel()->selectedRows(0);
	if (list.size() == 0) return; // nothing selected
	
	// only chose one
	QVariant var = m->data(m->index(list.at(0).row(), 0, QModelIndex()));
	QByteArray workStr = var.toString().toAscii().simplified();

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
	
	QByteArray workStr = wi.at(0)->text(0).toAscii().simplified();

	if (Jcn::dbg)
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
		QByteArray workStr = dispStr.toAscii().simplified().toUpper();
		workStr = workStr.simplified().toUpper();
		
		if (workStr.length() == 0)	return;

		if (! isLineValid(workStr) )
		{
			QMessageBox msgBox;
			msgBox.setText("Invalid data: " + workStr);
			msgBox.exec();
			return;
		}	

		QByteArray logText = "add_new_to_db_clicked: " + workStr;
		TL (logText);

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

		workStr.prepend(_ui.jcnStr_LIST->currentItem()->text().toAscii() + " ");
		workStr = workStr.simplified().toUpper();

		if (workStr.length() == 0)	return;

		if (! isLineValid(workStr) )
		{
			QMessageBox msgBox;
			msgBox.setText("Invalid data: " + workStr);
			msgBox.exec();
			return;
		}	

		JcnCalc calc(workStr);

		if ( _dbModel->addEntry(calc) ) // good add
		{
			QSqlQuery query  = _db.exec(calc.toSqlInsert());
			if ( query.lastError().isValid() )
			{
				std::cout << "QRY Error: " << query.lastError().databaseText() << std::endl;
				std::cout << "   " << calc.toSqlInsert() << std::endl;

				QByteArray logText = "QRY Error: ";
				logText += " dbt=" + query.lastError().databaseText().toAscii();
				logText += " drt=" + query.lastError().driverText().toAscii();
				logText += " n=" + QByteArray::number(query.lastError().number());
				logText += " [" + calc.text() + "]";
				TL (logText);

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
									query.lastError().driverText().toAscii() + " " +
									QByteArray::number(query.lastError().number())
									);
					msgBox.exec();
				}					
			}

		   	QFile file(_jcnDbFn);
		   	if(! file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
		   	{
	   			QMessageBox msgBox;
	 			msgBox.setText("Can't open to file: " + _jcnDbFn);
 				msgBox.exec();
				return;
			}
		
			file.write("\r\n" + calc.text() + "\r\n"); // use CR and NL
			
			file.close();
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


	JcnModel * m =	(JcnModel*)_ui.db_View->model();

	QList<QModelIndex> list = _ui.db_View->selectionModel()->selectedRows(0);
	if (list.size() == 0) 
	{
		std::cout << "nothing selected - return" << std::endl;
		return; // nothing selected
	}
	
	// only chose one
	int itemRow = list.at(0).row();
	QVariant var = m->data(m->index(itemRow, 0, QModelIndex()));
	QByteArray workStr = var.toString().toAscii().simplified();
	
	// std::cout << "[" << workStr << "]" << std::endl;

	TL("rmv_from_db_clicked " + workStr);
	
	JcnCalc c;
	if ( m->findEntry(workStr, c) ) // c has index
	{
		// save current db as a bak file
		QString backfn = _jcnDbFn+".bak."+QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
   		QFile tmpfile(_jcnDbFn);
		if ( ! tmpfile.rename(backfn) )
		{
			QMessageBox msgBox;
			TL("Can backup to file: " + backfn.toAscii());
 			msgBox.setText("Can backup to file: <b>" + backfn + "<b>");
 			msgBox.exec();
			return;
		}
		
		// lets open new db file
		// lets start reading backup and wring into new db
   		QFile dbfile(_jcnDbFn);
   		if(! dbfile.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append))
   		{
   			QMessageBox msgBox;

			TL("Can't open file: " + _jcnDbFn);
			msgBox.setText("Can't open file: " + _jcnDbFn);
			msgBox.exec();
			
			// we failed - let's return backup to original db
	   		QFile tmpfile(backfn);
			tmpfile.rename(_jcnDbFn);
			return;
		}


   		QFile bak_dbfile(backfn);
   		if(! bak_dbfile.open(QIODevice::ReadOnly | QIODevice::Text ) )
   		{
   			QMessageBox msgBox;
			TL("Can't open file: " + backfn.toAscii());
			msgBox.setText("Can't open file: " + backfn);
			msgBox.exec();

			// we failed - let's return backup to original db
	   		QFile tmpfile(backfn);
			tmpfile.rename(_jcnDbFn);			
			return;
		}

		QByteArray line;
		while( !bak_dbfile.atEnd() )
		{
			// lets read until we find match
	   		// strip leading and trailing spaces
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
		

		// remove from view
		m->rmvEntry(c, itemRow);

		if (m != _dbModel)
		{
			// must remove from main model also

			QList<QStandardItem *> lst = _dbModel->findItems (workStr);
			if ( lst.size() > 0 )
			{
				for (int k=0; k < lst.size(); ++k)
				{
					QStandardItem * itm = lst.at(k);
					QModelIndex idx = _dbModel->indexFromItem(itm);

					_dbModel->takeRow(idx.row());
				}
			}
		}
		
		
		if ( ! _db.transaction() )
		{
			std::cout << "Failed to start txn: " << std::endl;				
			return;
		}

		JcnCalc wc(workStr);

		//std::cout << wc.toSqlDelete() << std::endl;

		QSqlQuery query  = _db.exec(wc.toSqlDelete());
		if ( query.lastError().isValid() )
		{
			std::cout << "DELETE QRY Error: " << query.lastError().databaseText() << std::endl;
			std::cout << "   " << wc.toSqlDelete() << std::endl;

			QByteArray logText = "DELETE QRY Error: ";
			logText += " dbt=" + query.lastError().databaseText().toAscii();
			logText += " drt=" + query.lastError().driverText().toAscii();
			logText += " n=" + QByteArray::number(query.lastError().number());
			logText += " [" + wc.text() + "]  ";
			logText += wc.toSqlDelete();
			TL (logText);

   			QMessageBox msgBox;
			msgBox.setText("DELETE QRY Error: " + query.lastError().databaseText());
			msgBox.exec();
			return;
		}

		if ( ! _db.commit() )
		{
			std::cout << "Failed to commit txn:" << std::endl;			
			return;
		}
	}
	else
	{
		std::cout << "no index: " << std::endl;
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

	JcnModel * m =	(JcnModel*)_ui.db_View->model();

	QList<QModelIndex> list = _ui.db_View->selectionModel()->selectedRows(0);
	if (list.size() == 0) return; // nothing selected

	_ui.main_TW->setCurrentIndex(2);
	
	// only chose one
	QVariant var = m->data(m->index(list.at(0).row(), 0, QModelIndex()));
	QByteArray workStr = var.toString().toAscii().simplified();

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
	JcnModel * mdl = (JcnModel*)_ui.db_View->model();
	
	QList<QStandardItem *> lst = mdl->findItems(str, Qt::MatchStartsWith);
	if ( lst.size() > 0 )
	{
		QStandardItem * itm = lst.at(0);

		// select it				
		_ui.db_View->clearSelection();								
		_ui.db_View->setCurrentIndex(itm->index());
	}		
}
// --------------------------------------------------------------------
void 
Jcn::db_filters_clicked()
{
	if (Jcn::dbg)
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
	if (Jcn::dbg)
		std::cout << "textFilter_textChanged: " << std::endl;

	_ui.numFilterDb_LE->setText("");
	_ui.sqlText_LE->setText("");
}
// ------------------------------------------------------------------
void 
Jcn::numFilter_textChanged(const QString & str)
{
	if (Jcn::dbg)
		std::cout << "numFilter_textChanged: " << std::endl;

	_ui.textFilterDb_LE->setText("");
	_ui.sqlText_LE->setText("");
}
// ------------------------------------------------------------------
void 
Jcn::expFilter_textChanged(const QString & str)
{
	if (Jcn::dbg)
		std::cout << "expFilter_textChanged: " << std::endl;

	_ui.textFilterDb_LE->setText("");
	_ui.numFilterDb_LE->setText("");
}
// ------------------------------------------------------------------
void 
Jcn::applyFilterDb_clicked()
{
	if (Jcn::dbg)
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
			std::cout << _tmpSqlDbModel->lastError().text() << std::endl;

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
		_tmpSqlDbModel->setQuery(sql_str);		
		if ( _tmpSqlDbModel->lastError().isValid() )
			std::cout << _tmpSqlDbModel->lastError().text() << std::endl;
	 	
		_ui.db_View->setModel(_tmpSqlDbModel);

		_tmpSqlDbModel->setHeaders();
	}

}
// ------------------------------------------------------------------
void 
Jcn::resetFilterDb_clicked()
{
	if (Jcn::dbg)
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
	if (Jcn::dbg)
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
		workStr = text_str.toAscii();
	}
	else
	if ( num_str != "")  // text filter
	{
		workStr = num_str.toAscii();
	}
	else
	if ( exp_str != "")  // text filter
	{
		workStr = exp_str.toAscii();
	}	
	
	tdp->setHeaderText("Filter: " + workStr);
	tdp->setGridMode(TDP::NormalGrid);
	tdp->exec();
	
	if (tdp->result() == QDialog::Accepted)
		tdp->print();
		
	delete tdp;
}

// ------------------------------------------------------------------
// ------------------------------------------------------------------
// find tab slots
// ------------------------------------------------------------------
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
	workStr = _ui.find_jcnText_LE->toPlainText().toAscii();
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



// ------------------------------------------------------------------
// menu actions
// ------------------------------------------------------------------
// file menu
// ------------------------------------------------------------------
void 
Jcn::createDb()
{
	QString openfn = QFileDialog::getSaveFileName(this,
     						tr("Create New File"), _jcnRootDir, tr("*"));
	if (openfn=="") return;

	QFile dbfile(openfn);
	if(! dbfile.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append))
	{
		QMessageBox msgBox;
		msgBox.setText("Can't create file: " + openfn);
		msgBox.exec();
			
		return;
	}

	load_jcn_db(openfn.toAscii(), true);
	
	return;			
}
// ------------------------------------------------------------------
void 
Jcn::openDb()
{
	QString openfn = QFileDialog::getOpenFileName(this,
     						tr("Open File"), _jcnRootDir, tr("*"));
	if (openfn=="") return;

	load_jcn_db(openfn.toAscii(), true);
	
	return;			
}
// ------------------------------------------------------------------
void 
Jcn::importDb()
{
	QString importfn = QFileDialog::getOpenFileName(this,
     						tr("Open File"), _jcnRootDir, tr("Db (jcn_db*);; Text files (*.txt);; All files (*)" ));
	if (importfn=="") return;

   	QFile impfile(importfn);
   	if(! impfile.open(QIODevice::ReadOnly | QIODevice::Text))
   	{
   		QMessageBox msgBox;
 		msgBox.setText("Can't open import file: " + importfn);
 		msgBox.exec();
		return;
	}

	QByteArray line;
	int count_lines=0;
	while( !impfile.atEnd() )
	{
		line = impfile.readLine();

		// check line for invalid chars
		if (! isLineValid(line) )
		{
			continue;
		}

		count_lines++;
	}

	_ui.db_View->setModel(NULL); // disable it for perf reason

	impfile.reset();
	
	QString backfn = _jcnDbFn+".bak."+QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
	if ( ! QFile::copy (_jcnDbFn, backfn) )
	{
		QMessageBox msgBox;
 		msgBox.setText("Can backup to file: <b>" + backfn + "<b>");
 		msgBox.exec();
		return;
	}

	// open real db file to append
	QFile dbfile(_jcnDbFn);
	if(! dbfile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
	{
   		QMessageBox msgBox;
 		msgBox.setText("Can't open db file: " + _jcnDbFn);
 		msgBox.exec();
		return;
	}
			
	QProgressDialog progress("Importing Database...", "Ok", 0, count_lines, this);
	int count = 0;
	while( !impfile.atEnd() )
	{
    	// strip leading and trailing spaces
		line = impfile.readLine().simplified();
		if (line[0] == '#' || line.length() == 0) continue;

		// check line for invalid chars
		if (! isLineValid(line) )
		{
			continue;
		}

		JcnCalc calc(line);
		if ( _dbModel->addEntry(calc) ) // good add
		{
			dbfile.write("\r\n" + calc.text() + "\r\n"); // use CR and NL
		}

    	progress.setValue(count++);
        _qapp->processEvents();		
	}

	_ui.db_View->setModel(_dbModel); // reenable it
	_dbModel->setHeaders();
	
	impfile.close();
	dbfile.close();

	QMessageBox msgBox;
 	msgBox.setText("Import is completed: Restart QJcn");
 	msgBox.exec();	
    _qapp->processEvents();
	sleep(5);
	exit(0);
	
	return;			
}
// ------------------------------------------------------------------
void 
Jcn::exportDb()
{
	QString expfn = _jcnDbFn+"."+QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
	if ( QFile::copy (_jcnDbFn, expfn) )
	{
		QMessageBox msgBox;
 		msgBox.setText("Exported to file: <b>" + expfn + "<b>");
 		msgBox.exec();
		return;
	}
	else
	{
		QMessageBox msgBox;
 		msgBox.setText("Can't export to file: " + expfn);
 		msgBox.exec();
		return;		
	}
}
// ------------------------------------------------------------------
void 
Jcn::editDb()
{
std::cout << "Edit db" << std::endl;

	TextEdit *  ted = new TextEdit(this);
    ted->resize( 700, 800 );

	// get checksum of this file
	FileChkSum fcs(_jcnDbFn);
	unsigned long long start_csm = fcs.calc();
	
	ted->load(_jcnDbFn);

    ted->show();
	
	while (ted->isVisible())
	{
		QApplication::processEvents(QEventLoop::WaitForMoreEvents);
	}

	unsigned long long end_csm = fcs.calc();
		
	delete ted;

	std::cout << "Go ON" << std::endl;	
	if (start_csm != end_csm)
	{
		std::cout << "File changed: " 
				  << " start chksum=: " <<  start_csm
				  << " end chksum=: " << end_csm
				<< std::endl;
		
		std::cout << "Rebuild database" << std::endl;

		QByteArray sqlFileName = _jcnDbFn + ".sql";
		if (! QFile::remove(sqlFileName) )
		{
			std::cout << "Can not remove " << sqlFileName << std::endl;
		}

		load_jcn_db();
	}
}
// ------------------------------------------------------------------
void 
Jcn::exitApp()
{
    std::cout << "Exit." << std::endl;
    exit(0);
}

// --------------------------------------------------------------------
// misc menu
// --------------------------------------------------------------------
void 
Jcn::generate()
{
	if (Jcn::dbg)
		std::cout << "generate" << std::endl;

	QByteArray workStr = _ui.jcnText_LE->toPlainText().toAscii();
	
   	QFile file(_jcnDbFn);
   	if(! file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append))
   	{
   		QMessageBox msgBox;
		msgBox.setText("Can't open to file: " + _jcnDbFn);
		msgBox.exec();
		return;
	}

	for (int i=0; i < _ui.jcnStr_LIST->count(); ++i)
	{
		QListWidgetItem * item  = _ui.jcnStr_LIST->item(i);

		QByteArray writeStr = item->text().toAscii() + " " + workStr;
		writeStr = writeStr.simplified();

		JcnCalc calc(writeStr);
		if ( _dbModel->addEntry(calc) ) // good add
		{		
			// lets add to SQLDB if sql is on
			QSqlQuery query  = _db.exec(calc.toSqlInsert());
			if ( query.lastError().isValid() )
			{
				if (Jcn::dbg)
				{
					std::cout << "QRY Error: " << query.lastError().databaseText() << std::endl;
					std::cout << "  " << writeStr << std::endl;
				}
			}

			file.write("\r\n" + calc.text() + "\r\n"); // use CR and NL
		}
	}
	
	file.close();
}
// --------------------------------------------------------------------
void 
Jcn::fullMatches()
{
	_ui.main_TW->setCurrentIndex(3);
	
	if (Jcn::dbg)
		std::cout << "fullMatches" << std::endl;

	_ui.fullMatch_TW->clear();
	
	QByteArray rangeLimitSql = "";
	if ( _ui.rl_GB->isChecked() )
	{
		int f=0, t=0;

		// F
		f = _ui.rl_f_hv_1_LE->text().simplified().toInt();
		t = _ui.rl_f_hv_2_LE->text().simplified().toInt();

		if ( f > 0 && t >= f)
		{			
			rangeLimitSql = " where ( f_hv between " + _ui.rl_f_hv_1_LE->text().toAscii() + " and " + _ui.rl_f_hv_2_LE->text().toAscii() + ") ";
		}

		// B
		f = _ui.rl_b_hv_1_LE->text().simplified().toInt();
		t = _ui.rl_b_hv_2_LE->text().simplified().toInt();

		if ( f > 0 && t >= f)
		{
			if (rangeLimitSql == "") // not defined
				rangeLimitSql += " where ";
			else
				rangeLimitSql += " and "; // and it wit hprior def

			rangeLimitSql += " (f_hv between " + _ui.rl_b_hv_1_LE->text().toAscii() + " and " + _ui.rl_b_hv_2_LE->text().toAscii() + ") ";
		}


		// FB
		f = _ui.rl_fb_hv_1_LE->text().simplified().toInt();
		t = _ui.rl_fb_hv_2_LE->text().simplified().toInt();

		if ( f > 0 && t >= f)
		{
			if (rangeLimitSql == "") // not defined
				rangeLimitSql += " where ";
			else
				rangeLimitSql += " and "; // and it wit hprior def

			rangeLimitSql += " (f_hv between " + _ui.rl_fb_hv_1_LE->text().toAscii() + " and " + _ui.rl_fb_hv_2_LE->text().toAscii() + ") ";
		}

	}

//std::cout << "rangeLimitSql: " << rangeLimitSql << std::endl;

	QSqlQuery countQuery("SELECT count(*) from jcn_table " + rangeLimitSql);
	int countRec;
	while (countQuery.next()) 
	{
         countRec = countQuery.value(0).toInt();
		 std::cout << "countRec=" << countRec << std::endl;
		 _ui.count_LB->setText("Matching: " + QString::number(countRec) + " records");
    }

	QProgressDialog progress("Full Match Search...", "Cancel", 0, countRec, this);

//		std::cout << "Top Level: " << ts_hmsu() << std::endl;
	QSqlQuery top_lvl_qry;
	if ( ! top_lvl_qry.exec( "SELECT * from jcn_table " + rangeLimitSql) )
	{
		std:: cout << top_lvl_qry.lastError().databaseText() << std::endl;

		QMessageBox msgBox;
		msgBox.setText("Full Match : Can't run top_lvl_qry: ");
		msgBox.exec();
		return;
	}

	int par_count=0;
	while ( top_lvl_qry.next() ) // top level query
	{
//			std::cout << "Top Level: " << ts_hmsu() << std::endl;

		par_count++;

	    progress.setValue(par_count);
        _qapp->processEvents();
		if (progress.wasCanceled()) return;

		JcnCalc c_par; // create new calc from query
		c_par.fromSqlQuery(top_lvl_qry);

//			std::cout << "   1 Btm Level: " << ts_hmsu() << std::endl;
		QSqlQuery btm_lvl_qry;
		if ( ! btm_lvl_qry.exec( c_par.toSqlFullFind() ) )
		{
			std:: cout << btm_lvl_qry.lastError().databaseText() << std::endl;

			QMessageBox msgBox;
			msgBox.setText("Full Match : Can't run btm_lvl_qry: ");
			msgBox.exec();
			return;
		}

		QTreeWidgetItem * parent = NULL;
		int chld_count = 0;
		while ( btm_lvl_qry.next() ) // bottom level query
		{
			JcnCalc c_chld; // create new calc from query
			c_chld.fromSqlQuery(btm_lvl_qry);

			if (c_chld.text().simplified() == c_par.text().simplified()) continue; // skip itself

			chld_count++;

			if (!parent)
			{
				QStringList lst;
				lst << c_par.text() << 0;
				parent = new QTreeWidgetItem(lst);
				_ui.fullMatch_TW->addTopLevelItem(parent);
			}

			QTreeWidgetItem * chld = new QTreeWidgetItem;
			chld->setText(0, c_chld.text() );
			parent->addChild(chld);
			parent->setText(1, QByteArray::number(chld_count) );

		}
//			std::cout << "   2 Btm Level: " << ts_hmsu() << std::endl;			
	}
}

// --------------------------------------------------------------------
// help menu
// --------------------------------------------------------------------
void 
Jcn::aboutApp()
{
    QString msg;
	msg = "This is <b>JCN Plus</b> tool, " + 
		QByteArray(xstr(JCN_BUILD_DATE)) + "<br>";
	msg += "Based on research by Sheree Rainbolt-Kren<br>";
	msg += "Written by Felix Singer";
    QMessageBox::about( this, "About", msg);
}
// --------------------------------------------------------------------
void 
Jcn::notesApp()
{
	//QMessageBox * mbox = new QMessageBox(this);
/*	
    QString msg;
	msg = "Notes for <b>JCN Plus</b> tool<br><br>";
	msg += "When this tool runs for the first time it will create a folder called <b>qjcn</b> on your C: drive.<br>";
	msg += "In will also place initial <b>jcn_db.txt</b> and <b>jcn_strings.txt</b> in that folder.<br>";
	msg += "<b>jcn_db.txt</b> is your database, this is where JCN Tool will add new texts.<br>";
	msg += "<b>jcn_strings.txt</b> is where predefined strings are kept.<br>";
	msg += "Both files are editable with any text editor (notepad, wordpad etc).<br>";
	msg += "If you choose to edit these files manually please make sure that JCN Tool is not running.<br>";
	msg += "<br>";	
	msg += "There are three tabs, <b>'Translate'</b>, <b>'Database'</b>, <b>'Find/Compare'</b><br>";
	msg += "<br>";
	msg += "<b>'Translate'</b> tab will allow you to experiment with different texts.<br>";
	msg += "Enter any text into a text field and press 'Translate' button.<br>";
	msg += "Optionally text can be prepended with predefined JCN string.<br>";
	msg += "Leave predefined JCN String selection blank if you do not need it.<br>";
	msg += "Predefined JCN Strings are stored in a file called 'jcn_strings.txt'.<br>";
	msg += "This is a plain text file and can be edited with any text editor'.<br>";
	msg += "Results can be printed by pressing 'Print' button and permanently<br>";
	msg += "stored in a database for future use by pressing 'Add To Database' button.<br>";
	msg += "Result can also be used to search a database for matches by pressing<br>";
	msg += "'Find/Compare' button.<br>";
	msg += "<br>";
	msg += "<b>'Database'</b> tab will allow you to browse your JCN data.<br>";
	msg += "This data is stored in a file called 'jcn_db.txt'.<br>";
	msg += "This is a plain text file and can be edited with any text editor.<br>";
	msg += "Use 'Search Field' to locate any specific text entry.<br>";
	msg += "Use 'Filters' and 'Apply'/'Reset' buttons to narrow or restore your database view.<br>";
	msg += "Filters can be applied to text or any number<br>";
	msg += "Use 'Find/Compare' button to search for matches.<br>";
	msg += "You can also right click to select an entry and use 'Find/Compare' to search as well.<br>";
	msg += "<br>";
	msg += "<b>'Find/Compare'</b> tab will allow you to find and compare various texts<br>";
	msg += "You can come here from other tabs or enter any text ypu wish into an entry field<br>";
	msg += "Use 'Opt' button to choose your matching methods.<br>";
	msg += "You can also right click to select an entry and use 'Find/Compare' to search as well.<br>";
	msg += "based on results of JCN translations.<br>";
	
	msg += "<br>";
*/

//mbox->setDetailedText(msg);
//mbox->show();
//QMessageBox::about( this, "Notes", msg);

	QTextBrowser * qtb = new QTextBrowser;
	qtb->setHtml(help_html);
	qtb->resize(700,700);
	qtb->setWindowTitle("JCN Tools Guide");
	qtb->show();
}
// --------------------------------------------------------------------
