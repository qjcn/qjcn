#include <QtGui/QtGui>

#ifdef WITH_QT5
	#include <QtWidgets/QProgressDialog>
#else
	#include <QtGui/QProgressDialog>
#endif


#include "JcnSqlModel.h"
#include "DD.h"
#include "Jcn.h"

// ----------------------------------------------------------------------
JcnSqlModel::JcnSqlModel(QTableView * tv, Jcn * j) :
	QSqlQueryModel(),
	_tv(tv),
	_jcn(j)
{
	setHeaders();

	QFont f = _tv->font();
	f.setPixelSize(10);
	f.setWeight(10);
	_tv->setFont(f);
}

// ----------------------------------------------------------------------
// this is inherited from QSqlQueryModel
// ----------------------------------------------------------------------
QVariant 
JcnSqlModel::data(const QModelIndex & index, int role) const
{
     QVariant value = QSqlQueryModel::data(index, role);
     
	// to manipulate text
	 if (value.isValid() && role == Qt::DisplayRole)
	 {
         switch( index.column() )
		 {
		 	case JcnDD_text: // text
			break;

			case JcnDD_f_h:
			case JcnDD_f_v:
			case JcnDD_f_hv:
			case JcnDD_f_c:
			case JcnDD_f_p:	
			case JcnDD_f_pc:
			case JcnDD_f_cl:
			break;
			
			case JcnDD_b_h:
			case JcnDD_b_v:
			case JcnDD_b_hv:
			case JcnDD_b_c:
			case JcnDD_b_p:
			case JcnDD_b_pc:
			case JcnDD_b_cl:
			break;
			
			case JcnDD_fb_h:
			case JcnDD_fb_v:
			case JcnDD_fb_hv:
			case JcnDD_fb_c:
			case JcnDD_fb_p:
			case JcnDD_fb_pc:
			case JcnDD_fb_cl:
			break;
			
		 } // switch
	} 

	// to manipulate color
	if (role == Qt::TextColorRole)
	{
		switch( index.column() )
		{
		 	case JcnDD_text: // text
		         return qVariantFromValue(QColor(Qt::darkGreen));
			break;

			case JcnDD_f_h:
			case JcnDD_f_v:
			case JcnDD_f_hv:
			case JcnDD_f_c:
			case JcnDD_f_p:	
			case JcnDD_f_pc:
			case JcnDD_f_cl:
		         return qVariantFromValue(QColor(Qt::blue));
			break;
			
			case JcnDD_b_h:
			case JcnDD_b_v:
			case JcnDD_b_hv:
			case JcnDD_b_c:
			case JcnDD_b_p:
			case JcnDD_b_pc:
			case JcnDD_b_cl:
		         return qVariantFromValue(QColor(Qt::red));
			break;
			
			case JcnDD_fb_h:
			case JcnDD_fb_v:
			case JcnDD_fb_hv:
			case JcnDD_fb_c:
			case JcnDD_fb_p:
			case JcnDD_fb_pc:
			case JcnDD_fb_cl:
		         return qVariantFromValue(QColor(Qt::darkMagenta));
			break;
		}
	}
     
	return value;
}
// ------------------------------------------------------------------
void	
JcnSqlModel::load()
{
	if (jcndbg)
		std::cout << "JcnSqlModel::load" << std::endl;

	_tv->setModel(NULL);
	
	clear();
	_entryMap.clear();
	
	setQuery("SELECT * from jcn_table");
	_tv->setModel(this);
	setHeaders();

	while (canFetchMore())
		fetchMore();
	 
//	_tv->scrollToBottom();
//	_tv->scrollToTop();
}

