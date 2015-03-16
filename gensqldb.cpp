#include <iostream>

#include <QtCore/QFile>
#include <QtCore/QByteArray>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>

#include "JcnCalc.h"
#include "qtutil.h"

int
main(int argc, char ** argv)
{
	if (argc <= 1)
	{
		std::cout << "Usage: " << argv[0] << " file name" << std::endl;
		exit(1);
	}

	QSqlDatabase db;
	db = QSqlDatabase::addDatabase("QSQLITE");
	QByteArray fname = argv[1];
	QByteArray dbFileName = fname + ".sql";
	if ( ! QFile::exists(dbFileName) )
	{
       	std::cout << "Creating SQLDB: " << dbFileName << std::endl;

	    // let's open this database
    	db.setDatabaseName(dbFileName);
	    if ( ! db.open() )
    	{
    		std::cout << "Can not create SQLDB: " << dbFileName << std::endl;	
			exit(-1);
    	}
		
		std::cout << "Creating schema: " << JcnCalc::toSqlCreate() << std::endl;
		QSqlQuery query  = db.exec(JcnCalc::toSqlCreate());
		if ( query.lastError().isValid() )
		{
			std::cout << query.lastError().databaseText() << std::endl;
			std::cout << "Failed to create schema: " << JcnCalc::toSqlCreate() << std::endl;
			exit(-1);
		}
		
		// load database from text file
		std::cout << "Loading SQLDB from TextDB." << std::endl;
		QByteArray textDbFn = fname;
   		QFile file(textDbFn);
   		if(! file.open(QIODevice::ReadOnly | QIODevice::Text))
   		{
			std::cout << "Can't open file: " << textDbFn << std::endl;
			exit(-1);
		}


		QByteArray line;
		int count_lines=0;
		while( !file.atEnd() )
		{
			line = file.readLine();
			count_lines++;
		}
		file.reset();

		int count = 0;

		while( !file.atEnd() )
		{
    		// strip leading and trailing spaces
			line = file.readLine().simplified();
			if (line[0] == '#' || line.length() == 0) continue;
			line.replace('"',' ');  // make sure we do not have quotes embedded
			line.replace('\'',' ');  // make sure we do not have quotes embedded
			
			count++;
			JcnCalc c(line);
			
			QSqlQuery query  = db.exec(c.toSqlInsert());
			if ( query.lastError().isValid() )
			{
				std::cout << "QRY Error: " << query.lastError().databaseText() << std::endl;
				std::cout << "LINE=" << line << std::endl;
			}
		}
	}

	return 0;
}
