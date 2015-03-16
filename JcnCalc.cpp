#include "qtutil.h"
#include "Jcn.h"
#include "JcnCalc.h"
#include "DD.h"

#include <bitset>

QByteArray alpha2roman[26] = 
{
	"I",
	"II",
	"III",
	"IV",
	"V",
	"VI",
	"VII",
	"VIII",
	"IX",
	"X",
	"XI",
	"XII",
	"XIII",
	"XIV",
	"XV",
	"XVI",
	"XVII",
	"XVIII",
	"XIX",
	"XX",
	"XXI",
	"XXII",
	"XXIII",
	"XXIV",
	"XXV",
	"XXVI"
};

#define FC2I(ch)	(ch-'A'+1)
#define BC2I(ch)	('Z'-ch +1)

#define FA2R(ch)	(alpha2roman[ch-'A'])
#define BA2R(ch)	(alpha2roman['Z'-ch])

// ----------------------------------------------------------
JcnCalc::JcnCalc() :

	_ws(""),

	_idx(0),
	
	_f_h(0),
	_f_v(0),
	_f_hv(0),
	_f_c(0),
	_f_p(0),
	_f_pc(0),
	_f_cl(0),
					
	_b_h(0),
	_b_v(0),
	_b_hv(0),
	_b_c(0),
	_b_p(0),
	_b_pc(0),
	_b_cl(0),

	_fb_h(0),
	_fb_v(0),
	_fb_hv(0),
	_fb_c(0),
	_fb_p(0),
	_fb_pc(0),
	_fb_cl(0),
	
	_mask(0),
	
	refItem(NULL)
{
}

// ----------------------------------------------------------
JcnCalc::JcnCalc(const QString & s, int index) :

	_ws(s.simplified().toUpper().toLatin1()),

	_idx(index),

	_f_h(0),
	_f_v(0),
	_f_hv(0),
	_f_c(0),
	_f_p(0),
	_f_pc(0),
	_f_cl(0),
					
	_b_h(0),
	_b_v(0),
	_b_hv(0),
	_b_c(0),
	_b_p(0),
	_b_pc(0),
	_b_cl(0),

	_fb_h(0),
	_fb_v(0),
	_fb_hv(0),
	_fb_c(0),
	_fb_p(0),
	_fb_pc(0),
	_fb_cl(0),
	
	_mask(0),
	
	refItem(NULL)
{
}

// ----------------------------------------------------------
JcnCalc::JcnCalc(const QByteArray & s, int index) :

	_ws(s.simplified().toUpper()),

	_idx(index),

	_f_h(0),
	_f_v(0),
	_f_hv(0),
	_f_c(0),
	_f_p(0),
	_f_pc(0),
	_f_cl(0),
					
	_b_h(0),
	_b_v(0),
	_b_hv(0),
	_b_c(0),
	_b_p(0),
	_b_pc(0),
	_b_cl(0),

	_fb_h(0),
	_fb_v(0),
	_fb_hv(0),
	_fb_c(0),
	_fb_p(0),
	_fb_pc(0),
	_fb_cl(0),
	
	_mask(0),

	refItem(NULL)
{
}

// ----------------------------------------------------------
JcnCalc::JcnCalc(const JcnCalc & rhs)
{
	_ws 	= rhs._ws;

	_idx 	= rhs._idx;

	_f_h	= rhs._f_h;
	_f_v	= rhs._f_v;
	_f_hv	= rhs._f_hv;
	_f_c	= rhs._f_c;
	_f_p	= rhs._f_p;
	_f_pc	= rhs._f_pc;
	_f_cl	= rhs._f_cl;
					
	_b_h	= rhs._b_h;			
	_b_v	= rhs._b_v;
	_b_hv	= rhs._b_hv;
	_b_c	= rhs._b_c;
	_b_p	= rhs._b_p;
	_b_pc	= rhs._b_pc;
	_b_cl	= rhs._b_cl;
	
	_fb_h	= rhs._fb_h;			
	_fb_v	= rhs._fb_v;
	_fb_hv	= rhs._fb_hv;
	_fb_c	= rhs._fb_c;
	_fb_p	= rhs._fb_p;
	_fb_pc	= rhs._fb_pc;
	_fb_cl	= rhs._fb_cl;
	
	_mask	= rhs._mask;
	
	refItem = rhs.refItem;
}
// ----------------------------------------------------------
JcnCalc &
JcnCalc::operator=(const JcnCalc & rhs)
{
    if (this == &rhs)
        return *this;

	_ws 	= rhs._ws;

	_idx 	= rhs._idx;
	
	_f_h	= rhs._f_h;
	_f_v	= rhs._f_v;
	_f_hv	= rhs._f_hv;
	_f_c	= rhs._f_c;
	_f_p	= rhs._f_p;
	_f_pc	= rhs._f_pc;
	_f_cl	= rhs._f_cl;
					
	_b_h	= rhs._b_h;			
	_b_v	= rhs._b_v;
	_b_hv	= rhs._b_hv;
	_b_c	= rhs._b_c;
	_b_p	= rhs._b_p;
	_b_pc	= rhs._b_pc;
	_b_cl	= rhs._b_cl;
	
	_fb_h	= rhs._fb_h;			
	_fb_v	= rhs._fb_v;
	_fb_hv	= rhs._fb_hv;
	_fb_c	= rhs._fb_c;
	_fb_p	= rhs._fb_p;
	_fb_pc	= rhs._fb_pc;
	_fb_cl	= rhs._fb_cl;

	_mask	= rhs._mask;
	
	refItem = rhs.refItem;
			
	return *this;
}

// ----------------------------------------------------------


// ----------------------------------------------------------
// ----------------------------------------------------------
// FWA calculations
// ----------------------------------------------------------
// ----------------------------------------------------------

