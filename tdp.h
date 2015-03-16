
#ifndef TDP_H
#define TDP_H

#include "ui_tdp.h"
	
#ifdef WITH_QT5
	#include <QtGui/QTextLength>
	#include <QtPrintSupport/QPrinter>

	#include <QtWidgets/QDialog>
	#include <QtWidgets/QTableView>
	#include <QtWidgets/QFileDialog>
	#include <QtWidgets/QGraphicsView>
#else
	#include <QtGui/QDialog>
	#include <QtGui/QTableView>
	#include <QtGui/QPrinter>
	#include <QtGui/QTextLength>
	#include <QtGui/QFileDialog>
	#include <QtGui/QGraphicsView>
#endif

#include <vector>

class QGraphicsScene;
class QAbstractItemModel;

// Text preview widget
class TDP : public QDialog
{
	Q_OBJECT
	Q_ENUMS (Grids)
	Q_ENUMS (RowLayouts)
	
public:

	enum Grids 
	{
		NoGrid	=			0x0,
		NormalGrid	=		0x1,
		AlternateColor	=	0x2,
		AlternateWithGrid =	0x3
	};

	enum RowLayouts
	{
		SingleRow	=		0x0,
		MultiRow	=		0x1,
		MultiPage	=		0x2,
	};


	TDP(QApplication * a, QTableView * p_tableView, QPrinter * p_printer,  QWidget *parent=0);
    virtual ~TDP();
	
	virtual void	setHeaderText(const QString &text);
	virtual void	setGridMode(Grids);
	virtual void	setRowLayoutMode(RowLayouts);
	virtual void	print();
	virtual int		exec();
	virtual void	exportPdf(const QString &filename);

			void	prep_gsv();
	
private slots:
	virtual void 	on_setupToolButton_clicked();
	virtual void	on_zoomInToolButton_clicked();
	virtual void	on_zoomOutToolButton_clicked();
	virtual void 	showPage(int pagenum);
	
	virtual void 	savePdf();
	virtual void	apply();
			
private:
    Ui_TDP			_ui;
	
	virtual void render_it();

	virtual void setupSpinBox();

	QGraphicsView * _view;
	QTableView *	_tableView;
	QPrinter *		_printer;
	TDP::Grids		_gridMode;
	TDP::RowLayouts _rowLayoutMode;

	int 			_rows;
	int 			_pages;
	int 			_leftMargin;
	int 			_rightMargin;
	int 			_topMargin;
	int 			_bottomMargin;
	int 			_spacing;
	int 			_headerSize;
	int 			_footerSize;
	int 			_sceneZoomFactor;
	double 			_columnZoomFactor;
	double 			_rowHeight;
	double 			_columnMultiplier;
	QString			_headerText;
    QString 		_footerText;

	QVector<QTextLength>	_colSizes;
	QAbstractItemModel *	_model;

	QFont 			_titleFont;
	QFont 			_headerFont;
	QFont 			_font;
	QFontMetrics *	_titleFmt;
	QFontMetrics *	_headerFmt;
	QFontMetrics *	_fmt;
	QString 		_headerStdText;

	QApplication *	_qapp;

	std::vector<QGraphicsScene*> _gsv;

	void			adjCellFont(QFont & cellFont);
	
	int 			_cellSizeOffset;
	int 			_cellFontOffset;

	void	calc_optimal();
	void	cleanup();
	void	prep_DocHeader(QGraphicsScene * gs, int startCol, int endCol);

	void	prep_Page(QGraphicsScene * gs, int pageCount);
	void	prep_TableHeader(QGraphicsScene * gs);
	void	prep_PageHeader(QGraphicsScene * gs);
	void	prep_PageFooter(QGraphicsScene * gs, int pageCount);
};

#endif
