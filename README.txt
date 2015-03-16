Initial creation of qjcn project in git

mkdir -p qjcn
cp -r qjcn2_clean/* qjcn

cd qjcn
git init
git config user.email "qjcn@appqloud.com"
git add *
git commit -m "first commit"
git remote add origin https://github.com/qjcn/qjcn.git
git push -u origin master



# clone repository
git clone https://github.com/qjcn/qjcn.git

##############################################################################

qjcn depends on qt-4.8.x
on Windows it needs: mingw (with gcc4.8+) and optional msys

This release is built with qt-4.8.2

Links for qt in mingw from nokia
  http://get.qt.nokia.com/qt/source/qt-everywhere-opensource-src-4.8.2.zip
  http://get.qt.nokia.com/misc/MinGW-gcc440_1.zip

#############################################################
Instrtuction for building qt on windows

Install qt-4.8.2 into c:\qt\4.8.2
Unzip mingw in c:\mingw

Goto c:\qt\4.8.2

optional:
vi mkspec/win32-g++\qmake.conf
   line 26
   change QMAKE_CFLAGS_RELEASE=-03


run: configure.exe -qt-libtiff -qt-libpng -qt-libmng -qt-libjpeg -qt-sql-sqlite -no-opengl -release -static
