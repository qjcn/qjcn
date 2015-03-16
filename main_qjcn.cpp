#include <iostream>
#include <unistd.h>
#include <QtCore/QObject>
#include <QtCore/QString>

#ifdef WITH_QT5
	#include <QtWidgets/QApplication>
#else
	#include <QtGui/QApplication>
#endif

#include "Jcn.h"
#include "DD.h"


// -----------------------------------------------------------
void
proc_opt(int argc, char ** argv)
{
extern char *optarg;

	int c;
   	while ((c = getopt(argc, argv, "d:?")) != EOF)
   	{
		switch (c) 
       	{
        	case 'd':
	       	{
				jcndbg = atoi(optarg);
            }
           	break;

        	case '?':
			default:
	       	{
				std::cout << "Usage: " << argv[0] << " [-d n]" << std::endl;
				std::cout <<  "    -d [n]  debug on with level" << std::endl;
				exit(1);
            }
           	break;
       	}
   	}
}
// -----------------------------------------------------------
int 
main(int argc, char ** argv)
{
	proc_opt(argc, argv);
    QApplication app(argc, argv);	
	Jcn jcn(&app);
	jcn.show();
	app.exec();
	return 0;
}
