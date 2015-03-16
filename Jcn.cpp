#ifdef Q_WS_WIN  // if we run under windows
#include <winbase.h>
#endif

#ifdef JCNOS_LINUX
#include <pwd.h>
#endif

#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <QtCore/QSysInfo>

#ifdef WITH_QT5
	#include <QtWidgets/QMessageBox>
	#include <QtWidgets/QScrollBar>
	#include <QtWidgets/QProgressDialog>
	#include <QtWidgets/QTextBrowser>
#else
	#include <QtGui/QtGui>
	#include <QtGui/QMessageBox>
#endif


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


// --------------------------------------------------------------------
Jcn::Jcn(QApplication * a, QMainWindow * parent):
    QMainWindow(parent),
    _qapp(a),
	_findModel1_rev(NULL),
	_findModel2_rev(NULL),
	_revMode(false)
{
	_sqlDbModel = NULL;
	_tmpSqlDbModel = NULL;

	check_data();

	create_ui();

	load_data();

}

// -------------------------------------------------------------
void
Jcn::create_ui()
{
	JCNLOG("create ui");

    _ui.setupUi(this);

	JCNLOG("create menus");

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

	// menu settings
//    connect(_ui.actionCustomColumns, SIGNAL(triggered()), this, SLOT(custCol()));

	// menu help
    connect(_ui.actionAbout, SIGNAL(triggered()), this, SLOT(aboutApp()));
    connect(_ui.actionNotes, SIGNAL(triggered()), this, SLOT(notesApp()));

	// for clean exit
    connect(_qapp, SIGNAL(lastWindowClosed()), this, SLOT(exitApp()));

	// main tab
    connect( _ui.main_TW, SIGNAL(selected(QString)), this, SLOT(tab_selected(QString)) );


	JCNLOG("create tabs");

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


	QString ttmsg = "Enter a valid SQL WHERE criteria in a folowing form: <br>";
	ttmsg += "  <u><b>column=value and column=value or column=value</b></u>    etc...<br><br>";
	ttmsg += "Valid columns are:<br><b> ";

	for (int c=0; c < JcnDD_Last; ++c)
	{
		ttmsg += JcnDD[c].sql_tag;
		ttmsg += ", ";
		if (c%7==0) ttmsg += "<br>";
	}
	ttmsg += "</b><br>";

	ttmsg += "SQL WHERE Criteria Examples: <br>";
	ttmsg += "  f_h=66<br>";
	ttmsg += "  f_h like '%66%'<br>";
	ttmsg += "  f_h=88 and b_h=92<br>";
	ttmsg += "  (f_h=88 and b_h=92) or (f_h=88+1 and b_h=92-1) or (f_h=88-1 and b_h=92+1)<br>";
	ttmsg += "  f_h like '%88%' and b_h like '%86%'<br>";
	ttmsg += "  jcntext like '%reiki%'<br>";
	ttmsg += "  jcntext like '%reiki%' and f_h=111<br>";
	ttmsg += "  jcntext like '%reiki%' and (f_h=111 or f_h=99)<br>";

	_ui.sqlText_LE->setToolTip(ttmsg);

	// sqlite db
	_db = QSqlDatabase::addDatabase("QSQLITE");
//	_memdb = QSqlDatabase::addDatabase("QSQLITE");

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

	_ui.main_TW->setCurrentIndex(0); // set to translate tab at start

	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//	_ui.f_GB->hide();	//
//	_ui.b_GB->hide();	//
//	_ui.fb_GB->hide();	//

	_ui.findChoice_Frame->hide();	// make search choice invisible
	_ui.filter_Frame->hide();		// make filter options invisible

//	_ui.addNewToDb_PB->setDisabled(true);
//	_ui.rmvFromDb_PB->setDisabled(true);

}
// -------------------------------------------------------------
void
Jcn::load_data()
{
	JCNLOG("start load_data");

	load_jcn_strings();

	create_jcn_db_view();
	create_find_view();

	createCtxMenu();

	load_jcn_db();

	JCNLOG("finish load_data");
}

