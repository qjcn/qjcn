#ifndef jcnsql_h
#define jcnsql_h

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>

#include <QtCore/QList>
#include <QtCore/QDate>
#include <QtCore/QByteArray>

namespace jcnsql
{

	QByteArray createSchema_Sql()
	{
		QByteArray ret;
		
		ret = "CREATE TABLE jcn_table ";
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
		ret += " fb_cl NUMERIC  ";
		
		ret += "); ";		
	}

} // namespace


#endif

