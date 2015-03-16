#ifndef JcnCalc_h
#define JcnCalc_h

#include <QtGui/QStandardItem>
#include <QtCore/QByteArray>
#include <QtCore/QString>

#include <QtSql/QSqlQuery>

#include "qtutil.h"

const int cl_const = 1000000;

class JcnCalc
{
public:

	JcnCalc();
	JcnCalc(const QString & s, int index=0);
	JcnCalc(const QByteArray & s, int index=0);
	JcnCalc(const JcnCalc & rhs);
	JcnCalc & operator=(const JcnCalc & rhs);

	
	const QByteArray & text() const { return _ws; }
	const int idx() const { return _idx; }

	QByteArray fwa_alpha2roman();		
	QByteArray fwa(bool toRom=false);
	int f_h();
	int f_v();
	int f_hv();
	int f_c();
	int f_p();
	int f_pc();
	int f_cl();

	QByteArray bwa_alpha2roman();
	QByteArray bwa(bool toRom=false);
	int b_h();
	int b_v();
	int b_hv();
	int b_c();
	int b_p();
	int b_pc();
	int b_cl();
	
	int fb_h();
	int fb_v();
	int fb_hv();
	int fb_c();
	int fb_p();
	int fb_pc();	
	int fb_cl();

	QString toText(bool rev=false);
	QString toInfo(bool rev=false);
	QString toDb();
	QString toHtml(bool showF=false, bool showB=false, bool toRom=false);
	QString toCsvLog();
	QString toXml();
	
	// sql interface
	static QByteArray	toSqlCreate();
	static bool 		idxSqlCreate(QSqlDatabase & db);
	
	QByteArray toSqlInsert();
	QByteArray toSqlDelete();
	QByteArray toSqlFullFind();
	void fromSqlQuery(QSqlQuery & qry);

		
	bool	match(JcnCalc & c, int & mask, 
					int matchMask=0);

	bool	match(JcnCalc & c, int & mask, 
					int f_matchMask,
					int b_matchMask,
					int fb_matchMas										
					);

	bool	equal(JcnCalc & c);
	bool 	equal(JcnCalc & c, int f_cnt, int b_cnt, int f_b_cnt);

	int		mask() { return _mask; }
	
private:
	QByteArray		_ws;
	int				_idx;

	int				_f_h;
	int				_f_v;
	int				_f_hv;
	int				_f_c;
	int				_f_p;
	int				_f_pc;
	int				_f_cl;
					
	int				_b_h;
	int				_b_v;
	int				_b_hv;
	int				_b_c;
	int				_b_p;
	int				_b_pc;
	int				_b_cl;

	int				_fb_h;
	int				_fb_v;
	int				_fb_hv;
	int				_fb_c;
	int				_fb_p;
	int				_fb_pc;
	int				_fb_cl;

	int 			_mask;

public:
	QStandardItem * refItem;

	// these override values
	void	f_h(int p) { _f_h = p; }
	void	f_v(int p) { _f_v = p; }
	void	b_h(int p) { _b_h = p; }
	void	b_v(int p) { _b_v = p; }
};

#endif
