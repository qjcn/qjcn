#include <iostream>
#include "qtutil.h"

#include "tdp.h"

#ifdef WITH_QT5
	#include <QtPrintSupport/QPageSetupDialog>
	#include <QtPrintSupport/QPrintDialog>

	#include <QtCore/QDebug>
	#include <QtCore/QTime>
	#include <QtCore/QRectF>

	#include <QtGui/QTextDocument>
	#include <QtGui/QPainter>
	#include <QtGui/QStandardItemModel>	
	
	#include <QtWidgets/QGraphicsTextItem>
	#include <QtWidgets/QHeaderView>
	#include <QtWidgets/QProgressDialog>
	#include <QtWidgets/QMessageBox>

#else
	#include <QtCore/QDebug>
	#include <QtGui/QTextDocument>
	#include <QtGui/QPageSetupDialog>
	#include <QtGui/QGraphicsTextItem>
	#include <QtGui/QHeaderView>
	#include <QtGui/QPainter>
	#include <QtCore/QTime>
	#include <QtGui/QPrintDialog>
	#include <QtCore/QRectF>
	#include <QtGui/QStandardItemModel>
	#include <QtGui/QProgressDialog>
	#include <QtGui/QMessageBox>
#endif

//=================================================================
TDP::TDP(
		QApplication * a,
		QTableView * p_tableView,
		QPrinter * p_printer,
		QWidget * parent
	 ) :
	QDialog(parent)
{
	_cellSizeOffset=0;
	_cellFontOffset=0;
	
	_qapp = a;
	
    //variables
    _tableView	= 	p_tableView;

    _leftMargin 	= 20;
    _rightMargin	= 20;
    _topMargin		= 20;
    _bottomMargin	= 20;
    _spacing		= 4;
    _headerSize		= 100;
    _footerSize		= 60;

    _sceneZoomFactor= 100;
    _columnZoomFactor=0.65;
    _gridMode		=	NoGrid;
    _rowLayoutMode	=	SingleRow;
	
    _model 			= _tableView->model();
	_rows  			= _model->rowCount()-1;

    _rowHeight 		= 13 + _spacing;
		
    //setup printer
    _printer	=	p_printer;
    _printer->setFullPage(true);
    switch (QLocale::system().country()) 
	{
	    case QLocale::AnyCountry:
	    case QLocale::Canada:
	    case QLocale::UnitedStates:
	    case QLocale::UnitedStatesMinorOutlyingIslands:
			_printer->setPageSize(QPrinter::Letter);
        break;

	    default:
            _printer->setPageSize(QPrinter::A4);
        break;
    }

    setHeaderText("Document");
    _footerText = trUtf8("Page: ");

    // get column widths
    for (int i=0; i < _model->columnCount(); i++)
    {
        int colWidth = _tableView->columnWidth(i);
        _colSizes.append(QTextLength(QTextLength::FixedLength, colWidth));
    }

    // title font
    _titleFont = _tableView->font();
    _titleFont.setPointSize(10);
    _titleFont.setBold(false);
    _titleFont.setUnderline(false);
    _titleFmt = new QFontMetrics(_titleFont);

    // header font
    _headerFont = _tableView->font();
    _headerFont.setBold(true);
    _headerFont.setUnderline(true);
    _headerFont.setPointSize(10);
    _headerFmt = new QFontMetrics(_headerFont);

    // normal font
    _font = _tableView->font();
    _font.setPointSize(8);
    _fmt = new  QFontMetrics(_font);

    QString date=QDate::currentDate().toString(QLocale().dateFormat());
    QString time=QTime::currentTime().toString(QLocale().timeFormat(QLocale::ShortFormat));
    _headerStdText = date + "  " + time;
}
//=================================================================
int 
TDP::exec()
{
	//GUI
    _ui.setupUi(this);

    connect(_ui.page_SB, SIGNAL(valueChanged(int)), this, SLOT( showPage(int)));
    connect(_ui.okPdfButton, SIGNAL(clicked()), 	  this, SLOT(savePdf()));
    connect(_ui.apply_PB, SIGNAL(clicked()), 	  this, SLOT(apply()));
	
    _ui.graphicsView->ensureVisible(0,0,10,10);

	// prep it
	render_it();

/*
    if(_pages == 0) return QDialog::exec();
			
    setupSpinBox();
		
	// shows first page
    on_pageSpinBox_valueChanged(1);
*/

    return QDialog::exec();
}
//=================================================================
TDP::~TDP()
{
	cleanup();
}
//=================================================================
void 
TDP::on_setupToolButton_clicked()
{
    QPageSetupDialog * dialog;
    dialog = new QPageSetupDialog(_printer, this);
    if (dialog->exec() == QDialog::Rejected)
	{
        return;
    }

	render_it();

}
//=================================================================
void 
TDP::on_zoomInToolButton_clicked()
{
	_ui.graphicsView->scale(1.5, 1.5);
}
//=================================================================
void 
TDP::on_zoomOutToolButton_clicked()
{
	_ui.graphicsView->scale(1 / 1.5, 1 / 1.5);
}
//=================================================================
void 
TDP::setupSpinBox()
{
    _ui.page_SB->setPrefix(QString::number(_pages) + " / ");
    _ui.page_SB->setMaximum(_pages);
    _ui.page_SB->setValue(1);
	showPage(1);
}
//=================================================================
void 
TDP::render_it()
{
	cleanup();
	
	calc_optimal();
	
	prep_gsv();

    setupSpinBox();
}
//=================================================================
void 
TDP::setHeaderText(const QString &text)
{
	_headerText = text;
}
//=================================================================
void 
TDP::print()
{
	//printDialog
    _printer->setFromTo(1, _pages);
    _printer->setOutputFormat(QPrinter::NativeFormat);
    _printer->setOutputFileName("");

    QPrintDialog dialog(_printer, this);

    dialog.setWindowTitle("Print Document");
    if (dialog.exec() == QDialog::Rejected) 
	{
        return;
    }

    // get num copies
    int doccopies;
    int pagecopies;
    if (_printer->collateCopies()) 
	{
        doccopies = 1;
		pagecopies = _printer->numCopies();
    } 
	else 
	{
        doccopies = _printer->numCopies();
        pagecopies = 1;
    }

    // get page range
    int firstpage = _printer->fromPage();
    int lastpage = _printer->toPage();
    if (firstpage == 0 && lastpage == 0) 
	{ // all pages
        firstpage = 1;
        lastpage = _pages;
    }

    // print order
    bool ascending = true;
    if (_printer->pageOrder() == QPrinter::LastPageFirst) 
	{
        int tmp = firstpage;
        firstpage = lastpage;
        lastpage = tmp;
        ascending = false;
    }

    // loop through and print pages
    QPainter painter(_printer);
    for (int dc=0; dc < doccopies; dc++) 
	{
        int pagenum = firstpage;
        while (true) 
		{
            for (int pc=0; pc < pagecopies; pc++) 
			{
                if (_printer->printerState() == QPrinter::Aborted ||
                    _printer->printerState() == QPrinter::Error) 
				{
                    break;
                }

				// print page
				_gsv[pagenum-1]->render(&painter);
                if (pc < pagecopies-1) _printer->newPage();
            }

			if (pagenum == lastpage) break;

			if (ascending) 
			{
                pagenum++;
            } 
			else 
			{
                pagenum--;
            }
            _printer->newPage();
        }

        if (dc < doccopies-1) _printer->newPage();
    }

}
//=================================================================
void 
TDP::savePdf()
{
	exportPdf("jcn_results_" + QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + ".pdf");
}
//=================================================================
void 
TDP::apply()
{
//std::cout << "apply" << std::endl;

	_cellFontOffset = _ui.fontSize_SB->value();
	_cellSizeOffset = _ui.cellSize_SB->value();

	render_it();
}
//=================================================================
void 
TDP::exportPdf(const QString &filename)
{
    // file save dialog
    QString dialogcaption = "Export PDF";
    QString exportname = QFileDialog::getSaveFileName(this, dialogcaption, filename, "*.pdf");
    if (exportname.isEmpty()) return;
    if (QFileInfo(exportname).suffix().isEmpty())
            exportname.append(".pdf");

    // setup printer
    _printer->setOutputFormat(QPrinter::PdfFormat);
    _printer->setOutputFileName(exportname);

    // print pdf
    QPainter painter(_printer);
    for (int pagenum = 1; pagenum <= _pages; pagenum++) 
	{
		_gsv[pagenum-1]->render(&painter);

        if (pagenum < _pages) 
		{
            _printer->newPage();
        }
    }
}
//=================================================================
void 
TDP::showPage(int pagenum)
{
	int p = pagenum - 1;
	
	if (p < _gsv.size() && p >=0 )
	{
	    _ui.graphicsView->setScene( NULL );  // must be done
	    _ui.graphicsView->setScene( _gsv[p] );
	}

    _ui.graphicsView->ensureVisible( 0, 0, 10, 10);
}
//=================================================================
void
TDP::cleanup()
{
	// clean up gsv
	for (int i=0; i< _gsv.size(); ++i)
	{
		delete _gsv[i];
	}

	_gsv.clear();
}
//=================================================================
void
TDP::adjCellFont(QFont & cellFont)
{
   int fsz;
   fsz = cellFont.pixelSize();
   if (fsz == -1)
   {
	   fsz = cellFont.pointSize();
	   if (fsz != -1)
		   cellFont.setPointSize(fsz + _cellFontOffset);
   }
   else
   {
	   cellFont.setPixelSize(fsz + _cellFontOffset);			
   }
}
//=================================================================
void
TDP::calc_optimal()
{
	// calculate optimal sizes for font and page selections

//	_cellFontOffset;
//	_cellSizeOffset;

	QGraphicsScene * gs = new QGraphicsScene; // create scense for this page
		
	// get rect from printer based on paper selection and adjust it
    QRectF p_rect = _printer->paperRect();
    QRectF a_rect = QRectF(0, 0,
								p_rect.width() / _printer->logicalDpiX() * _sceneZoomFactor,
								p_rect.height() / _printer->logicalDpiY() * _sceneZoomFactor);
    gs->setSceneRect(a_rect);

	int w = gs->width();
	int h = gs->height();

	int csize = 0; int dx=0;
   	for (int c=0; c < _model->columnCount(); c++) // go thru all cols
   	{		
   		int logicalIndex = _tableView->horizontalHeader()->logicalIndex(c);

       	int aColSize = QVariant(_colSizes[logicalIndex].rawValue()).toInt() + _cellSizeOffset;	// adjusted col width
				
       	dx = csize + _leftMargin;

       	csize = csize + aColSize;
	}
	
	if (dx > w - 50)
	{
		delete gs;
		_cellSizeOffset -= 1;
		
		if ( abs(_cellSizeOffset) % 4 == 0 )
			_cellFontOffset -= 1;

		_ui.fontSize_SB->setValue(_cellFontOffset);
		_ui.cellSize_SB->setValue(_cellSizeOffset);
		
		calc_optimal();  // do it again		
	}
	else
		delete gs;
}
//=================================================================
void 
TDP::prep_gsv()
{
	if (_rows > 1000)
	{
		QMessageBox msgBox;
 		msgBox.setText("Trying to print " + QString::number(_rows) + " lines.");
 		msgBox.setInformativeText("System may not have enough memory. Are you sure?");
 		msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes );
 		msgBox.setDefaultButton(QMessageBox::No);
 		int ret = msgBox.exec();
 		if (ret == QMessageBox::No )
 		{
	 		return;
	 	}
	}

	QProgressDialog progress("Generating Preview...", "Ok", 0, _rows, this );
	
   	qreal dx=0, dy=0; // starting pos
   	QPen	pen(Qt::gray, 1);
   	QBrush	brush(Qt::gray,Qt::SolidPattern);
   	
	int	borderAdjust = _rowHeight / 5;
	int lineCount = 0;
    int rhAdjust = 0;
		
	bool done = false;
	int pageCount = 0;

	while (!done)
	{
		pageCount++;

		// initial y pos on page
    	dy = _topMargin + _headerSize + 18 + (_rowHeight * 2);
		
		QGraphicsScene * gs = new QGraphicsScene; // create scene for this page

		// prep header, footer and table header
		prep_Page(gs, pageCount);

	   	// Table rows ---------------------------------------------------------------------------
		while (lineCount <= _rows)
    	{
	    	progress.setValue(lineCount);
	        _qapp->processEvents();		
		
    		int csize = 0;

			dy += _rowHeight + rhAdjust;

	    	rhAdjust = 0; // row height adjustment is set to 0 - may be changed based on text layout in 0 col
			
			int yposAdj = 0;
        	for (int c=0; c < _model->columnCount(); c++) // go thru all cols
        	{		
        		int logicalIndex = _tableView->horizontalHeader()->logicalIndex(c);

            	int aColSize = QVariant(_colSizes[logicalIndex].rawValue()).toInt() + _cellSizeOffset;	// adjusted col width

            	QString itemText = _model->data(_model->index(lineCount, logicalIndex)).toString(); 	// get text to be printed

            	QFont cellFont = ((QStandardItemModel*)_model)->item(lineCount, c)->font();				// get font of this cell

				// for cell with regular fond move y down 3 pix
				if (! cellFont.bold() )
					yposAdj = 3;
				else
					yposAdj = 1;
									
				adjCellFont(cellFont);																	// adj font sizing
				
            	QGraphicsTextItem * gitem = new QGraphicsTextItem;							 			// create new item
            	gitem->setFont(cellFont);														 		// set font for this item

				// we may need to break it up into multiple lines
				// test first col only to find out height adjust
				if (c == 0)
				{
					if (itemText.length() > 32)
					{
						int numnls = itemText.length() / 23;

						for (int n=1; n <= numnls; ++n)
						{
							itemText.insert( itemText.indexOf(" ", n * 25), "\n");
						}

						rhAdjust = 15 * numnls;
					}
				}

            	gitem->setPlainText(itemText);

            	dx = csize + _leftMargin;

            	gitem->moveBy(dx, dy+yposAdj);				// place this item at X,Y 

				if ( _ui.grid_CB->isChecked() )
	          		gs->addRect(dx, dy + borderAdjust, aColSize, _rowHeight + rhAdjust); // cell frame

            	gs->addItem(gitem);  // add to scene

            	csize = csize + aColSize;

			}  // for cols

			lineCount++;

//			std::cout << lineCount << " " << pageCount << "\r" << std::flush;

			if (dy >= 700) break;  // page end
		}  // for lines

		
		_gsv.push_back(gs);  // save scene

		if (lineCount >= _rows)  // we are done
			done = true;

	} // while done

	_pages = _gsv.size();
}

