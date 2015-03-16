#include <iostream>

#ifdef WITH_QT5
	#include <QtWidgets/QProgressDialog>
#else
	#include <QtGui/QProgressDialog>
#endif


#include "JcnModel.h"
#include "DD.h"
#include "Jcn.h"
#include <set>

// ------------------------------------------------------------------
JcnModel::JcnModel(QTableView * tv, Jcn * j) :
	QStandardItemModel(0, JcnDD_Last),
	_tv(tv),
	_jcn(j)
{
	if (jcndbg)
		std::cout << "JcnModel::JcnModel" << std::endl;
		
	setHeaders();
}
// ------------------------------------------------------------------
void	
JcnModel::clearAndReset()
{
	if (jcndbg)
		std::cout << "JcnModel::clearAndReset" << std::endl;

	clear();
	_entryMap.clear();
	setHeaders();
}
// ------------------------------------------------------------------
void
JcnModel::setHeaders()
{	
	if (jcndbg)
		std::cout << "JcnModel::setHeaders" << std::endl;

	setHorizontalHeaderLabels(DDHeader()); // set headers

	for (int i=0; i< JcnDD_Last; ++i) // set width
	{
		_tv->setColumnWidth(i, JcnDD[i].w);
	}
}
// ------------------------------------------------------------------
bool 
JcnModel::findEntry(const QByteArray & s, JcnCalc & c)
{
	if (jcndbg)
		std::cout << "JcnModel::findEntry" << std::endl;

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
JcnModel::hasEntry(const QByteArray & s)
{
	if (jcndbg)
		std::cout << "JcnModel::hasEntry" << std::endl;

	EntryMap_itr_t itr = _entryMap.find(s.simplified().toUpper());
	if ( itr == _entryMap.end())
		return false;
	else
		return true;
}
// ------------------------------------------------------------------
bool
JcnModel::addEntry(const QByteArray & s)
{
	JcnCalc c(s);
	return addEntry(c);
}
// ------------------------------------------------------------------
bool
JcnModel::addEntry(JcnCalc & c)
{
	if (jcndbg)
		std::cout << "JcnModel::addEntry: " << c.text() << std::endl;

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
JcnModel::addEntry(JcnCalc & trg,
					JcnCalc & src,
					int f_matchMask,
					int b_matchMask,
					int fb_matchMask					
					)
{
	if (jcndbg)
		std::cout << "JcnModel::addEntry: " << trg.text() << std::endl;

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
JcnModel::rmvEntry(const JcnCalc & c, int itemRow)
{
	return rmvEntry(c.text(), itemRow);
}
// ------------------------------------------------------------------
bool 
JcnModel::rmvEntry(const QByteArray & s, int itemRow)
{
	if (jcndbg)
		std::cout << "JcnModel::rmvEntry: " << s.simplified().toUpper() << std::endl;

	_entryMap.erase(s.simplified().toUpper());
	takeRow(itemRow);
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
JcnModel::setEntry(JcnCalc & c)
{
	if (jcndbg)
		std::cout << "JcnModel::setEntry" << std::endl;

	int entryRow = rowCount();

//	int rs = 1;
//	if (c.text().length() > 30)
//		rs = c.text().length() / 30; // change row span

	QStandardItem * oneItem = NULL;
	
	SET_ENTRY(text);

	// fwa
	SET_ENTRY(f_h);
	SET_ENTRY(f_v);
	SET_ENTRY(f_hv);						
	SET_ENTRY(f_c);
	SET_ENTRY(f_p);
	SET_ENTRY(f_pc);
	SET_ENTRY(f_cl);
						
	// bwa
	SET_ENTRY(b_h);
	SET_ENTRY(b_v);
	SET_ENTRY(b_hv);						
	SET_ENTRY(b_c);
	SET_ENTRY(b_p);
	SET_ENTRY(b_pc);
	SET_ENTRY(b_cl);

	// fwa+bwa
	SET_ENTRY(fb_h);
	SET_ENTRY(fb_v);
	SET_ENTRY(fb_hv);						
	SET_ENTRY(fb_c);
	SET_ENTRY(fb_p);
	SET_ENTRY(fb_pc);
	SET_ENTRY(fb_cl);

	return true;
}
// ------------------------------------------------------------------
void
JcnModel::markEntry(int r, std::set<int> & markCols)
{
	if (jcndbg)
		std::cout << "JcnModel::markEntry r s" << std::endl;

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
JcnModel::markEntry(int r, int markCol)
{
	if (jcndbg)
		std::cout << "JcnModel::markEntry r c" << std::endl;

	QStandardItem * itm = item(r,markCol);
	QFont f = itm->font();
	f.setBold(true);
	f.setUnderline(true);
	f.setPixelSize(13);
	itm->setFont(f);
}
// ------------------------------------------------------------------
// find matches
// ------------------------------------------------------------------
void	
JcnModel::findAndCompare(JcnCalc & src, std::vector<JcnCalc> & v, int matchMask)
{
	if (jcndbg)
		std::cout << "JcnModel::findAndCompare c v m" << std::endl;

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
JcnModel::findAndCompare(JcnCalc & src, std::vector<JcnCalc> & v, 
						 int f_matchMask,
						 int b_matchMask,
						 int fb_matchMask
						 )
{
	if (jcndbg)
		std::cout << "JcnModel::findAndCompare c v m m m" << std::endl;

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