QByteArray
JcnCalc::fwa_alpha2roman()
{
QByteArray ret;

	for (int i=0; i < _ws.length(); ++i)
	{
		char c = _ws.at(i);
		if (c <'A' || c > 'Z') 
		{
			ret += " ";
			continue; // skip junk
		}

		ret += FA2R(c);
		ret += " ";
	}
	return ret;
}
// ----------------------------------------------------------
QByteArray
JcnCalc::fwa(bool toRom)
{
	if (toRom)
		return fwa_alpha2roman();

	QByteArray ret;

	for (int i=0; i < _ws.length(); ++i)
	{
		char c = _ws.at(i);
		if (c <'A' || c > 'Z') 
		{
			ret += " ";
			continue; // skip junk
		}

		ret += QByteArray::number(FC2I(c));
		ret += " ";
	}
	return ret;
}
// ----------------------------------------------------------
// --------------------------- f ----------------------------
// ----------------------------------------------------------
// h - number
int
JcnCalc::f_h()
{
	if (_f_h == 0)
	{
	   for (int i=0; i < _ws.length(); ++i)
	   {
		   char c = _ws.at(i);
		   if (c <'A' || c > 'Z') 
		   {
			   continue; // skip junk
		   }

		   int n = FC2I(c);
		   _f_h += n/10 + n%10;
	   }
	}
	
	return _f_h;
}
// ----------------------------------------------------------
// v - number
int
JcnCalc::f_v()
{
	if (_f_v == 0)
	{
		for (int i=0; i < _ws.length(); ++i)
		{
			char c = _ws.at(i);
			if (c <'A' || c > 'Z') 
			{
				continue; // skip junk
			}

			int n = FC2I(c);		
			_f_v += n;
		}
	}

	return _f_v;
}
// ----------------------------------------------------------
// hv - number
int
JcnCalc::f_hv()
{
	if (_f_hv == 0)
	{
		_f_hv =  f_h() + f_v();
	}
	
	return _f_hv;
}
// ----------------------------------------------------------
// c - number
int
JcnCalc::f_c()
{
	if ( _f_c == 0 && f_hv() > 0 )
	{
		_f_c = f_hv() + 360;
	}
	
	return _f_c;
}
// ----------------------------------------------------------
// p - number
int
JcnCalc::f_p()
{
	if (_f_p == 0)
	{
		_f_p = f_hv() * 3;
	}
	
	return _f_p;
}
// ----------------------------------------------------------
// pc - number
int
JcnCalc::f_pc()
{
	if (_f_pc == 0)
	{
		_f_pc = f_c() + f_p();
	}
	
	return _f_pc;
}
// ----------------------------------------------------------
// support function for cl number
int
_cl_number(int h, int v, int hv)
{
	int ret = 0;
	
	int a = cl_const + h;
	int b = cl_const + v;
	int c = cl_const + hv;
	
	QByteArray c_ar[3];
	c_ar[0] = QByteArray::number(a);	
	c_ar[1] = QByteArray::number(b);
	c_ar[2] = QByteArray::number(c);
	
	QByteArray l_s = QByteArray::number(cl_const);
	
	QByteArray ar_ret[6];
	for (int c=1; c<7; ++c) // cols - ignore first - 1s
	{	
		QByteArray cl_str="";
		for (int r=0; r<3; ++r)  // rows
		{
			// skip 0 in first or last row
			if ( c_ar[r].at(c) == '0' && (r==0 || r==2))
					continue;
						
			cl_str += c_ar[r].at(c);
		}
		ret += cl_str.toInt();
	}
	
	return ret;
}

// ----------------------------------------------------------
// cl number
int
JcnCalc::f_cl()
{
	if (_f_cl == 0)
	{
		_f_cl = _cl_number( f_h(), f_v(), f_hv() );
	}
	
	return _f_cl;
}

// ----------------------------------------------------------
// ----------------------------------------------------------
// BWA calculations
// ----------------------------------------------------------
// ----------------------------------------------------------


QByteArray
JcnCalc::bwa_alpha2roman()
{
QByteArray ret;

	for (int i=0; i < _ws.length(); ++i)
	{
		char c = _ws.at(i);
		if (c <'A' || c > 'Z') 
		{
			ret += " ";
			continue; // skip junk
		}

		ret += BA2R(c);
		ret += " ";
	}
	return ret;
}
// ----------------------------------------------------------
QByteArray
JcnCalc::bwa(bool toRom)
{
	if (toRom)
		return bwa_alpha2roman();
		
	QByteArray ret;
	for (int i=0; i < _ws.length(); ++i)
	{
		char c = _ws.at(i);
		if (c <'A' || c > 'Z') 
		{
			ret += " ";
			continue; // skip junk
		}

		ret += QByteArray::number(BC2I(c));
		ret += " ";
	}
	return ret;
}
// ----------------------------------------------------------
// --------------------------- b ----------------------------
// ----------------------------------------------------------
// h number
int
JcnCalc::b_h()
{
	if( _b_h == 0)
	{
		for (int i=0; i < _ws.length(); ++i)
		{
			char c = _ws.at(i);
			if (c <'A' || c > 'Z') 
			{
				continue; // skip junk
			}

			int n = BC2I(c);
			_b_h += n/10 + n%10;
		}
	}
	return _b_h;
}
// ----------------------------------------------------------
// v number
int
JcnCalc::b_v()
{
	if( _b_v == 0)
	{
		for (int i=0; i < _ws.length(); ++i)
		{
			char c = _ws.at(i);
			if (c <'A' || c > 'Z') 
			{
				continue; // skip junk
			}

			int n = BC2I(c);		
			_b_v += n;
		}
	}
	return _b_v;
}
// ----------------------------------------------------------
// hv number
int
JcnCalc::b_hv()
{
	if (_b_hv == 0)
	{
		_b_hv =  b_h() + b_v();
	}
	
	return _b_hv;
}
// ----------------------------------------------------------
// c number
int
JcnCalc::b_c()
{
	if (_b_c == 0 && b_hv() > 0)
	{
		_b_c = b_hv() + 360;
	}
	
	return _b_c;
}
// ----------------------------------------------------------
// p number
int
JcnCalc::b_p()
{
	if (_b_p == 0)
	{
		_b_p = b_hv() * 3;
	}
	
	return _b_p;
}
// ----------------------------------------------------------
// pc number
int
JcnCalc::b_pc()
{
	if (_b_pc == 0)
	{
		_b_pc = b_c() + b_p();
	}
	
	return _b_pc;
}
// ----------------------------------------------------------
// cl number
int
JcnCalc::b_cl()
{
	if (_b_cl == 0)
	{
		_b_cl = _cl_number( b_h(), b_v(), b_hv() );
	}
	
	return _b_cl;
}


