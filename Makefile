CC=g++

CPPFLAGS=-c -g -O3

B_OS:=$(shell uname -s)
B_MARCH:=$(shell uname -m)

ifeq ($(B_OS),Linux)
	QTDIR=$(SHARE)/qt4
	JCNOS=JCNOS_LINUX
else
	QTDIR=c:/qt/4.8.2
	JCNOS=JCNOS_WIN
endif

DEFINES=-DJCN_BUILD_DATE=$(shell date +%F.%T) -DJCN_REV_TAG="$(shell git rev-list --count HEAD)" -D$(JCNOS)

INCLUDES=\
	-I./ \
	-I$(QTDIR)/include -I$(QTDIR)/include/QtCore


ifeq ($(B_OS),Linux)
	ifeq ($(B_MARCH),x86_64)
	    LIBS= -Xlinker -Bstatic -L$(QTDIR)/lib -lQtCore -lQtXml -lQtCore -lQtGui -lQtCore -lQtSql \
	    -Xlinker -Bdynamic -L/usr/X11R6/lib64 \
	    -lXext -lX11 \
	    -Xlinker -Bdynamic -lpthread -lrt -lz -ldl
#	    -Xlinker -Bdynamic -lpthread -lgthread-2.0 -lglib-2.0 -lrt -lz -ldl
	else
	    LIBS= -Xlinker -Bstatic -L$(QTDIR)/lib -lQtCore -lQtXml -lQtCore -lQtGui -lQtCore -lQtSql \
	    -Xlinker -Bdynamic -L/usr/X11R6/lib -lpng12 -lSM -lICE -lXext -lX11 \
	    -Xlinker -Bdynamic -lpthread -lgthread-2.0 -lglib-2.0 -lrt -lz -ldl
	endif
else
	LIBS=-L$(QTDIR)/lib -lmingw32 -lqtmain -lQtGui -lQtSql \
		-lgdi32 -lcomdlg32 -loleaut32 -limm32 -lwinmm \
		-lwinspool -lmsimg32 -lQtCore -lkernel32 \
		-luser32 -lshell32 -luuid -lole32 -ladvapi32 \
		-lws2_32 -static
endif


LDFLAGS=

ifeq ($(B_OS),Linux)
JCNEXE=qjcn
else
JCNEXE=qjcn.exe
endif

JCN_RCC=jcn.rcc

JCN_UIS=ui_tdp.h ui_JcnForm.h
JCN_MOCS=moc_tdp.cpp moc_Jcn.cpp moc_textedit.cpp moc_JcnSqlModel.cpp

CMN_OBJECTS= \
	qtutil.o \
	FileChkSum.o \
	moc_tdp.o \
	tdp.o \
	moc_Jcn.o \
	Jcn.o \
	Jcn_TabTranslate.o \
	Jcn_TabDb.o \
	Jcn_TabFindCompare.o \
	JcnCalc.o \
	DD.o \
	JcnModel.o \
	JcnSqlModel.o \
	moc_JcnSqlModel.o \
	ccalc.o \
	Express.o \
	textedit.o \
	moc_textedit.o

JCN_OBJECTS= \
	$(CMN_OBJECTS)\
	main_qjcn.o

DBTOOL_OBJECTS= \
	$(CMN_OBJECTS)\
	gensqldb.o

all:	$(JCNEXE)


$(JCNEXE): $(JCN_UIS) $(JCN_MOCS) $(JCN_RCC) $(JCN_OBJECTS)
	$(CC) $(JCN_OBJECTS) $(LDFLAGS) $(LIBS) -o $@
	strip -s $(JCNEXE)

$(DBTOOL): $(DBTOOL_OBJECTS)
	$(CC) $(DBTOOL_OBJECTS) $(LDFLAGS) $(LIBS) -o $@
	strip -s $(DBTOOL)

clean:
	rm -f *.o *.exe $(JCNEXE) core* moc_* ui_*  *.rcc

# ui rule
%.rcc : %.qrc
	$(QTDIR)/bin/rcc $< -o $@

ui_%.h : %.ui
	$(QTDIR)/bin/uic $< -o $@

# moc rule
moc_%.cpp : %.h
	$(QTDIR)/bin/moc $< -o $@

%: %.cpp
	$(CC) $< $(DEFINES) $(INCLUDES) $(LDFLAGS) $(LIBS) -o $@

%.o: %.cpp
	$(CC) $(CPPFLAGS) $(DEFINES) $(INCLUDES) $< -o $@