// ------------------------------------------------------------------
void	
JcnSqlModel::clearAndReset()
{
	if (jcndbg)
		std::cout << "JcnSqlModel::clearAndReset" << std::endl;

	clear();
	_entryMap.clear();
	setHeaders();
}
// ------------------------------------------------------------------
void
JcnSqlModel::setHeaders()
{	
	for (int i=0; i< JcnDD_Last; ++i) // set width
	{
		setHeaderData(i, Qt::Horizontal, QObject::tr( JcnDD[i].text));
		_tv->setColumnWidth(i, JcnDD[i].w);
	}
}
// ------------------------------------------------------------------
bool 
JcnSqlModel::findEntry(const QByteArray & s, JcnCalc & c)
{
	if (jcndbg)
		std::cout << "JcnSqlModel::findEntry" << std::endl;

/*
   	EntryMap_itr_t itra;
    for (itra = _entryMap.begin();  itra != _entryMap.end(); ++itra)
    {
		JcnCalc & trg = itra->second;
		std::cout << "(" << trg.text() << ")" << std::endl;
	}
*/

	EntryMap_itr_t itr = _entryMap.find(s.simplified().toUpper());
	if ( itr == _entryMap.end())
		return false;
	else
	{
		c = itr->second;
		return true;
	}
}
// ------------------------------------------------------------------
bool 
JcnSqlModel::hasEntry(const QByteArray & s)
{
	if (jcndbg)
		std::cout << "JcnSqlModel::hasEntry" << std::endl;

	EntryMap_itr_t itr = _entryMap.find(s.simplified().toUpper());
	if ( itr == _entryMap.end())
		return false;
	else
		return true;
}
// ------------------------------------------------------------------
bool
JcnSqlModel::addEntry(const QByteArray & s)
{
	JcnCalc c(s);
	return addEntry(c);
}
// ------------------------------------------------------------------
bool
JcnSqlModel::addEntry(JcnCalc & c)
{
	if (jcndbg)
		std::cout << "JcnSqlModel::addEntry: " << c.text() << std::endl;

	EntryMap_itr_t itr = _entryMap.find(c.text());
	if ( itr == _entryMap.end())
	{
		_entryMap.insert(std::make_pair(c.text(), c));
		return setEntry(c);
	}
	else
	{
		return -1;
	}
}
// ------------------------------------------------------------------
bool
JcnSqlModel::addEntry(JcnCalc & trg,
					JcnCalc & src,
					int f_matchMask,
					int b_matchMask,
					int fb_matchMask					
					)
{
	if (jcndbg)
		std::cout << "JcnSqlModel::addEntry: " << trg.text() << std::endl;

	EntryMap_itr_t itr = _entryMap.find(trg.text());
	if ( itr == _entryMap.end())
	{
		int mask = 0;

		trg.match(src, mask, f_matchMask, b_matchMask, fb_matchMask);

		_entryMap.insert(std::make_pair(trg.text(), trg));
		
		return setEntry(trg);
	}
	else
	{
		return -1;
	}
}
// ------------------------------------------------------------------
bool 
JcnSqlModel::rmvEntry(const JcnCalc & c, int itemRow)
{
	return rmvEntry(c.text(), itemRow);
}
// ------------------------------------------------------------------
bool 
JcnSqlModel::rmvEntry(const QByteArray & s, int itemRow)
{
	if (jcndbg)
		std::cout << "JcnSqlModel::rmvEntry: " << s.simplified().toUpper() << std::endl;

	_entryMap.erase(s.simplified().toUpper());
//@@@	takeRow(itemRow);
	return true;
}
// ------------------------------------------------------------------
// ------------------------------------------------------------------
#define SET_ENTRY(key)\
	{ \
		oneItem = new QStandardItem; \
		oneItem->setData(QVariant(c.key()), Qt::DisplayRole); \
		oneItem->setForeground(QBrush(QColor(JcnDD[JcnDD_ ## key].color))); \
		\
		if ( c.mask() & JcnDD[JcnDD_ ## key].mask ) \
		{ \
			QFont f = oneItem->font(); \
			f.setBold(true); \
			f.setUnderline(true); \
			f.setPixelSize(13); \
			oneItem->setFont(f); \
		} \
		else \
		{ \
			QFont f = oneItem->font(); \
			f.setPixelSize(10); \
			f.setWeight(10); \
			oneItem->setFont(f); \
		} \
		\
		setItem(entryRow, JcnDD_ ## key, oneItem); \
	}

// ------------------------------------------------------------------
bool
JcnSqlModel::setEntry(JcnCalc & c)
{
	if (jcndbg)
		std::cout << "JcnSqlModel::setEntry" << std::endl;

	int entryRow = rowCount();

//	int rs = 1;
//	if (c.text().length() > 30)
//		rs = c.text().length() / 30; // change row span

	QStandardItem * oneItem = NULL;
	
//	SET_ENTRY(text);

	// fwa
//	SET_ENTRY(f_h);
//	SET_ENTRY(f_v);
//	SET_ENTRY(f_hv);						
//	SET_ENTRY(f_c);
//	SET_ENTRY(f_p);
//	SET_ENTRY(f_pc);
//	SET_ENTRY(f_cl);
						
	// bwa
//	SET_ENTRY(b_h);
//	SET_ENTRY(b_v);
//	SET_ENTRY(b_hv);						
//	SET_ENTRY(b_c);
//	SET_ENTRY(b_p);
//	SET_ENTRY(b_pc);
//	SET_ENTRY(b_cl);

	// fwa+bwa
//	SET_ENTRY(fb_h);
//	SET_ENTRY(fb_v);
//	SET_ENTRY(fb_hv);						
//	SET_ENTRY(fb_c);
//	SET_ENTRY(fb_p);
//	SET_ENTRY(fb_pc);
//	SET_ENTRY(fb_cl);

	return true;
}
// ------------------------------------------------------------------
void
JcnSqlModel::markEntry(int r, std::set<int> & markCols)
{
	if (jcndbg)
		std::cout << "JcnSqlModel::markEntry r s" << std::endl;

	for (int c=0; c < JcnDD_Last; ++c)
	{
		if ( markCols.find(c) != markCols.end() )
		{
			markEntry(r, c);
		}
	}
}
// ------------------------------------------------------------------
void
JcnSqlModel::markEntry(int r, int markCol)
{
	if (jcndbg)
		std::cout << "JcnSqlModel::markEntry r c" << std::endl;
/*@@@
	QStandardItem * itm = item(r,markCol);
	QFont f = itm->font();
	f.setBold(true);
	f.setUnderline(true);
	f.setPixelSize(13);
	itm->setFont(f);
*/
}
// ------------------------------------------------------------------
// find matches
// ------------------------------------------------------------------
void	
JcnSqlModel::findAndCompare(JcnCalc & src, std::vector<JcnCalc> & v, int matchMask)
{
	if (jcndbg)
		std::cout << "JcnSqlModel::findAndCompare c v m" << std::endl;

	QProgressDialog progress("Searching ...", "Ok", 0, _entryMap.size(), _jcn);
	int count = 0;
   	EntryMap_itr_t itr;
    for (itr = _entryMap.begin();  itr != _entryMap.end(); ++itr)
    {
		JcnCalc & trg = itr->second;

		if ( src.text().simplified() == trg.text().simplified() ) continue; // skip itself

		int mask = 0;
		if ( trg.match(src, mask, matchMask) )
			v.push_back(trg);

    	progress.setValue(count++);
       _jcn->_qapp->processEvents();
	}
}
// ------------------------------------------------------------------
void	
JcnSqlModel::findAndCompare(JcnCalc & src, std::vector<JcnCalc> & v, 
						 int f_matchMask,
						 int b_matchMask,
						 int fb_matchMask
						 )
{
	if (jcndbg)
		std::cout << "JcnSqlModel::findAndCompare c v m m m" << std::endl;

	QProgressDialog progress("Searching ...", "Ok", 0, _entryMap.size(), _jcn);
	int count = 0;
   	EntryMap_itr_t itr;
    for (itr = _entryMap.begin();  itr != _entryMap.end(); ++itr)
    {
		JcnCalc & trg = itr->second;

		if ( src.text().simplified() == trg.text().simplified() ) continue; // skip itself

		int mask = 0;
		if ( trg.match(src, mask, f_matchMask, b_matchMask, fb_matchMask) )
			v.push_back(trg);

    	progress.setValue(count++);
       _jcn->_qapp->processEvents();
	}
}
// ------------------------------------------------------------------
