#ifndef qt_qtutil_h
#define qt_qtutil_h

#include <sys/time.h>

#include <stdlib.h>
#include <iostream>
#include <QtCore/QList>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtCore/QDateTime>
#include <QtCore/QVariant>
#include <QtCore/QString>
#include <QtCore/QByteArray>
#include <QtCore/QTextStream>
#include <QtCore/QMutex>

#define xstr(s)			zstr(s)
#define zstr(s)			#s
#define xcat2(a,b)		zstr(a) zstr(b)
#define xcat3(a,b,c)	zstr(a) zstr(b) zstr(c)


std::ostream &
operator<<(std::ostream & os, const QVariant & v);

std::ostream &
operator<<(std::ostream & os, const QString & str);

std::ostream &
operator<<(std::ostream & os, const QByteArray & str);

std::ostream &
operator<<(std::ostream & os, const QDate & d);

std::ostream &
operator<<(std::ostream & os, const QTime & t);

QByteArray	readToDelim(QTextStream & s, char d);

typedef QList<QByteArray> qbal_t;

struct cmp_qbar
{
	bool operator() (const QByteArray & s1, const QByteArray & s2) const
	{ return s1 < s2; }
};

double				qbar_to_double_cvt(const QByteArray & s);
float				qbar_to_float_cvt(const QByteArray & s);
bool 				qbar_to_bool_cvt(const QByteArray & s);
int 				qbar_to_int_cvt(const QByteArray & s);
long 				qbar_to_long_cvt(const QByteArray & s);
short				qbar_to_short_cvt(const QByteArray & s);
char				qbar_to_char_cvt(const QByteArray & s);
uint 				qbar_to_uint_cvt(const QByteArray & s);
ulong 				qbar_to_ulong_cvt(const QByteArray & s);
ushort 				qbar_to_ushort_cvt(const QByteArray & s);

void				print_bin_buffer(const QByteArray & s);


int					noz(int);
int					rev(int);
QString				rev_qstr(const QString & s);
QByteArray			rev_qbar(const QByteArray & s);


// ----------------------------------------------------------

void gtd(timeval * tv);
QByteArray	ts_su();
QByteArray 	ts_hms();
QByteArray 	ts_hmsu();


#endif
