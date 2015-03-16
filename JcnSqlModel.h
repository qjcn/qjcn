#ifndef JcnSqlModel_h
#define JcnSqlModel_h


#ifdef WITH_QT5
	#include <QtWidgets/QTableView>
#else
	#include <QtGui/QTableView>
#endif


#include <QtSql/QSqlQueryModel>

#include <map>
#include <set>

#include "JcnCalc.h"
#include "qtutil.h"

typedef std::map <QByteArray, JcnCalc, cmp_qbar>	EntryMap_t;
typedef EntryMap_t::const_iterator 					EntryMap_citr_t;
typedef EntryMap_t::iterator 						EntryMap_itr_t;


class Jcn; // forward ref

class JcnSqlModel : public QSqlQueryModel
{
	Q_OBJECT

public:
	JcnSqlModel(QTableView * tv, Jcn * j);

	// virtual
    QVariant	data(const QModelIndex &item, int role=Qt::DisplayRole) const;

	// --------------------------------------------
	void		load();
	void		clearAndReset();
	void		setHeaders();

	bool		findEntry(const QByteArray & s, JcnCalc & c);

	bool		hasEntry(const QByteArray & s);

	bool		addEntry(const QByteArray & s);
	bool		addEntry(JcnCalc & c);
	bool		setEntry(JcnCalc & c);

	bool		addEntry(JcnCalc & trg,
						JcnCalc & src,
						int f_matchMask,
						int b_matchMask,
						int fb_matchMask
						);
		
	void	markEntry(int r, std::set<int> & markCols);
	void	markEntry(int r, int markCol);
	
	bool	rmvEntry(const QByteArray & s, int itemRow);
	bool	rmvEntry(const JcnCalc & c, int itemRow);
		
	void	findAndCompare(JcnCalc & c, std::vector<JcnCalc> & v, 
							int matchMask=0);

	void	findAndCompare(JcnCalc & c, std::vector<JcnCalc> & v, 
							int f_matchMask,
							int b_matchMask,
							int fb_matchMask						
							);

	EntryMap_t 			_entryMap;
	
private:
	QTableView *	_tv;
	Jcn	*			_jcn;
};

#endif