// ----------------------------------------------------------
// --------------------------- fb ---------------------------
// ----------------------------------------------------------

// h number
int 
JcnCalc::fb_h()
{
	if (_fb_h == 0)
	{
		_fb_h = f_h() + b_h();
	}
	
	return _fb_h;
}

// ----------------------------------------------------------
// v number
int 
JcnCalc::fb_v()
{
	if (_fb_v == 0)
	{
		_fb_v = f_v() + b_v();
	}
	
	return _fb_v;
}

// ----------------------------------------------------------
// fb number
int 
JcnCalc::fb_hv()
{
	if (_fb_hv == 0)
	{
		_fb_hv = f_hv() + b_hv();
	}
	
	return _fb_hv;
}
// ----------------------------------------------------------
// c number
int 
JcnCalc::fb_c()
{
	if (_fb_c == 0)
	{
		// 2011-01-28 change to sum of f and b      _fb_c = fb_hv() + 360;
		_fb_c = f_c() + b_c();
	}
	
	return _fb_c;
}
// ----------------------------------------------------------
// p number
int 
JcnCalc::fb_p()
{
	if (_fb_p == 0)
	{
		// 2011-01-28 change to sum of f and b      _fb_p = fb_hv() * 3;
		_fb_p = f_p() + b_p();
	}
	
	return _fb_p;
}
// ----------------------------------------------------------
// pc number
int 
JcnCalc::fb_pc()
{
	if (_fb_pc == 0)
	{
		// 2011-01-28 change to sum of f and b       _fb_pc = fb_c() + fb_p();
		_fb_pc = f_pc() + b_pc();
	}
	
	return _fb_pc;
}
// ----------------------------------------------------------
// cl number
int
JcnCalc::fb_cl()
{
	if (_fb_cl == 0)
	{	
		// 2011-01-28 change to sum of f and b        _fb_cl = _cl_number( fb_h(), fb_v(), fb_hv() );
		_fb_cl = f_cl() + b_cl();
	}
	
	return _fb_cl;
}

