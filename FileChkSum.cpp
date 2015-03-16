#include <stdlib.h>
#include <iostream>
#include "FileChkSum.h"

// ------------------------------------------------------------
FileChkSum::FileChkSum(const QByteArray & fileName) :
	_fileName(fileName)
{
}
// ------------------------------------------------------------
unsigned long long
FileChkSum::calc()
{
	std::cout << "FileChkSum::calc" << std::endl;
	
	QFileInfo fi(_fileName);
	if ( !fi.exists() )
		return 0;
	
	unsigned long long fsz = fi.size();
	std::cout << "File Size: " << fsz << std::endl;
		
	char * buffer = (char *)malloc(fsz + 10);
	
	QFile f(_fileName);
	if (! f.open(QIODevice::ReadOnly) )
	{
		free(buffer);
		return 0;
	}
	
	unsigned long long r;
	if ( (r = f.read(buffer, fsz)) == -1)
	{
		free(buffer);
		return 0;
	}

	std::cout << "Bytes Read: " << r << std::endl;

	char * buf = buffer; // save buf

	unsigned long long ret=0;
	for (unsigned long long i = 0; i < fsz; ++i)
		ret += (char)(*buf++);

	free(buffer);

	f.close();
	std::cout << "FileChkSum::calc done: " << ret << std::endl;
	return ret;
}
// ------------------------------------------------------------