// -------------------------------------------------------------
void
Jcn::dbglog(const QString & s, int lev)
{
	if (lev <= jcndbg)
		std::cout << s << std::endl;
}

// -------------------------------------------------------------
/*

	Ensure that data path is set up correctly
	Create directory if needed
	Create log file
	Create initial data files if needed

*/
// -------------------------------------------------------------
void
Jcn::check_data()
{
QString logText;

	JCNLOG("start - check_data");

#ifdef Q_WS_WIN  // if we run under windows
	_jcnRootDir = jcn_dir_win_def;
#else // unix

	struct passwd *pw = getpwuid(getuid());
	const char * homedir = pw->pw_dir;
	_jcnRootDir = homedir;
	_jcnRootDir += jcn_dir_unix_def;
#endif

	JCNLOG(" checking path: " + _jcnRootDir);

	QDir d(_jcnRootDir);

	// if dir does not exist then make a new one
	// this must be a first run - we must create initial database
	if (! d.exists() )
	{
		JCNLOG(" creating path: " + _jcnRootDir);
		if ( ! d.mkpath(_jcnRootDir) ) // create it
		{
			logText = " -- can't create path: " + _jcnRootDir;
			JCNLOG(logText);

	   		QMessageBox msgBox;
 			msgBox.setText(logText);
			msgBox.exec();
			exit(-1);
		}
	}

	//----------------
	// create log file
	//----------------
	JCNLOG(" creating log file: " + _jcnRootDir  + "/" + jcn_log_fname);
	log_file.setFileName(_jcnRootDir  + "/" + jcn_log_fname);
	if (! log_file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate) )
	{
		logText = " -- can't create log file: " + _jcnRootDir  + "/" + jcn_log_fname;
		JCNLOG(logText);

		QMessageBox msgBox;
		msgBox.setText(logText);
		msgBox.exec();
		exit(-1);
	}


	//------------------------------------------------------------------------
	// check jcn string question file - if does not exist create initial one
	//------------------------------------------------------------------------
	JCNLOG(" checking file: " + _jcnRootDir  + "/" + jcn_str_fname_def);
	// check jcn_strings.txt
	QFile q_file(_jcnRootDir  + "/" + jcn_str_fname_def);
	if (! q_file.exists() )
	{
		JCNLOG(" creating file: " + _jcnRootDir  + "/" + jcn_str_fname_def);
		// create default jcn_strings.txt
   		if(! q_file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
   		{
			logText = " -- can't create file: " + _jcnRootDir  + "/" + jcn_str_fname_def;
			JCNLOG(logText);

   			QMessageBox msgBox;
			msgBox.setText(logText);
			msgBox.exec();
			exit(-1);
		}


		logText="Create default JCN questions file: " + _jcnRootDir  + "/" + jcn_str_fname_def;
		QMessageBox msgBox(
							QMessageBox::Information,
							tr("QJcn"),
							logText,
							QMessageBox::Ok
						  );
		JCNLOG(logText);
		msgBox.exec();

		int count=0;
		while (jcn_strings_data[count])
		{
			QByteArray test = jcn_strings_data[count];
			if (test == ".") // reached end
				break;

			q_file.write(QByteArray(jcn_strings_data[count]) + "\r\n"); // use CR and NL
//			JCNLOG("Add def question: " + QByteArray(jcn_strings_data[count]));
			count++;
		}
		q_file.flush();
		q_file.close();
	}


	//----------------------------------------------------------------
	// check jcn text db file - if does not exist create initial one
	//----------------------------------------------------------------
	JCNLOG(" checking file: " + _jcnRootDir  + "/" + jcn_db_fname_root_def+".txt");
	// check jcn_db.txt
	QFile s_file(_jcnRootDir  + "/" + jcn_db_fname_root_def+".txt");
	if (! s_file.exists() )
	{
		JCNLOG(" creating file: " + _jcnRootDir  + "/" + jcn_db_fname_root_def+".txt");
		QFile s_file(_jcnRootDir  + "/" + jcn_db_fname_root_def+".txt");
   		if(! s_file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
   		{
			logText = " -- can't create file: " + _jcnRootDir  + "/" + jcn_db_fname_root_def+".txt";
			JCNLOG(logText);

   			QMessageBox msgBox;
			msgBox.setText(logText);
			msgBox.exec();
			exit(-1);
		}


		logText="Create default JCN DB file: " + _jcnRootDir  + "/" + jcn_db_fname_root_def+".txt";
		QMessageBox msgBox(
							QMessageBox::Information,
							tr("QJcn"),
							logText,
							QMessageBox::Ok
						  );
		JCNLOG(logText);
		msgBox.exec();

		int count=0;
		while (jcn_db_data[count])
		{
			QByteArray test = jcn_db_data[count];
			if (test == ".") // reached end
				break;

			s_file.write(QByteArray(jcn_db_data[count]) + "\r\n"); // use CR and NL
//			JCNLOG("Add def data: " + QByteArray(jcn_db_data[count]));
			count++;
		}
		s_file.flush();
		s_file.close();
	}

	JCNLOG("finish: check_data");
}


// -------------------------------------------------------------
void
Jcn::check_sql_db(const QByteArray & fname)
{
	QString logStr;

	JCNLOG("start - check_sql_db: " + fname);

	JCNLOG("Check SQLDB: ");

/*
	JCNLOG("Open SQLDB: memory");

   	_memdb.setDatabaseName(":memory:");
    if ( ! _memdb.open() )
   	{
		JCNLOG("Can not open SQLDB :memory:");

		QMessageBox msgBox;
		msgBox.setText("Can not open SQLDB :memory: Exit.");
		msgBox.exec();
		exit(-1);
		return;
	}
*/


	JCNLOG("Open SQLDB: " + fname);
	//	let's open this database
	QSqlQuery qry;

   	_db.setDatabaseName(fname);
    if ( ! _db.open() )
   	{
		JCNLOG("Can not open SQLDB: " + fname);

		QMessageBox msgBox;
		msgBox.setText("Can not open SQLDB: " + fname + " Exit.");
		msgBox.exec();
		exit(-1);

		return;
	}


	// load in database from text file
	// in case where we use disk database it will need to be done only in case or record number mismatch

	JCNLOG("Testing SQLDB and TextDB.");

	QString logText;
   	QFile file(_jcnTxtDbFn);
   	if(! file.open(QIODevice::ReadOnly | QIODevice::Text))
   	{
		QMessageBox msgBox;
		JCNLOG("Can't open file: " + _jcnTxtDbFn);
		msgBox.setText("Can not open file: " + _jcnTxtDbFn);
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

		dbmap[line] = line; // this will enforce dupes
		count_lines++;
	}
	file.reset();  // for another run


	QByteArray qryVersion="";
	int countRec=0;

	logStr = "Get record count from  disk SQLDB";
	JCNLOG(logStr);


	// let see if numbers of records match
	qry  = _db.exec("SELECT count(*) from jcn_table");
	if ( qry.lastError().isValid() )
	{
		logStr = "QRY Error: " + qry.lastError().databaseText();
		JCNLOG(logStr);

		// lets try to recreate db
		goto DB_CREATE;
	}

	while (qry.next())
	{
        countRec = qry.value(0).toInt();
    }

	logStr = "SQLDB countRec=" + QString::number(countRec) + " dbmap size=" + QString::number(dbmap.size());
	JCNLOG(logStr);

	if ( countRec != dbmap.size() )
	{
		goto DB_CREATE;
	}


	// --------------------------
	// check version
	// --------------------------

	logStr = "Get version number";
	JCNLOG(logStr);

	// lets check version
	qry  = _db.exec("SELECT * from jcn_version limit 1");
	if ( qry.lastError().isValid() )
	{
		logStr = "QRY Error: " + qry.lastError().databaseText();
		JCNLOG(logStr);

		// lets try to recreate db
		goto DB_CREATE;
	}


	while (qry.next())
	{
        qryVersion = qry.value(0).toString().toLatin1();
    }

	logStr = "Version=" + qryVersion;
	JCNLOG(logStr);

	if ( qryVersion != QByteArray(xstr(JCN_BUILD_DATE)) )
	{
		logStr = "Version missmatch:  recorded=" + qryVersion +
				"  actual=" + QByteArray(xstr(JCN_BUILD_DATE));
		JCNLOG(logStr);
		goto DB_CREATE;
	}


	goto DB_GOOD;


	DB_CREATE:
	{
		logStr = "Creating new SQLDB: ";
		JCNLOG(logStr);

		// drop table just in case - if does not exist its ok
		qry  = _db.exec("drop table jcn_table"); // just in case
		qry  = _db.exec("drop table jcn_version"); // just in case

		QByteArray sql;

		sql = JcnCalc::toSqlCreate();
		logStr = "Creating schema: " + sql;
		JCNLOG(logStr);

		// re-create schema
		qry  = _db.exec(sql);
		if ( qry.lastError().isValid() )
		{
			std::cout << qry.lastError().databaseText() << std::endl;
			std::cout << "Failed to execute sql: " << sql << std::endl;
			JCNLOG("Failed to execute sql: " + qry.lastError().databaseText().toLatin1() );

			QMessageBox msgBox;
			msgBox.setText("Failed to execute sql: " + qry.lastError().databaseText() + " Exit.");
			msgBox.exec();
			exit(-1);
		}

/* @@@ 20120724 when I create idx compare does not work

		logStr = "Creating indexes: ";
		JCNLOG(logStr);

		if (! JcnCalc::idxSqlCreate(_db) )
		{
			std::cout << "Failed to create idx: "<< std::endl;
			JCNLOG("Failed to create idx: " );

			QMessageBox msgBox;
			msgBox.setText("Failed to create idx: Exit.");
			msgBox.exec();
			exit(-1);
		}
*/

		// record version information
		sql = "CREATE TABLE jcn_version (data TEXT); ";
		qry  = _db.exec(sql);
		if ( qry.lastError().isValid() )
		{
			std::cout << qry.lastError().databaseText() << std::endl;
			std::cout << "Failed to execute sql: " << sql << std::endl;
			JCNLOG("Failed to execute sql: " + qry.lastError().databaseText().toLatin1() );

			QMessageBox msgBox;
			msgBox.setText("Failed to execute qr: " + qry.lastError().databaseText() + " Exit.");
			msgBox.exec();
			exit(-1);
		}


		sql = "INSERT INTO jcn_version ";
		sql += " (data) ";
		sql += " VALUES ('" + QByteArray(xstr(JCN_BUILD_DATE)) + "'); ";
		qry  = _db.exec(sql);
		if ( qry.lastError().isValid() )
		{
			std::cout << qry.lastError().databaseText() << std::endl;
			std::cout << "Failed to execute sql: " << sql << std::endl;
			JCNLOG("Failed to execute sql: " + qry.lastError().databaseText().toLatin1() );

			QMessageBox msgBox;
			msgBox.setText("Failed to execute sql: " + qry.lastError().databaseText() + " Exit.");
			msgBox.exec();
			exit(-1);
		}
	}


	DB_RELOAD:
	{
		bool dataValid = true;

		QProgressDialog progress("Re-creating SQL DB ...", "", 0, count_lines, this);

		int count = 0;

		if ( ! _db.transaction() )
		{
			std::cout << "Failed to start txn: " << std::endl;
			JCNLOG( "Failed to start txn: " );

			QMessageBox msgBox;
			msgBox.setText("Failed to start txn: Exit.");
			msgBox.exec();
			exit(-1);

			return;
		}

		JCNLOG("LOAD FROM DISK");

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
				logText += " dbt=" + qry.lastError().databaseText().toLatin1();
				logText += " drt=" + qry.lastError().driverText().toLatin1();
				logText += " n=" + QByteArray::number(qry.lastError().number());
				logText += " [" + c.text() + "]";
				JCNLOG(logText);
			}

    		progress.setValue(count);
        	_qapp->processEvents();
		}


		if ( ! _db.commit() )
		{
			std::cout << "Failed to commit txn:" << std::endl;
			JCNLOG( "Failed to commit txn:" );

			QMessageBox msgBox;
			msgBox.setText("Failed to commit txn: Exit.");
			msgBox.exec();
			exit(-1);

			return;
		}

		if (!dataValid)
		{
			QMessageBox msgBox;
			msgBox.setText("Some data is ommited, it contains inalid symbols:  Check  c:\\qjcn\\jcn.log for errors. ");
			msgBox.exec();
//			exit(-1);
		}
	}


	DB_GOOD:
	{
		JCNLOG( "SQLDB OK" );
	}

	JCNLOG("finish - check_sql_db(fname): " + fname);
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
			(ch == '!')	 ||					// excl is OK
			(ch == ')')	 ||					// parent is OK
			(ch == '(')	 ||					// parent is OK
			(ch == '\\')  ||				// slash is OK
			(ch == '/')						// backslash is OK
			)
		{

		}
		else // no good
		{
			//std::cout << "INVALID LINE: " << line << std::endl;
			JCNLOG("INVALID CHAR IN LINE: " + line);
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
	JCNLOG("start - load_jcn_strings()");

	_jcnTxtStrFn = _jcnRootDir + "/" + fname;
	QString logText;

	JCNLOG("Check file: " + _jcnTxtStrFn);
   	QFile file(_jcnTxtStrFn);
   	if(! file.open(QIODevice::ReadOnly | QIODevice::Text))
   	{

//		logText = "can't open file " + _jcnTxtStrFn;
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

	JCNLOG("finish - load_jcn_strings()");
}
// -------------------------------------------------------------
void
Jcn::create_jcn_db_view()
{
	JCNLOG("start - create_jcn_db_view()");

	for (int i=0; i< JcnDD_Last; ++i)
	{
		_ui.db_View->setColumnWidth(i, JcnDD[i].w);
	}

//	_ui.db_View->setWordWrap(true);

	_tmpSqlDbModel = new JcnSqlModel(_ui.db_View, this);
	_sqlDbModel = new JcnSqlModel(_ui.db_View, this);

	_ui.db_View->setModel(_sqlDbModel);
	_ui.db_View->setSortingEnabled(true);

//@@@	_sqlDbModel->setSortRole(Qt::DisplayRole);

	JCNLOG("finish - create_jcn_db_view()");
}
// -------------------------------------------------------------
void
Jcn::create_find_view()
{
	JCNLOG("start - create_find_view()");

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

	JCNLOG("finish - create_find_view()");
}

// --------------------------------------------------------------------
//   load dadabase
//   when force is set to true use given name without root
// --------------------------------------------------------------------
void
Jcn::load_jcn_db(
					const QByteArray & rootname,
					bool force
				)
{
	_jcnTxtDbFn = rootname + ".txt";
	_jcnSqlDbFn = rootname + ".sql";
	_jcnColDbFn = rootname + ".col";

	JCNLOG("start - load_jcn_db(fname): txt=" + _jcnTxtDbFn + " sql=" + _jcnSqlDbFn);

	QString logText;

	if (! force) // normaly we prepend root path
	{
		_jcnTxtDbFn = _jcnRootDir + "/" + _jcnTxtDbFn;
		_jcnSqlDbFn = _jcnRootDir + "/" + _jcnSqlDbFn;
		_jcnColDbFn = _jcnRootDir + "/" + _jcnColDbFn;
	}


   	QFile file(_jcnTxtDbFn);
   	if(! file.open(QIODevice::ReadOnly | QIODevice::Text))
   	{
		QMessageBox msgBox;
		msgBox.setText("Can't open file: " + _jcnTxtDbFn);
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

	// create sql db or use existing one
	check_sql_db(_jcnSqlDbFn);


	if (jcndbg)
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
		if (jcndbg)
			std::cout << "countRec=" << countRec << std::endl;
   	}

	QProgressDialog progress("Loading JCN SQL Database ...", "OK", 0, countRec, this);

	QByteArray titleText = "JCN Tool 2.1: txt=" + _jcnTxtDbFn;
	titleText += "   sql="+_jcnSqlDbFn;
	setWindowTitle(titleText);

	if (!_sqlDbModel)
		_sqlDbModel = new JcnSqlModel(_ui.db_View, this);

	_sqlDbModel->load();

	JCNLOG("finish - load_jcn_db(): ");
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
	if (jcndbg > 10)
		std::cout << "tab_selected" << std::endl;
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

	load_jcn_db(openfn.toLatin1(), true);

	return;
}
// ------------------------------------------------------------------
void
Jcn::openDb()
{
	QString openfn = QFileDialog::getOpenFileName(this,
     						tr("Open File"), _jcnRootDir, tr("*"));
	if (openfn=="") return;

	load_jcn_db(openfn.toLatin1(), true);

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

	QString backfn = _jcnTxtDbFn+".bak."+QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
	if ( ! QFile::copy (_jcnTxtDbFn, backfn) )
	{
		QMessageBox msgBox;
 		msgBox.setText("Can backup to file: <b>" + backfn + "<b>");
 		msgBox.exec();
		return;
	}

	// open real db file to append
	QFile dbfile(_jcnTxtDbFn);
	if(! dbfile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
	{
   		QMessageBox msgBox;
 		msgBox.setText("Can't open db file: " + _jcnTxtDbFn);
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
/* @@@
		if ( _dbModel->addEntry(calc) ) // good add
		{
			dbfile.write("\r\n" + calc.text() + "\r\n"); // use CR and NL
		}
*/
    	progress.setValue(count++);
        _qapp->processEvents();
	}

//@@@	_ui.db_View->setModel(_dbModel); // reenable it
//@@@	_dbModel->setHeaders();

	impfile.close();
	dbfile.close();

	QMessageBox msgBox;
 	msgBox.setText("Import is completed: Restart QJcn");
 	msgBox.exec();
    _qapp->processEvents();
#ifdef Q_WS_WIN  // if we run under windows
	//Sleep(5);
#else
  sleep(5);
#endif
	exit(0);

	return;
}
// ------------------------------------------------------------------
void
Jcn::exportDb()
{
	QString expfn = _jcnTxtDbFn+"."+QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
	if ( QFile::copy (_jcnTxtDbFn, expfn) )
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
	TextEdit *  ted = new TextEdit(this);
    ted->resize( 700, 800 );

	// get checksum of this file
	FileChkSum fcs(_jcnTxtDbFn);
	unsigned long long csm1 = fcs.calc();

	ted->load(_jcnTxtDbFn);

    ted->show();

	// while we are editing file
	while ( ted->isVisible() )
	{
		QApplication::processEvents(QEventLoop::WaitForMoreEvents);
	}

	// calculate checksum after we exit
	unsigned long long csm2 = fcs.calc();

	delete ted;

	// if checksums are not the same - file has changed
	// we must rebuild sql database
	if (csm1 != csm2)
	{
		std::cout << "File changed: "  << std::endl
				  << " csm1=" << csm1 << std::endl
				  << " csm2=" << csm2
				<< std::endl;

		std::cout << "Rebuild database" << std::endl;


		// must free up resources
		// must close
		delete _sqlDbModel;
		_sqlDbModel = NULL;
	    _db.close();
		QSqlDatabase::removeDatabase("QSQLITE");


		if (! QFile::remove(_jcnSqlDbFn) )
		{
			std::cout << "Can not remove: " << _jcnSqlDbFn << std::endl;
		}

		_db = QSqlDatabase::addDatabase("QSQLITE");

		load_jcn_db();
	}
	else
	{
		std::cout << "File has not changed: "  << std::endl;
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

/*
	generates a set of entries based on what is jcn_strings file and entered data
*/

void
Jcn::generate()
{
	if (jcndbg)
		std::cout << "generate" << std::endl;

	// lets check where we are
	// if we are in translate tab or db tab

	QByteArray workStr;
	QList<QByteArray> qbal;

	if ( _ui.main_TW->currentIndex() == 0) // translate
	{
		workStr = _ui.jcnText_LE->toPlainText().toLatin1().simplified().toUpper();
		if (workStr.length() == 0)
			return;

		qbal.append(workStr);
	}
	else
	if ( _ui.main_TW->currentIndex() == 1) // db tab
	{
		JcnSqlModel * m =	(JcnSqlModel*)_ui.db_View->model();
		QList<QModelIndex> list = _ui.db_View->selectionModel()->selectedRows(0);
		if (list.size() == 0)
		{
			return; // nothing selected
		}

		// only chose one entry in case it is a multi selection
		// safer this way
		for (int j=0; j< list.size(); ++j)
		{
			int itemRow = list.at(j).row();
			QVariant var = m->data(m->index(itemRow, 0, QModelIndex()));

			// extract text from this entry, clean it up and convert
			workStr = var.toString().toLatin1().simplified().toUpper();

			if (workStr.length() != 0)
			{
				qbal.append(workStr);
			}
		}
	}
	else
	{
		return;
	}


   	QFile file(_jcnTxtDbFn);
   	if(! file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append))
   	{
   		QMessageBox msgBox;
		msgBox.setText("Can't open to file: " + _jcnTxtDbFn);
		msgBox.exec();
		return;
	}



	int totCount = _ui.jcnStr_LIST->count() * qbal.size();
	QProgressDialog progress("Generating ...", "Cancel", 0, totCount, this);
	int count=0;
	for (int i=0; i < _ui.jcnStr_LIST->count(); ++i)
	{
		QListWidgetItem * item  = _ui.jcnStr_LIST->item(i);

		for (int j=0; j< qbal.size(); ++j)
		{
			workStr = qbal.at(j);
			QByteArray writeStr = item->text().toLatin1() + " " + workStr;
			writeStr = writeStr.simplified();

			JcnCalc calc(writeStr);

			// lets add to SQLDB
			QSqlQuery query  = _db.exec(calc.toSqlInsert());
			if ( query.lastError().isValid() )
			{
				if (jcndbg)
				{
					std::cout << "QRY Error: " << query.lastError().databaseText() << std::endl;
					std::cout << "  " << writeStr << std::endl;
				}
			}
			else // no error - add to text file as well
			{
				file.write("\r\n" + calc.text() + "\r\n"); // use CR and NL
			}

			count++;
		    progress.setValue(count);
    	    _qapp->processEvents();
			if (progress.wasCanceled()) goto DONE_GEN;
		}
	}

DONE_GEN:

	// reload db
	_sqlDbModel->load();

	file.close();
}

// --------------------------------------------------------------------

/*
	find all 21 matches
*/


void
Jcn::fullMatches()
{
	_ui.main_TW->setCurrentIndex(3);

	if (jcndbg)
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
			rangeLimitSql = " where ( f_hv between " + _ui.rl_f_hv_1_LE->text().toLatin1() + " and " + _ui.rl_f_hv_2_LE->text().toLatin1() + ") ";
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

			rangeLimitSql += " (f_hv between " + _ui.rl_b_hv_1_LE->text().toLatin1() + " and " + _ui.rl_b_hv_2_LE->text().toLatin1() + ") ";
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

			rangeLimitSql += " (f_hv between " + _ui.rl_fb_hv_1_LE->text().toLatin1() + " and " + _ui.rl_fb_hv_2_LE->text().toLatin1() + ") ";
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
	msg = "This is <b>JCN Tool 2.1</b>, " +
		QByteArray(xstr(JCN_BUILD_DATE)) + "<br>" +
		" revision="+QByteArray(xstr(JCN_REV_TAG)) + "<br>";
	msg += "Based on research by Sheree Rainbolt-Kren<br>";
	msg += "Written by Felix Singer";
    QMessageBox::about( this, "About", msg);
}
// --------------------------------------------------------------------
void
Jcn::notesApp()
{
	QTextBrowser * qtb = new QTextBrowser;
	qtb->setHtml(help_html);
	qtb->resize(700,700);
	qtb->setWindowTitle("JCN Tool Guide");
	qtb->show();
}
// --------------------------------------------------------------------