// ----------------------------------------------------------
// ----------------------------------------------------------
QString 
JcnCalc::toText(bool rev)
{
QString ret;

	ret = "<b>Text: " + _ws + "</b>" + (rev ? " <i>(rev)</i>" : "" ) + "<br>";

	if (rev)
	{
		ret += "<b>FWA:</b> ";
		ret += rev_qbar(QByteArray::number(f_h())) + "h ";
		ret += rev_qbar(QByteArray::number(f_v())) + "v ";
		ret += rev_qbar(QByteArray::number(f_hv())) + "hv ";
		ret += rev_qbar(QByteArray::number(f_c())) + "c# ";
		ret += rev_qbar(QByteArray::number(f_p())) + "p# ";
		ret += rev_qbar(QByteArray::number(f_pc())) + "pc# ";
		ret += rev_qbar(QByteArray::number(f_cl())) + "cl# ";
		ret += "<br>";

		ret += "<b>BWA:</b> ";
		ret += rev_qbar(QByteArray::number(b_h())) + "h ";
		ret += rev_qbar(QByteArray::number(b_v())) + "v ";
		ret += rev_qbar(QByteArray::number(b_hv())) + "hv ";
		ret += rev_qbar(QByteArray::number(b_c())) + "c# ";
		ret += rev_qbar(QByteArray::number(b_p())) + "p# ";
		ret += rev_qbar(QByteArray::number(b_pc())) + "pc# ";
		ret += rev_qbar(QByteArray::number(b_cl())) + "cl# ";
		ret += "<br>";

		ret += "<b>FWA+BWA:</b> ";
		ret += rev_qbar(QByteArray::number(fb_h())) + "h ";
		ret += rev_qbar(QByteArray::number(fb_v())) + "v ";
		ret += rev_qbar(QByteArray::number(fb_hv())) + "hv ";
		ret += rev_qbar(QByteArray::number(fb_c())) + "c# ";
		ret += rev_qbar(QByteArray::number(fb_p())) + "p# ";
		ret += rev_qbar(QByteArray::number(fb_pc())) + "pc# ";
		ret += rev_qbar(QByteArray::number(fb_cl())) + "cl# ";
		ret += "<br>";
	}
	else
	{
		ret += "<b>FWA:</b> ";
		ret += QByteArray::number(f_h()) + "h ";
		ret += QByteArray::number(f_v()) + "v ";
		ret += QByteArray::number(f_hv()) + "hv ";
		ret += QByteArray::number(f_c()) + "c# ";
		ret += QByteArray::number(f_p()) + "p# ";
		ret += QByteArray::number(f_pc()) + "pc# ";
		ret += QByteArray::number(f_cl()) + "cl# ";
		ret += "<br>";

		ret += "<b>BWA:</b> ";
		ret += QByteArray::number(b_h()) + "h ";
		ret += QByteArray::number(b_v()) + "v ";
		ret += QByteArray::number(b_hv()) + "hv ";
		ret += QByteArray::number(b_c()) + "c# ";
		ret += QByteArray::number(b_p()) + "p# ";
		ret += QByteArray::number(b_pc()) + "pc# ";
		ret += QByteArray::number(b_cl()) + "cl# ";
		ret += "<br>";

		ret += "<b>FWA+BWA:</b> ";
		ret += QByteArray::number(fb_h()) + "h ";
		ret += QByteArray::number(fb_v()) + "v ";
		ret += QByteArray::number(fb_hv()) + "hv ";
		ret += QByteArray::number(fb_c()) + "c# ";
		ret += QByteArray::number(fb_p()) + "p# ";
		ret += QByteArray::number(fb_pc()) + "pc# ";
		ret += QByteArray::number(fb_cl()) + "cl# ";
		ret += "<br>";
	}
	
	return ret;		
}
// ----------------------------------------------------------
QString 
JcnCalc::toInfo(bool rev)
{
QString ret;

	ret = "Text: " + _ws;

	if (rev)
	{
		ret += " FWA: ";
		ret += rev_qbar(QByteArray::number(f_h())) + "h ";
		ret += rev_qbar(QByteArray::number(f_v())) + "v ";
		ret += rev_qbar(QByteArray::number(f_hv())) + "hv ";
		ret += rev_qbar(QByteArray::number(f_c())) + "c# ";
		ret += rev_qbar(QByteArray::number(f_p())) + "p# ";
		ret += rev_qbar(QByteArray::number(f_pc())) + "pc# ";
		ret += rev_qbar(QByteArray::number(f_cl())) + "cl# ";

		ret += " BWA: ";
		ret += rev_qbar(QByteArray::number(b_h())) + "h ";
		ret += rev_qbar(QByteArray::number(b_v())) + "v ";
		ret += rev_qbar(QByteArray::number(b_hv())) + "hv ";
		ret += rev_qbar(QByteArray::number(b_c())) + "c# ";
		ret += rev_qbar(QByteArray::number(b_p())) + "p# ";
		ret += rev_qbar(QByteArray::number(b_pc())) + "pc# ";
		ret += rev_qbar(QByteArray::number(b_cl())) + "cl# ";

		ret += " FWA+BWA: ";
		ret += rev_qbar(QByteArray::number(fb_h())) + "h ";
		ret += rev_qbar(QByteArray::number(fb_v())) + "v ";
		ret += rev_qbar(QByteArray::number(fb_hv())) + "hv ";
		ret += rev_qbar(QByteArray::number(fb_c())) + "c# ";
		ret += rev_qbar(QByteArray::number(fb_p())) + "p# ";
		ret += rev_qbar(QByteArray::number(fb_pc())) + "pc# ";
		ret += rev_qbar(QByteArray::number(fb_cl())) + "cl# ";
	}
	else
	{
		ret += " FWA: ";
		ret += QByteArray::number(f_h()) + "h ";
		ret += QByteArray::number(f_v()) + "v ";
		ret += QByteArray::number(f_hv()) + "hv ";
		ret += QByteArray::number(f_c()) + "c# ";
		ret += QByteArray::number(f_p()) + "p# ";
		ret += QByteArray::number(f_pc()) + "pc# ";
		ret += QByteArray::number(f_cl()) + "cl# ";

		ret += " BWA: ";
		ret += QByteArray::number(b_h()) + "h ";
		ret += QByteArray::number(b_v()) + "v ";
		ret += QByteArray::number(b_hv()) + "hv ";
		ret += QByteArray::number(b_c()) + "c# ";
		ret += QByteArray::number(b_p()) + "p# ";
		ret += QByteArray::number(b_pc()) + "pc# ";
		ret += QByteArray::number(b_cl()) + "cl# ";

		ret += " FWA+BWA: ";
		ret += QByteArray::number(fb_h()) + "h ";
		ret += QByteArray::number(fb_v()) + "v ";
		ret += QByteArray::number(fb_hv()) + "hv ";
		ret += QByteArray::number(fb_c()) + "c# ";
		ret += QByteArray::number(fb_p()) + "p# ";
		ret += QByteArray::number(fb_pc()) + "pc# ";
		ret += QByteArray::number(fb_cl()) + "cl# ";
	}
	
	return ret;		
}
// ----------------------------------------------------------
QString 
JcnCalc::toDb()
{
QString ret;

	ret = "\r\n" + _ws + "\r\n";
	return ret;
}
// ----------------------------------------------------------
QString 
JcnCalc::toHtml(bool showF, bool showB, bool toRom)
{
QString ret;
	
	ret = "Entered Text: <b>" + _ws + "</b><br><br>";

	ret += "<b>FWA:</b><br>";    

	if (showF)
		ret += "<span style=\"color: rgb(0, 0, 200);\">" +
				fwa(toRom) +
				"</span><br>";

//		ret += "<font size=-2><b>" + fwa(toRom) + "</b></font size=+2><br>";
		
	ret += QByteArray::number(f_h()) + " h<br>";
	ret += QByteArray::number(f_v()) + " v<br>";
	ret += QByteArray::number(f_hv()) + " hv<br>";
	ret += QByteArray::number(f_c()) + " c#<br>";
	ret += QByteArray::number(f_p()) + " p#<br>";
	ret += QByteArray::number(f_pc()) + " pc#<br>";
	ret += QByteArray::number(f_cl()) + " cl#<br>";
	
	ret += "<br>";
	
	ret += "<b>BWA:</b><br>";

	if (showB)
		ret += "<span style=\"color: rgb(0, 200, 0);\">" +
				bwa(toRom) +
				"</span><br>";

//		ret += "<font size=-2><b>" + bwa(toRom) + "</b></font size=+2><br>";
		
	ret += QByteArray::number(b_h()) + " h<br>";
	ret += QByteArray::number(b_v()) + " v<br>";
	ret += QByteArray::number(b_hv()) + " hv<br>";
	ret += QByteArray::number(b_c()) + " c#<br>";
	ret += QByteArray::number(b_p()) + " p#<br>";
	ret += QByteArray::number(b_pc()) + " pc#<br>";
	ret += QByteArray::number(b_cl()) + " cl#<br>";
	
	ret += "<br>";
	
	ret += "<b>FWA+BWA:</b><br>";
	ret += QByteArray::number(fb_h()) + " h<br>";
	ret += QByteArray::number(fb_v()) + " v<br>";
	ret += QByteArray::number(fb_hv()) + " hv<br>";
	ret += QByteArray::number(fb_c()) + " c#<br>";
	ret += QByteArray::number(fb_p()) + " p#<br>";
	ret += QByteArray::number(fb_pc()) + " pc#<br>";
	ret += QByteArray::number(fb_cl()) + " cl#<br>";
	
	return ret;
}
// ----------------------------------------------------------
QString 
JcnCalc::toCsvLog()
{
	char wstr[1024];
	sprintf(wstr, "%3d,%3d,%3d,%3d,%3d,%3d,%3d|%3d,%3d,%3d,%3d,%3d,%3d,%3d|%3d,%3d,%3d,%3d,%3d,%3d,%3d",
			f_h(),
			f_v(), 
			f_hv(),
			f_c(),
			f_p(), 
			f_pc(),
			f_cl(),
			
			b_h(),
			b_v(),
			b_hv(),
			b_c(),
			b_p(),
			b_pc(),
			b_cl(),

			fb_h(),
			fb_v(),
			fb_hv(),
			fb_c(),
			fb_p(),
			fb_pc(),
			fb_cl()
			);
		
	return wstr;
}
// ----------------------------------------------------------
QString 
JcnCalc::toXml()
{
QString ret;
		
	return ret;
}

