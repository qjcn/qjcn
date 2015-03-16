#include <iostream>
#include <QtCore/QStringList>
#include <QtCore/QByteArray>
#include <QtCore/QList>

#include "Express.h"
#include "DD.h"
#include "qtutil.h"
#include "Jcn.h"

#include <bitset>

// --------------------------------------------------------------
Express::Express(const QByteArray & buf, bool exact) :
	_buf(buf),
	_andOp(false)
{
	QList<QByteArray>	qbl;
	qbl = _buf.split(FIELD_DELIM);

	for (int i=0; i < qbl.size(); ++i)
	{
		if (qbl[i].length() <=0) continue;
		
		Pair p(qbl[i], exact);

//		if ( p.isValid() )
			_prs.push_back(p);
	}
}
// --------------------------------------------------------------
void
Express::reset()
{
	for (int i=0; i< _prs.size(); ++i)
	{
		Pair & p = _prs[i];
		p.reset();
	}
}
// --------------------------------------------------------------
bool
Express::isValid()
{
	if (_prs.size() == 0)
		return false;
	else
	{
		for (int i=0; i< _prs.size(); ++i)
		{
			Pair & p = _prs[i];
			if (! p.isValid() )
				return false;
		}
		
		return true;
	}
}
// --------------------------------------------------------------
bool
Express::isMatch(const QByteArray & t, const QByteArray & v)
{
	if (jcndbg > 0)
		std::cout << "  Express::isMatch t v: " << t << " " << v << std::endl;

    std::vector<Pair>::iterator itr;
    for (itr = _prs.begin(); itr != _prs.end(); ++itr)
    {
		Pair & p = *itr;
		if ( p.isMatch(t, v) )  
			return true;
	}
	
	return false;
}
// --------------------------------------------------------------
bool
Express::isMatch(const QByteArray & t, const QByteArray & v, int m, const JcnMatchGrp_Val mg)
{
	if (jcndbg > 0)
		std::cout << "  Express::isMatch t v m: " << t << " " << v << " " << m << std::endl;

    std::vector<Pair>::iterator itr;
    for (itr = _prs.begin(); itr != _prs.end(); ++itr)
    {
		Pair & p = *itr;
		if ( p.isMatch(t, v, m, mg) )
			return true;
	}
	
	return false;
}
// --------------------------------------------------------------
bool	
Express::isMatch()	
{
	if (jcndbg > 0)
		std::cout << "  Express::isMatch" << std::endl;

	if (_andOp)
		return isAndMatch();
	else
		return isOrMatch();
}
// --------------------------------------------------------------
// all pair must have at least one match
// --------------------------------------------------------------
bool	
Express::isAndMatch()	
{
	if (jcndbg > 0)
		std::cout << "  Express::isAndMatch" << std::endl;

	bool ret=true;
    std::vector<Pair>::iterator itr;
    for (itr = _prs.begin(); itr != _prs.end(); ++itr)
    {		
		Pair & p = *itr;
//		ret &= p.matchCount();
		if ( p.matchCount() == 0 )
			return false;
	}

	return true;
}
// --------------------------------------------------------------
// any pair must have at least one match
// --------------------------------------------------------------
bool
Express::isOrMatch()
{
	if (jcndbg > 0)
		std::cout << "  Express::isOrMatch" << std::endl;


    std::vector<Pair>::iterator itr;
    for (itr = _prs.begin(); itr != _prs.end(); ++itr)
    {		
		Pair & p = *itr;
		if ( p.matchCount() > 0 )  
			return true;
	}

	return false;
}
// --------------------------------------------------------------
// --------------------------------------------------------------
bool
check_valid_tag(const QByteArray & t)
{
	if (t=="any") return true;

	for (int i=0; i < JcnDD_Last; ++i)
	{
		if (QByteArray(JcnDD[i].tag) == t)
			return true;
	}

	return false;
}
// --------------------------------------------------------------
bool
check_valid_val(const QByteArray & v)
{
	return true;
/*		
	if ( v.toInt() == 0 )
	{
		// if value is not an int then value can be another tag
		return check_valid_tag(v);
	}	
	else
		return true;
*/
}
// --------------------------------------------------------------
Express::Pair::Pair(const QByteArray & buf, bool exact) :
	_buf(buf),
	_tag(""),
	_val(""),
	_valistag(false),
	_matchCount(0),
	_exact(exact)
{
QByteArray tmp;
	
	int delimidx = _buf.indexOf(KV_DELIM);
	if ( delimidx > 0 )
	{	
		tmp = _buf.left(delimidx).simplified();
		if ( check_valid_tag(tmp) )
			_tag = tmp;

		// exception - text is never exact
		if (_tag == "text" ) _exact=false;
		
		tmp = _buf.right(_buf.length() - delimidx-1).simplified();
		if ( check_valid_tag(tmp) )
			_valistag=true; // value is tag as well

		_val = tmp;
	}	

//std::cout << "tag = [" << _tag << "]" 
//		  << " val = [" << _val << "]" << std::endl;
}
// --------------------------------------------------------------
void
Express::Pair::reset()
{
	_matchCount=0;
}
// --------------------------------------------------------------
bool
Express::Pair::isValid()
{
	if ( _tag != "" && _val !="" )
		return true;
	else
		return false;
}
// --------------------------------------------------------------
bool
Express::Pair::isMatch(const QByteArray & t, const QByteArray & v)
{
	if ( _valistag ) // when value is another tag
	{
		if ( 
			_tag.toUpper() == t.toUpper()
			&&
			v.toUpper() == _val.toUpper()
		    )
		{
			_matchCount++;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{

if (jcndbg > 0)
{
std::cout
		<< "    t: " << _tag.toUpper() << " " << t.toUpper()
		<< " v: " <<  _val.toUpper() << " " << v.toUpper()
		<< std::endl;
}		

		if ( 
			(_tag.toUpper() == t.toUpper() || _tag == "any" )
			&&
			( _exact ?
				v.toUpper() == _val.toUpper() :
				v.toUpper().contains( _val.toUpper() )
			 )
		   )
		{
			_matchCount++;
			return true;
		}
		else
		{
			return false;
		}
	}
}


#define EXP_MM(mm, d) (mm & d ? true : false)
#define EXP_MM2(mm, d1, d2) (mm & d1 && mm & d2? true : false)
#define EXP_MM3(mm, d1, d2, d3) (mm & d1 && mm & d2 && mm & d3 ? true : false)

#define EXP_MATCH_FIELD(key, mm) \
\
	if (jcndbg) \
		std::cout << "Enter EXP_MATCH_FIELD: " << xstr(key) << std::hex << " mm=" << std::bitset<16>(mm) << std::endl; \
\
/* no mod ops - only straigth and reversed */ \
\
if ( v1 == v2 && EXP_MM(mm,JcnMatchAlgo[JcnMacthAlgo_exact].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "  exact: " << std::hex << int(mm) << std::endl; \
} \
\
if ( rev(v1) == v2 && EXP_MM(mm,JcnMatchAlgo[JcnMacthAlgo_rev].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "MATCH_FIELD rev 1: " << " mm=" << int(mm) << std::endl; \
}\
\
if ( v1 == rev(v2) && EXP_MM(mm,JcnMatchAlgo[JcnMacthAlgo_rev].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "MATCH_FIELD rev 2: " << " mm=" << int(mm) << std::endl; \
}\
\
if ( rev(v1) == rev(v2) && EXP_MM(mm,JcnMatchAlgo[JcnMacthAlgo_rev].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "   rev 3: " << std::hex << " mm=" << int(mm) << std::endl; \
}\
\
/* ign0 ops - straight and reversed */ \
\
if ( noz(v1) == noz(v2) && EXP_MM(mm,JcnMatchAlgo[JcnMacthAlgo_ign0].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "MATCH_FIELD ign0 1: " << " mm=" << int(mm) << std::endl; \
}\
\
if ( noz(rev(v1)) == noz(v2) && EXP_MM2(mm,JcnMatchAlgo[JcnMacthAlgo_ign0].mask,JcnMatchAlgo[JcnMacthAlgo_rev].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "MATCH_FIELD ign0 2: " << " mm=" << int(mm) << std::endl; \
}\
\
if ( noz(v1) == noz(rev(v2)) && EXP_MM2(mm,JcnMatchAlgo[JcnMacthAlgo_ign0].mask,JcnMatchAlgo[JcnMacthAlgo_rev].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "MATCH_FIELD ign0 3: " << " mm=" << int(mm) << std::endl; \
}\
\
if ( noz(rev(v1)) == noz(rev(v2)) && EXP_MM2(mm,JcnMatchAlgo[JcnMacthAlgo_ign0].mask,JcnMatchAlgo[JcnMacthAlgo_rev].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "MATCH_FIELD ign0 4: " << " mm=" << int(mm) << std::endl; \
}\
\
/* mult by 2 ops */ \
\
if ( v1*2 == v2 && EXP_MM(mm,JcnMatchAlgo[JcnMacthAlgo_mul2].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "MATCH_FIELD mul2 1: " << " mm=" << int(mm) << std::endl; \
}\
\
if ( v1 == v2*2 && EXP_MM(mm,JcnMatchAlgo[JcnMacthAlgo_mul2].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "MATCH_FIELD mul2 2: " << " mm=" << int(mm) << std::endl; \
}\
\
/* mult by 3 ops */ \
\
if ( v1*3 == v2 && EXP_MM(mm,JcnMatchAlgo[JcnMacthAlgo_mul3].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "MATCH_FIELD mul3 1: " << " mm=" << int(mm) << std::endl; \
}\
\
if ( v1 == v2*3 && EXP_MM(mm,JcnMatchAlgo[JcnMacthAlgo_mul3].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "MATCH_FIELD mul3 2: " << " mm=" << int(mm) << std::endl; \
}\
\
/* plus 1 ops  */ \
\
if ( v1+1 == v2 && EXP_MM(mm,JcnMatchAlgo[JcnMacthAlgo_plus1].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "MATCH_FIELD plus1 1: " << " mm=" << int(mm) << std::endl; \
}\
\
if ( v1 == v2+1 && EXP_MM(mm,JcnMatchAlgo[JcnMacthAlgo_plus1].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "MATCH_FIELD plus1 2: " << " mm=" << int(mm) << std::endl; \
}\
if (jcndbg) \
	std::cout << "Exit  MATCH_FIELD: " << xstr(key) << std::hex << " mm=" << int(mm) << std::endl; \


// --------------------------------------------------------------
// use mask to determin how to match

bool
Express::Pair::isMatch(const QByteArray & t, const QByteArray & v, int m, const JcnMatchGrp_Val mg)
{

if (jcndbg > 0)
{
std::cout
	<< "    t: " << _tag.toUpper() << " " << t.toUpper()
	<< " v: " <<  _val.toUpper() << " " << v.toUpper()
	<< std::endl;
}

	bool ret = false; 
	int mask = 0;	

	if ( _tag.toUpper() == t.toUpper() )
	{		
		int v1 = v.toInt();
		int v2 = _val.toInt();

		switch (mg)
		{
			case JcnMatchGrp_f:
				// fwa matching
				EXP_MATCH_FIELD(f_h, m)
				EXP_MATCH_FIELD(f_v, m)
				EXP_MATCH_FIELD(f_hv, m)
				EXP_MATCH_FIELD(f_c, m)
				EXP_MATCH_FIELD(f_p, m)
				EXP_MATCH_FIELD(f_pc, m)
				EXP_MATCH_FIELD(f_cl, m)
			break;

			case JcnMatchGrp_b:
				// fwa matching
				EXP_MATCH_FIELD(b_h, m)
				EXP_MATCH_FIELD(b_v, m)
				EXP_MATCH_FIELD(b_hv, m)
				EXP_MATCH_FIELD(b_c, m)
				EXP_MATCH_FIELD(b_p, m)
				EXP_MATCH_FIELD(b_pc, m)
				EXP_MATCH_FIELD(b_cl, m)
			break;

			case JcnMatchGrp_fb:
				// fwa matching
				EXP_MATCH_FIELD(fb_h, m)
				EXP_MATCH_FIELD(fb_v, m)
				EXP_MATCH_FIELD(fb_hv, m)
				EXP_MATCH_FIELD(fb_c, m)
				EXP_MATCH_FIELD(fb_p, m)
				EXP_MATCH_FIELD(fb_pc, m)
				EXP_MATCH_FIELD(fb_cl, m)
			break;

			default:
				return false;
			break;
		}
						
		if (ret)
		{
			_matchCount++;
		}
	}

	return ret;
}
// --------------------------------------------------------------
