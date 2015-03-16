#ifndef FileChkSum_h
#define FileChkSum_h

#include <QtCore/QByteArray>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

class FileChkSum
{
public:
	FileChkSum(const QByteArray & fileName);
	unsigned long long calc();	

private:
	QByteArray _fileName;
};

#endif