#define SQLTABLE QByteArray("jcn_table")

// ----------------------------------------------------------
QByteArray
JcnCalc::toSqlCreate()
{
	QByteArray ret;

	ret = "CREATE TABLE " + SQLTABLE;
	ret += " (";

	ret += " jcntext TEXT, ";

	ret += " f_h   NUMERIC, ";
	ret += " f_v   NUMERIC, ";
	ret += " f_hv  NUMERIC, ";
	ret += " f_c   NUMERIC, ";
	ret += " f_p   NUMERIC, ";
	ret += " f_pc  NUMERIC, ";
	ret += " f_cl  NUMERIC, ";

	ret += " b_h   NUMERIC, ";
	ret += " b_v   NUMERIC, ";
	ret += " b_hv  NUMERIC, ";
	ret += " b_c   NUMERIC, ";
	ret += " b_p   NUMERIC, ";
	ret += " b_pc  NUMERIC, ";
	ret += " b_cl  NUMERIC, ";

	ret += " fb_h  NUMERIC, ";
	ret += " fb_v  NUMERIC, ";
	ret += " fb_hv NUMERIC, ";
	ret += " fb_c  NUMERIC, ";
	ret += " fb_p  NUMERIC, ";
	ret += " fb_pc NUMERIC, ";
	ret += " fb_cl NUMERIC,  ";

	ret += " CONSTRAINT pk PRIMARY KEY (jcntext, f_h, f_v, f_hv, f_c, f_p, f_pc, f_cl,  b_h, b_v, b_hv, b_c, b_p, b_pc, b_cl,  fb_h, fb_v, fb_hv, fb_c, fb_p, fb_pc, fb_cl)";

	ret += "); ";

	return ret;
}
// ----------------------------------------------------------
bool
JcnCalc::idxSqlCreate(QSqlDatabase & db)
{
	bool ret;
	QSqlQuery qry;
	QByteArray sql;
	

	// add indexes
	sql = " CREATE INDEX jcntext_idx ON " + SQLTABLE + " (jcntext); ";
	qry  = db.exec(sql);
	if ( qry.lastError().isValid() )	{	return false; }
	
	// -- f
	sql = " CREATE INDEX f_h_idx ON " + SQLTABLE + " (f_h); ";
	qry  = db.exec(sql);
	if ( qry.lastError().isValid() )	{	return false; }

	sql = " CREATE INDEX f_v_idx ON " + SQLTABLE + " (f_v); ";
	qry  = db.exec(sql);
	if ( qry.lastError().isValid() )	{	return false; }

	sql = " CREATE INDEX f_hv_idx ON " + SQLTABLE + " (f_hv); ";
	qry  = db.exec(sql);
	if ( qry.lastError().isValid() )	{	return false; }

	sql = " CREATE INDEX f_c_idx ON " + SQLTABLE + " (f_c); ";
	qry  = db.exec(sql);
	if ( qry.lastError().isValid() )	{	return false; }

	sql = " CREATE INDEX f_p_idx ON " + SQLTABLE + " (f_p); ";
	qry  = db.exec(sql);
	if ( qry.lastError().isValid() )	{	return false; }

	sql = " CREATE INDEX f_pc_idx ON " + SQLTABLE + " (f_pc); ";
	qry  = db.exec(sql);
	if ( qry.lastError().isValid() )	{	return false; }

	sql = " CREATE INDEX f_cl_idx ON " + SQLTABLE + " (f_cl); ";
	qry  = db.exec(sql);
	if ( qry.lastError().isValid() )	{	return false; }


	// -- b
	sql = " CREATE INDEX b_h_idx ON " + SQLTABLE + " (b_h); ";
	qry  = db.exec(sql);
	if ( qry.lastError().isValid() )	{	return false; }

	sql = " CREATE INDEX b_v_idx ON " + SQLTABLE + " (b_v); ";
	qry  = db.exec(sql);
	if ( qry.lastError().isValid() )	{	return false; }

	sql = " CREATE INDEX b_hv_idx ON " + SQLTABLE + " (b_hv); ";
	qry  = db.exec(sql);
	if ( qry.lastError().isValid() )	{	return false; }

	sql = " CREATE INDEX b_c_idx ON " + SQLTABLE + " (b_c); ";
	qry  = db.exec(sql);
	if ( qry.lastError().isValid() )	{	return false; }

	sql = " CREATE INDEX b_p_idx ON " + SQLTABLE + " (b_p); ";
	qry  = db.exec(sql);
	if ( qry.lastError().isValid() )	{	return false; }

	sql = " CREATE INDEX b_pc_idx ON " + SQLTABLE + " (b_pc); ";
	qry  = db.exec(sql);
	if ( qry.lastError().isValid() )	{	return false; }

	sql = " CREATE INDEX b_cl_idx ON " + SQLTABLE + " (b_cl); ";
	qry  = db.exec(sql);
	if ( qry.lastError().isValid() )	{	return false; }


	// -- fb
	sql = " CREATE INDEX fb_h_idx ON " + SQLTABLE + " (fb_h); ";
	qry  = db.exec(sql);
	if ( qry.lastError().isValid() )	{	return false; }

	sql = " CREATE INDEX fb_v_idx ON " + SQLTABLE + " (fb_v); ";
	qry  = db.exec(sql);
	if ( qry.lastError().isValid() )	{	return false; }

	sql = " CREATE INDEX fb_hv_idx ON " + SQLTABLE + " (fb_hv); ";
	qry  = db.exec(sql);
	if ( qry.lastError().isValid() )	{	return false; }

	sql = " CREATE INDEX fb_c_idx ON " + SQLTABLE + " (fb_c); ";
	qry  = db.exec(sql);
	if ( qry.lastError().isValid() )	{	return false; }

	sql = " CREATE INDEX fb_p_idx ON " + SQLTABLE + " (fb_p); ";
	qry  = db.exec(sql);
	if ( qry.lastError().isValid() )	{	return false; }

	sql = " CREATE INDEX fb_pc_idx ON " + SQLTABLE + " (fb_pc); ";
	qry  = db.exec(sql);
	if ( qry.lastError().isValid() )	{	return false; }

	sql = " CREATE INDEX fb_cl_idx ON " + SQLTABLE + " (fb_cl); ";
	qry  = db.exec(sql);
	if ( qry.lastError().isValid() )	{	return false; }

	return true;
}
// --------------------------------------------------------------------	
QByteArray
JcnCalc::toSqlInsert()
{
	QByteArray ret;
	ret = "INSERT INTO " + SQLTABLE;

	ret += " (";

	ret += " jcntext, ";

	ret += " f_h , ";
	ret += " f_v , ";
	ret += " f_hv, ";
	ret += " f_c , ";
	ret += " f_p , ";
	ret += " f_pc, ";
	ret += " f_cl, ";

	ret += " b_h , ";
	ret += " b_v , ";
	ret += " b_hv, ";
	ret += " b_c , ";
	ret += " b_p , ";
	ret += " b_pc, ";
	ret += " b_cl, ";

	ret += " fb_h, ";
	ret += " fb_v, ";
	ret += " fb_hv,";
	ret += " fb_c, ";
	ret += " fb_p, ";
	ret += " fb_pc,";
	ret += " fb_cl ";
	ret += ")";

	ret += " VALUES (";
	ret += '"' + _ws + "\",";
	
	ret += QByteArray::number(f_h()) + ",";
	ret += QByteArray::number(f_v()) + ",";
	ret += QByteArray::number(f_hv()) + ",";
	ret += QByteArray::number(f_c()) + ",";
	ret += QByteArray::number(f_p()) + ",";
	ret += QByteArray::number(f_pc()) + ",";
	ret += QByteArray::number(f_cl()) + ",";

	ret += QByteArray::number(b_h()) + ",";
	ret += QByteArray::number(b_v()) + ",";
	ret += QByteArray::number(b_hv()) + ",";
	ret += QByteArray::number(b_c()) + ",";
	ret += QByteArray::number(b_p()) + ",";
	ret += QByteArray::number(b_pc()) + ",";
	ret += QByteArray::number(b_cl()) + ",";
			
	ret += QByteArray::number(fb_h()) + ",";
	ret += QByteArray::number(fb_v()) + ",";
	ret += QByteArray::number(fb_hv()) + ",";
	ret += QByteArray::number(fb_c()) + ",";
	ret += QByteArray::number(fb_p()) + ",";
	ret += QByteArray::number(fb_pc()) + ",";
	ret += QByteArray::number(fb_cl());

	ret += ");";

	return ret;
}
// --------------------------------------------------------------------	
QByteArray
JcnCalc::toSqlDelete()
{
	QByteArray ret;
	ret = "DELETE FROM " + SQLTABLE;
	ret += " WHERE jcntext = \"" + text() + "\"";

	return ret;
}
// --------------------------------------------------------------------	
QByteArray
JcnCalc::toSqlFullFind()
{
	QByteArray ret;
	ret = "SELECT * from " + SQLTABLE;

	ret += " WHERE ";

	ret +=     " f_h="   + QByteArray::number(f_h());
	ret += " AND f_v="   + QByteArray::number(f_v());
	ret += " AND f_hv="  + QByteArray::number(f_hv());
	ret += " AND f_c="   + QByteArray::number(f_c());
	ret += " AND f_p="   + QByteArray::number(f_p());
	ret += " AND f_pc="  + QByteArray::number(f_pc());
	ret += " AND f_cl="  + QByteArray::number(f_cl());

	ret += " AND b_h="   + QByteArray::number(b_h());
	ret += " AND b_v="   + QByteArray::number(b_v());
	ret += " AND b_hv="  + QByteArray::number(b_hv());
	ret += " AND b_c="   + QByteArray::number(b_c());
	ret += " AND b_p="   + QByteArray::number(b_p());
	ret += " AND b_pc="  + QByteArray::number(b_pc());
	ret += " AND b_cl="  + QByteArray::number(b_cl());

	ret += " AND fb_h="  + QByteArray::number(fb_h());
	ret += " AND fb_v="  + QByteArray::number(fb_v());
	ret += " AND fb_hv=" + QByteArray::number(fb_hv());
	ret += " AND fb_c="  + QByteArray::number(fb_c());
	ret += " AND fb_p="  + QByteArray::number(fb_p());
	ret += " AND fb_pc=" + QByteArray::number(fb_pc());
	ret += " AND fb_cl=" + QByteArray::number(fb_cl());

	return ret;
}
// ----------------------------------------------------------
void
JcnCalc::fromSqlQuery(QSqlQuery & qry)
{
	// text is at 0
	_ws = qry.value(0).toString().simplified().toLatin1();
}
// ----------------------------------------------------------

