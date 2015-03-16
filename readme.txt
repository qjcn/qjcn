Initial creation of qjcn2 project in svn

mkdir -p qjcn2.svn
cp -r qjcn2 qjcn2.svn

cd qjcn2.svn
svn import qjcn2 https://jcn.googlecode.com/svn/trunk/qjcn2 -m "init" --username jcn@appqloud.com



to checkout crypt:

# readonly checkout
svn co http://jcn.googlecode.com/svn/trunk/qjcn2 qjcn2

# commiting checkout
svn co https://jcn.googlecode.com/svn/trunk/qjcn2 qjcn2 --username jcn@appqloud.com

##############################################################################

required for building:
   qt-4.8.2
   mingw gcc44

optional:
   msys

Links for qt in mingw from nokia
  http://get.qt.nokia.com/qt/source/qt-everywhere-opensource-src-4.8.2.zip
  http://get.qt.nokia.com/misc/MinGW-gcc440_1.zip


Install qt-4.8.2 into c:\qt\4.8.2
Unzip mingw in c:\mingw


Goto c:\qt\4.8.2

vi mkspec/win32-g++\qmake.conf
   line 26
   change QMAKE_CFLAGS_RELEASE=-03


run: configure.exe -qt-libtiff -qt-libpng -qt-libmng -qt-libjpeg -qt-sql-sqlite -no-opengl -release -static 
