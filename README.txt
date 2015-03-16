Initial creation of qjcn2 project in git

mkdir -p qjcn2
cp -r qjcn2_clean/* qjcn2

cd qjcn2
git init
git config user.email "qjcn@appqloud.com"
git add *
git commit -m "first commit"
git remote add origin https://github.com/qjcn/qjcn.git
git push -u origin master






to checkout:

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