/*
   Matching rules
   
	-- no mods
	1. compare 
	2. compare src reversed
	3. compare trg reversed
	4. compare src and trg reversed      

	-- remove zeros
	1. compare 
	2. compare src reversed
	3. compare trg reversed
	4. compare src and trg reversed      

	-- mult by 2
	-- mult by 3
	-- plus 1

*/

#define MM(mm, d) (mm & d ? true : false)
#define MM2(mm, d1, d2) (mm & d1 && mm & d2? true : false)
#define MM3(mm, d1, d2, d3) (mm & d1 && mm & d2 && mm & d3 ? true : false)

#define MATCH_FIELD(key, mm)\
\
	if (jcndbg) \
		std::cout << "Enter MATCH_FIELD: " << xstr(key) << std::hex << " mm=" << std::bitset<16>(mm) << std::endl; \
\
/* no mod ops - only straigth and reversed */\
\
if ( key() == c.key() && MM(mm,JcnMatchAlgo[JcnMacthAlgo_exact].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "  exact: " << std::hex << int(mm) << std::endl; \
	}\
\
if ( rev(key()) == c.key() && MM(mm,JcnMatchAlgo[JcnMacthAlgo_rev].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "MATCH_FIELD rev 1: " << " mm=" << int(mm) << std::endl; \
}\
\
if ( key() == rev(c.key()) && MM(mm,JcnMatchAlgo[JcnMacthAlgo_rev].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "MATCH_FIELD rev 2: " << " mm=" << int(mm) << std::endl; \
}\
\
if ( rev(key()) == rev(c.key()) && MM(mm,JcnMatchAlgo[JcnMacthAlgo_rev].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "   rev 3: " << std::hex << " mm=" << int(mm) << std::endl; \
}\
\
/* ign0 ops - straight and reversed */\
\
if ( noz(key()) == noz(c.key()) && MM(mm,JcnMatchAlgo[JcnMacthAlgo_ign0].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "MATCH_FIELD ign0 1: " << " mm=" << int(mm) << std::endl; \
}\
\
if ( noz(rev(key())) == noz(c.key()) && MM2(mm,JcnMatchAlgo[JcnMacthAlgo_ign0].mask,JcnMatchAlgo[JcnMacthAlgo_rev].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "MATCH_FIELD ign0 2: " << " mm=" << int(mm) << std::endl; \
}\
\
if ( noz(key()) == noz(rev(c.key())) && MM2(mm,JcnMatchAlgo[JcnMacthAlgo_ign0].mask,JcnMatchAlgo[JcnMacthAlgo_rev].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "MATCH_FIELD ign0 3: " << " mm=" << int(mm) << std::endl; \
}\
\
if ( noz(rev(key())) == noz(rev(c.key()))  && MM2(mm,JcnMatchAlgo[JcnMacthAlgo_ign0].mask,JcnMatchAlgo[JcnMacthAlgo_rev].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "MATCH_FIELD ign0 4: " << " mm=" << int(mm) << std::endl; \
}\
\
/* mult by 2 ops */\
\
if ( key()*2 == c.key() && MM(mm,JcnMatchAlgo[JcnMacthAlgo_mul2].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "MATCH_FIELD mul2 1: " << " mm=" << int(mm) << std::endl; \
}\
\
if ( key() == c.key()*2 && MM(mm,JcnMatchAlgo[JcnMacthAlgo_mul2].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "MATCH_FIELD mul2 2: " << " mm=" << int(mm) << std::endl; \
}\
\
/* mult by 3 ops */\
\
if ( key()*3 == c.key() && MM(mm,JcnMatchAlgo[JcnMacthAlgo_mul3].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "MATCH_FIELD mul3 1: " << " mm=" << int(mm) << std::endl; \
}\
\
if ( key() == c.key()*3 && MM(mm,JcnMatchAlgo[JcnMacthAlgo_mul3].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "MATCH_FIELD mul3 2: " << " mm=" << int(mm) << std::endl; \
}\
\
/* plus 1 ops */\
\
if ( key()+1 == c.key() && MM(mm,JcnMatchAlgo[JcnMacthAlgo_plus1].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "MATCH_FIELD plus1 1: " << " mm=" << int(mm) << std::endl; \
}\
\
if ( key() == c.key()+1 && MM(mm,JcnMatchAlgo[JcnMacthAlgo_plus1].mask) ) \
{ \
	mask |= JcnDD[JcnDD_ ## key].mask; \
	ret = true; \
	if (jcndbg) \
		std::cout << "MATCH_FIELD plus1 2: " << " mm=" << int(mm) << std::endl; \
}\
if (jcndbg) \
	std::cout << "Exit  MATCH_FIELD: " << xstr(key) << std::hex << " mm=" << int(mm) << std::endl;


// ----------------------------------------------------------
bool
JcnCalc::match(JcnCalc & c, int & mask, int matchMask)
{
bool ret = false; 
	
	_mask = 0;
	
	// fwa matching
	MATCH_FIELD(f_h, matchMask)
	MATCH_FIELD(f_v, matchMask)
	MATCH_FIELD(f_hv, matchMask)
	MATCH_FIELD(f_c, matchMask)
	MATCH_FIELD(f_p, matchMask)
	MATCH_FIELD(f_pc, matchMask)
	MATCH_FIELD(f_cl, matchMask)
				
	// bwa matching
	MATCH_FIELD(b_h, matchMask)
	MATCH_FIELD(b_v, matchMask)
	MATCH_FIELD(b_hv, matchMask)
	MATCH_FIELD(b_c, matchMask)
	MATCH_FIELD(b_p, matchMask)
	MATCH_FIELD(b_pc, matchMask)
	MATCH_FIELD(b_cl, matchMask)

	// fwa + bwa matching
	MATCH_FIELD(fb_h, matchMask)
	MATCH_FIELD(fb_v, matchMask)
	MATCH_FIELD(fb_hv, matchMask)
	MATCH_FIELD(fb_c, matchMask)
	MATCH_FIELD(fb_p, matchMask)
	MATCH_FIELD(fb_pc, matchMask)
	MATCH_FIELD(fb_cl, matchMask)
						
	_mask = mask;

	if (jcndbg)
		std::cout << "1 JcnCalc::match " << _ws << " m=" << _mask << std::endl;

	return ret;
}

// ----------------------------------------------------------
bool
JcnCalc::match(JcnCalc & c, 
				int & mask, 
				int f_matchMask,
				int b_matchMask,
				int fb_matchMask			
				)
{
bool ret = false; 
	
	_mask = 0;
	
	// fwa matching
	MATCH_FIELD(f_h, f_matchMask)
	MATCH_FIELD(f_v, f_matchMask)
	MATCH_FIELD(f_hv, f_matchMask)
	MATCH_FIELD(f_c, f_matchMask)
	MATCH_FIELD(f_p, f_matchMask)
	MATCH_FIELD(f_pc, f_matchMask)
	MATCH_FIELD(f_cl, f_matchMask)
				
	// bwa matching
	MATCH_FIELD(b_h, b_matchMask)
	MATCH_FIELD(b_v, b_matchMask)
	MATCH_FIELD(b_hv, b_matchMask)
	MATCH_FIELD(b_c, b_matchMask)
	MATCH_FIELD(b_p, b_matchMask)
	MATCH_FIELD(b_pc, b_matchMask)
	MATCH_FIELD(b_cl, b_matchMask)

	// fwa + bwa matching
	MATCH_FIELD(fb_h, fb_matchMask)
	MATCH_FIELD(fb_v, fb_matchMask)
	MATCH_FIELD(fb_hv, fb_matchMask)
	MATCH_FIELD(fb_c, fb_matchMask)
	MATCH_FIELD(fb_p, fb_matchMask)
	MATCH_FIELD(fb_pc, fb_matchMask)
	MATCH_FIELD(fb_cl, fb_matchMask)
						
	_mask = mask;

	if (jcndbg)
		std::cout << "2 JcnCalc::match " << _ws << " m=" << _mask << std::endl;

	return ret;
}
// ----------------------------------------------------------
bool
JcnCalc::equal(JcnCalc & c)
{
bool ret;

	ret = 
		( f_h() == c.f_h() ) &&
		( f_v() == c.f_v() ) &&
		( f_hv() == c.f_hv() ) &&
		( f_c() == c.f_c() ) &&
		( f_p() == c.f_p() ) &&
		( f_pc() == c.f_pc() ) &&
		( f_cl() == c.f_cl() ) &&
								
		( b_h() == c.b_h() ) &&
		( b_v() == c.b_v() ) &&
		( b_hv() == c.b_hv() ) &&
		( b_c() == c.b_c() ) &&
		( b_p() == c.b_p() ) &&
		( b_pc() == c.b_pc() ) &&
		( b_cl() == c.b_cl() ) &&

		( fb_h() == c.fb_h() ) &&
		( fb_v() == c.fb_v() ) &&
		( fb_hv() == c.fb_hv() ) &&
		( fb_c() == c.fb_c() ) &&
		( fb_p() == c.fb_p() ) &&
		( fb_pc() == c.fb_pc() ) &&
		( fb_cl() == c.fb_cl() )

		;
	
	return ret;
}
// ----------------------------------------------------------
bool
JcnCalc::equal(JcnCalc & c, int f_cnt, int b_cnt, int f_b_cnt)
{
	int t_f_cnt = 0;
	int t_b_cnt = 0;
	int t_f_b_cnt = 0;

	
	if ( f_h() == c.f_h() )		t_f_cnt++;
	if ( f_v() == c.f_v() ) 	t_f_cnt++;
	if ( f_hv() == c.f_hv() ) 	t_f_cnt++;
	if ( f_c() == c.f_c() ) 	t_f_cnt++;
	if ( f_p() == c.f_p() ) 	t_f_cnt++;
	if ( f_pc() == c.f_pc() ) 	t_f_cnt++;
	if ( f_cl() == c.f_cl() ) 	t_f_cnt++;
								
	if ( b_h() == c.b_h() )		t_b_cnt++;
	if ( b_v() == c.b_v() )		t_b_cnt++;
	if ( b_hv() == c.b_hv() )	t_b_cnt++;
	if ( b_c() == c.b_c() )		t_b_cnt++;
	if ( b_p() == c.b_p() )		t_b_cnt++;
	if ( b_pc() == c.b_pc() )	t_b_cnt++;
	if ( b_cl() == c.b_cl() )	t_b_cnt++;

	if ( fb_h() == c.fb_h() )	t_f_b_cnt++;
	if ( fb_v() == c.fb_v() )	t_f_b_cnt++;
	if ( fb_hv() == c.fb_hv() )	t_f_b_cnt++;
	if ( fb_c() == c.fb_c() )	t_f_b_cnt++;
	if ( fb_p() == c.fb_p() )	t_f_b_cnt++;
	if ( fb_pc() == c.fb_pc() )	t_f_b_cnt++;
	if ( fb_cl() == c.fb_cl() )	t_f_b_cnt++;
	
	if (
		t_f_cnt >= f_cnt &&
		t_b_cnt >= b_cnt &&
		t_f_b_cnt >= f_b_cnt
		)
	{
		return true;
	}
	else
	{
		return false;
	}
}
// ----------------------------------------------------------
