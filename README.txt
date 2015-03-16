Initial creation of qjcn project in git:

	mkdir -p qjcn
	cp -r qjcn_clean/* qjcn

	cd qjcn
	git init
	git config user.email "qjcn@appqloud.com"
	git add *
	git commit -m "first commit"
	git remote add origin https://github.com/qjcn/qjcn.git
	git push -u origin master


To clone repository:
	git clone https://github.com/qjcn/qjcn.git

Workflow:
	edit
	git commit -a -m "message"
	git push --al

To get last revision commited:
	git rev-list --count HEAD

Binary can be downloaded from here:
  https://github.com/qjcn/qjcn/releases/download/12/qjcn.exe


--------------------------------------------------------

qjcn depends on qt-4.8.x

on Windows it also needs: mingw (with gcc4.8+) and optional msys

This release was built with qt-4.8.2

Software Links
	http://download.qt.io/archive/qt/4.8/4.8.2/qt-everywhere-opensource-src-4.8.2.tar.gz
	http://download.qt.io/archive/qt/4.8/4.8.2/qt-win-opensource-4.8.2-mingw.exe

	standalone mingw
		http://sourceforge.net/projects/mingw/files/latest/download?source=files

--------------------------------------------------------

Instrtuction for building qt

Install qt-4.8.2 into c:\qt\4.8.2
Unzip mingw in c:\mingw

Goto c:\qt\4.8.2

optional:
vi mkspec/win32-g++\qmake.conf
   line 26
   change QMAKE_CFLAGS_RELEASE=-03


run: configure.exe -qt-libtiff -qt-libpng -qt-libmng -qt-libjpeg -qt-sql-sqlite -no-opengl -release -static
