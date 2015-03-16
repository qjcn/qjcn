
#include "qtutil.h"

// --------------------------------------------------------------------
std::ostream &
operator<<(std::ostream & os, const QVariant & v)
{	
	return os << v.toString();
}
// --------------------------------------------------------------------
std::ostream &
operator<<(std::ostream & os, const QString & str)
{	
	return os << qPrintable(str);
}
// --------------------------------------------------------------------
std::ostream &
operator<<(std::ostream & os, const QByteArray & str)
{	
	return os << str.constData();
}
// --------------------------------------------------------------------
std::ostream &
operator<<(std::ostream & os, const QDate & d)
{	
	return os << d.toString();
}
// --------------------------------------------------------------------
std::ostream &
operator<<(std::ostream & os, const QTime & t)
{	
	return os << t.toString();
}
// --------------------------------------------------------------------
QByteArray
readToDelim(QTextStream & s, char d)
{
QByteArray ret="";
char c;
bool found = false;

	while (!s.atEnd() || !found)
	{
		s >> c;
		if (c==d) break;
		ret +=c;
    }
	return ret;
}
// -----------------------------------------------

double				qbar_to_double_cvt(const QByteArray & s) 			{ return atof(s.data()); }
float				qbar_to_float_cvt(const QByteArray & s) 			{ return (float)atof(s.data()); }
bool 				qbar_to_bool_cvt(const QByteArray & s) 				{ return (bool)atoi(s.data()); }
int 				qbar_to_int_cvt(const QByteArray & s)				{ return atoi(s.data()); }
long 				qbar_to_long_cvt(const QByteArray & s)				{ return atol(s.data()); }
short				qbar_to_short_cvt(const QByteArray & s)				{ return (short)atoi(s.data()); }
char				qbar_to_char_cvt(const QByteArray & s)				{ return (char)(s.at(0)); }
uint 				qbar_to_uint_cvt(const QByteArray & s)				{ return (unsigned int)atoi(s.data()); }
ulong 				qbar_to_ulong_cvt(const QByteArray & s)				{ return (unsigned long)atol(s.data()); }
ushort 				qbar_to_ushort_cvt(const QByteArray & s)			{ return (unsigned short)atoi(s.data()); }

void				
print_bin_buffer(const QByteArray & s)
{
	for (int i=0; i< s.length(); ++i)
	{
		printf("<%d[%d=%c]>", i, s.at(i), s.at(i));
	}
	printf("\n");
}

// ----------------------------------------------------------
int
noz(int n)
{
	QByteArray s = QByteArray::number(n);
	QByteArray result;
	for (int i=0; i< s.length(); ++i)
	{
		if ( s.at(i) != '0' )
			result += s.at(i);
	}

	return result.toInt();
}
// ----------------------------------------------------------
int
rev(int n)
{
	QByteArray s = QByteArray::number(n);
	QByteArray result;
	for (int i = s.length()-1; i >= 0; --i)
	{
		result += s.at(i);
	}

	return result.toInt();
}
// ----------------------------------------------------------
QString
rev_qstr(const QString & s)
{
QString ret;

	for (int i = s.length()-1; i>=0; --i)
	{
		ret += s[i];
	}

	return ret;
}
// ----------------------------------------------------------
QByteArray
rev_qbar(const QByteArray & s)
{
QByteArray ret;

	for (int i = s.length()-1; i>=0; --i)
	{
		ret += s[i];
	}

	return ret;
}
/*
// -----------------------------------------------------------------
void
gtd(timeval * tv)
{
    gettimeofday(tv, NULL); 
}
// -----------------------------------------------------------------
// returns timestamp in SECONDS USEC
// -----------------------------------------------------------------
QByteArray
ts_su()
{
	timeval tv;
	gtd(&tv);
	char cbuf[64];
	snprintf(cbuf,64,"%ld.%06ld", tv.tv_sec, tv.tv_usec );
    return QByteArray(cbuf);
}
// -----------------------------------------------------------------
// returns timestamp in HOURS:MINUTES:SECONDS
// -----------------------------------------------------------------
QByteArray 
ts_hms()
{
	timeval tv;
	gtd(&tv);
	char cbuf[64];
	struct tm	result;
	
	struct tm * t = localtime_r(&tv.tv_sec, &result);
	if ( ! t )
		return "";

	snprintf(cbuf,64,"%02d:%02d:%02d",t->tm_hour, t->tm_min, t->tm_sec );
    return QByteArray(cbuf);
}
// -----------------------------------------------------------------
// returns timestamp in HOURS:MINUTES:SECONDS.USEC
// -----------------------------------------------------------------
QByteArray 
ts_hmsu()
{
	timeval tv;
	char cbuf[64];
	struct tm result;
	gtd(&tv);
	struct tm * t = localtime_r(&tv.tv_sec, &result);
	snprintf(cbuf,64,"%02d:%02d:%02d.%06ld",
			t->tm_hour, t->tm_min, t->tm_sec, tv.tv_usec );
    return QByteArray(cbuf);
}
// -----------------------------------------------------------------
*/

