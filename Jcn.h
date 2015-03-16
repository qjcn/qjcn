#ifndef Jcn_h
#define Jcn_h

#include "ui_JcnForm.h"
#include <QtCore/QFile>
#include <QtCore/QByteArray>
#include <QtCore/QString>

#ifdef WITH_QT5
	#include <QtWidgets/QLabel>
	#include <QtWidgets/QMainWindow>
	#include <QtWidgets/QMenu>
	#include <QtGui/QContextMenuEvent>
#else
	#include <QtGui/QLabel>
	#include <QtGui/QMainWindow>
	#include <QtGui/QMenu>
	#include <QtGui/QContextMenuEvent>
#endif

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>

#include <map>

#include "JcnCalc.h"
#include "JcnModel.h"
#include "JcnSqlModel.h"
#include "qtutil.h"
#include "n2t.h"

#ifndef JCN_BUILD_DATE
#define JCN_BUILD_DATE
#endif

#ifndef JCN_REV_TAG
#define JCN_REV_TAG
#endif


#ifndef JCNLOG
#define JCNLOG(text) \
{ \
	QDateTime qdt = QDateTime::currentDateTime(); \
	log_file.write(QString(text).toLatin1() + "\n"); \
	log_file.flush();\
	std::cout << qdt.toString("yyyyMMdd-hhmmss") << ": " << text  << std::endl;\
}
#endif


typedef std::map<const QByteArray, JcnCalc, cmp_qbar> JcnMap_t;

const QByteArray jcn_str_fname_def = "jcn_strings.txt";
const QByteArray jcn_db_fname_root_def = "jcn_db";
const QByteArray jcn_log_fname = "jcn.log";
const QByteArray jcn_dir_win_def = "c:/qjcn";
const QByteArray jcn_dir_unix_def = "/qjcn"; // this will be appended to home dir exctraced from profile

class Jcn : public QMainWindow
{
    Q_OBJECT

public:
    Jcn(QApplication * a, QMainWindow * parent = 0);

    void 		load_jcn_strings(const QByteArray & fname = jcn_str_fname_def);

	void		dbglog(const QString & s, int lev=0);

	void		check_data();
	void		create_ui();
	void		load_data();

	void		check_sql_db(const QByteArray & fname);
	void		createCtxMenu();

	void		create_jcn_db_view();
	void		create_find_view();
    void		load_jcn_db(const QByteArray & rootname = jcn_db_fname_root_def,
							bool force=false);

	bool		isLineValid(QByteArray & line);

	void		add_to_db();
	int			createMatchMask();
	QByteArray	createExpressMatchFilter(JcnModel * mdl, const QByteArray & flag);


public slots:
  	void		tab_selected(QString s);

	// translate tab
	void		translate_text_changed();
  	void		translate_clicked();
  	void		add_to_db_clicked();
	void		find_clicked();
	void		print_result_clicked();

	// db tab
    void		add_new_to_db_clicked();
    void 		rmv_from_db_clicked();
    void		find_db_clicked();
	void		print_db_clicked();
	void		search_textChanged(const QString & str);

	void		db_filters_clicked();
	void		textFilter_textChanged(const QString & str);
	void		numFilter_textChanged(const QString & str);
	void		expFilter_textChanged(const QString & str);
	void		applyFilterDb_clicked();
	void		resetFilterDb_clicked();

	// find tab
	void		find_calc_text_changed();
    void		find_calc_clicked(bool clearit = true);
    void		sql_find_calc_clicked(bool clearit = true);
	QByteArray	build_sql_query(JcnCalc & c);
    void		find_options_clicked();
    void		find_find_clicked();
    void		find_full_clicked();
	void		print_find_clicked();
	void		rev_find_clicked();
	void		roman_find_clicked();

	void		find_f_GB_clicked(bool f);
	void		find_b_GB_clicked(bool f);
	void 		find_fb_GB_clicked(bool f);

    void		find_op1_clicked();
    void		find_op2_clicked();

	// file menu
	void 		createDb();
	void 		openDb();
	void 		exportDb();
	void 		importDb();
	void 		editDb();
    void 		exitApp();

	// misc menu
	void 		generate();
	void 		fullMatches();

	// help menu
    void 		aboutApp();
    void 		notesApp();

	// context menu
	void 		dbCtxMenuEvent(const QPoint & pos);
	void 		findCtxMenuEvent(const QPoint & pos);

signals:
	void		sig_dbFindCmp(void);
	void		sig_findFindCmp(void);

protected:
    // virtual from QMainWindows
    void closeEvent(QCloseEvent * e);

public:
	Ui::JcnForm 				_ui;
    QApplication *            	_qapp;

private:
	QByteArray					_jcnRootDir; // root dir
	QByteArray					_jcnRootFn;		// jcn db root file file name
	QByteArray					_jcnTxtStrFn;	// jcn strings file name inluding root dir
	QByteArray					_jcnTxtDbFn;	// jcn text db file name inluding root dir
	QByteArray					_jcnSqlDbFn;	// jcn sql db file name inluding root dir
	QByteArray					_jcnColDbFn;	// jcn custom colums file name inluding root dir
	JcnMap_t					_jcnMap;

	JcnModel *					_findModel1;
	JcnModel *					_findModel2;

	// new models
	JcnSqlModel *				_sqlDbModel;
	JcnSqlModel *				_tmpSqlDbModel;


	bool						_revMode;

	JcnModel *					_findModel1_rev;
	JcnModel *					_findModel2_rev;

	QMenu 						_dbCtxMenu;
	QMenu 						_findCtxMenu;
	QMenu 						_fullMatchCtxMenu;

public:
	static bool	sqlFile;

	QSqlDatabase 				_db;
	QFile 						log_file;
};

// --------------------------------------------------------------

#endif