//=================================================================
void
TDP::prep_Page(QGraphicsScene * gs, int pageNum)
{
	// get rect from printer based on paper selection and adjust it
    QRectF p_rect = _printer->paperRect();

//std::cout << p_rect.width() << " " << _printer->logicalDpiX() << " " << p_rect.width() / _printer->logicalDpiX() * _sceneZoomFactor << std::endl;
//std::cout << p_rect.height() << " " << _printer->logicalDpiY() << " " << p_rect.height() / _printer->logicalDpiY() * _sceneZoomFactor << std::endl;

    QRectF a_rect = QRectF(0, 0,
								p_rect.width() / _printer->logicalDpiX() * _sceneZoomFactor,
								p_rect.height() / _printer->logicalDpiY() * _sceneZoomFactor);
    gs->setSceneRect(a_rect);

	// main page rectangle outline
    gs->addRect(0, 0, gs->width(), gs->height(), QPen(Qt::gray, 1.0));

	prep_PageHeader(gs);
	prep_PageFooter(gs, pageNum);
	prep_TableHeader(gs);
}
//=================================================================
void 
TDP::prep_TableHeader(QGraphicsScene * gs)
{
	int csize = 0;
    for (int c = 0; c < _model->columnCount(); c++) 
    {
    	int logicalIndex = _tableView->horizontalHeader()->logicalIndex(c);
        QString txt = _model->headerData(logicalIndex, Qt::Horizontal, Qt::DisplayRole).toString();
        QGraphicsTextItem * item = new QGraphicsTextItem;

        item->setFont(_headerFont);

        item->setPlainText(txt);
        item->moveBy( csize + _leftMargin, _topMargin + _headerSize + (_spacing*2));
        csize = csize+QVariant(_colSizes[logicalIndex].rawValue()).toInt() + _cellSizeOffset;
        gs->addItem(item);
	}

}
//=================================================================
void 
TDP::prep_PageHeader(QGraphicsScene * gs)
{
    if (_headerSize > 0) 
    {
    	//line
        gs->addLine(_leftMargin,
					_headerSize + _topMargin,
					gs->width() - _rightMargin, 
					_headerSize + _topMargin,
					QPen(Qt::black, 1.0));

        // title
        QGraphicsTextItem * titleItem = new QGraphicsTextItem;
        titleItem->setFont(_titleFont);
        titleItem->setHtml(_headerText);
        int titleWidth = _titleFmt->width(_headerText);
        double xpos = (gs->width()/2) - (titleWidth/3);
        double ypos = (_headerSize/2) - (_titleFmt->height()/2);
        titleItem->moveBy(_leftMargin, _topMargin + 16);
        gs->addItem(titleItem);

        // std text
        QGraphicsTextItem * item = new QGraphicsTextItem;
        item->setFont(_font);
        item->setPlainText(_headerStdText);
        item->moveBy(_leftMargin, _topMargin);
        gs->addItem(item);
	}
}
//=================================================================
void 
TDP::prep_PageFooter(QGraphicsScene * gs, int pageCount)
{
    if (_footerSize > 0) 
    {
    	gs->addLine(_leftMargin,
					gs->height() - _footerSize - _bottomMargin, 
					gs->width() - _rightMargin, 
					gs->height() - _footerSize - _bottomMargin,
					QPen(Qt::black, 1.0));

        QGraphicsTextItem * item = new QGraphicsTextItem;
        item->setFont(_font);
//        	item->setPlainText(footerText + QString::number(pages) + " / " + QString::number(pageCount));
        item->setPlainText(_footerText + " " + QString::number(pageCount));
        item->moveBy(_leftMargin, gs->height() - _footerSize - _bottomMargin + _spacing);
        gs->addItem(item);
	}
}
//=================================================================
void 
TDP::prep_DocHeader(QGraphicsScene * gs, int startCol, int endCol)
{
	QRectF rect = _printer->paperRect();
	QRectF rectNew = QRectF(0,0,rect.width() / _printer->logicalDpiX() * _sceneZoomFactor, rect.height() / _printer->logicalDpiY() * _sceneZoomFactor);
	gs->setSceneRect(rectNew);

	// Table header -------------------------------------------------------------------------
	int csize=0;
	for (int c=0; c < _model->columnCount(); c++)
	{
		if ( c==0 || (c >= startCol && c<= endCol) ) // always do col 0
		{
    		int logicalIndex = _tableView->horizontalHeader()->logicalIndex(c);  						// get column index
    		QString txt = _model->headerData(logicalIndex, Qt::Horizontal, Qt::DisplayRole).toString();	// get text from col

    		QGraphicsTextItem * item = new QGraphicsTextItem;											// create new g text item

			// adjust font of this item
			int fsz;
			fsz = _headerFont.pixelSize();
			if (fsz == -1)
			{
				fsz = _headerFont.pointSize();
				if (fsz != -1)
					_headerFont.setPointSize(fsz + _cellFontOffset);
			}
			else
			{
				_headerFont.setPixelSize(fsz + _cellFontOffset);			
			}
    		item->setFont(_headerFont);

    		item->setPlainText(txt);	// set text
    		item->moveBy(csize + _leftMargin, _topMargin + _headerSize + (_spacing * 2)); // move pen
    		csize = csize + QVariant(_colSizes[logicalIndex].rawValue()).toInt() + _cellSizeOffset;  // adjust col

    		gs->addItem(item);  // add this item to scene
		}
	}
}
//=================================================================
void 
TDP::setGridMode(Grids p)
{
	_gridMode = p;
}
//=================================================================
void 
TDP::setRowLayoutMode(RowLayouts p)
{
	_rowLayoutMode = p;
}
//=================================================================
