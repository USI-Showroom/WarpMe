// Copyright (c) 2015 Teseo Schneider (Universita della Svizzera italiana)
// -------------------------------------------------------------------------------------------
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
// -----------------------------------------------------------------------

#include <iostream>

#include "MainWindowTouch.hpp"
#include "ui_MainWindowTouch.h"

#include "WebcamManager.hpp"
#include "MailManager.hpp"
#include "FacebookManager.hpp"
#include "NewPicture.hpp"
#include "PaperConstants.hpp"

#include <QPainter>
#include <QPrinterInfo>

//static const int SMALL_BTN_SIZE = 40;
//static const int LARGE_BTN_SIZE = 50;
//static const int BTN_OFFSET = 10;


static const int SMALL_BTN_SIZE = 80;
static const int LARGE_BTN_SIZE = 100;
static const int BTN_OFFSET = 20;


MainWindowTouch::MainWindowTouch(QWidget *parent) :
QMainWindow(parent), _ui(new Ui::MainWindowTouch), _printer(QPrinter::HighResolution), _logo(NULL), _fbm(this)
{


	_ui->setupUi(this);
// #ifndef DEBUG
	   showFullScreen();
// #endif

	QList<QPrinterInfo> infos = QPrinterInfo::availablePrinters();

	QPageSize size;
	bool found = false;
	for (int i = 0; i < infos.size(); i++)
	{
		//int i = 0;
		const QPrinterInfo &info = infos.at(i);
		//std::cout<<"printer name= "<<info.printerName().toStdString()<<std::endl;

		QList<QPageSize> sizes = info.supportedPageSizes();
		for (int j = 0; j < sizes.size(); j++)
		{
			QPageSize sz = sizes.at(j);
			QSizeF page_size = sz.definitionSize();
			// std::cout<<"page size = "<<sz.name().toStdString()<<" ("<<page_size.width()<<" x "<< page_size.height()<<")"<<std::endl;

			if(fabs(PaperConstants::PAGE_WIDTH() - page_size.width()) < 1e-10 && fabs(PaperConstants::PAGE_HEIGHT() - page_size.height()) < 1e-10)
			{
				size = sz;
				found = true;
			}
		}
	}

	if(!found) 
	{
		std::cerr<<"Unable to locate correct paper size"<<std::endl;
	}

	std::cout<<"Using "<<_printer.printerName().toStdString()<<" printer"<<std::endl;

	_printer.setPageSize(size);//QPrinter::A4);
	_printer.setResolution(PaperConstants::PAGE_DPI);
	_printer.setCreator("WarpMe @ USI");
	_printer.setDocName("WarpMe @ USI");
	_printer.setOrientation(QPrinter::Portrait);
	_printer.setPageMargins (0,0,0,0,QPrinter::Millimeter);
	_printer.setFullPage(true);

	// _printer.setOutputFileName("test.pdf");
	// _printer.setOutputFormat(QPrinter::PdfFormat);

	_printer.setOutputFileName("");
	_printer.setOutputFormat(QPrinter::NativeFormat);


	// const QPageLayout &tmp = _printer.pageLayout();
	// std::cout<<tmp.pageSize().size(QPageSize::Point).width()<<" "<<tmp.pageSize().size(QPageSize::Point).height()<<std::endl;
	// std::cout<<size.size(QPageSize::Point).width()<<" "<<size.size(QPageSize::Point).height()<<std::endl;
	// std::cout<<tmp.mode()<<std::endl;
	// std::cout<<sizeF.width()<<" "<<sizeF.height()<<std::endl;

	


	_ui->webcamImg->resize(LARGE_BTN_SIZE,LARGE_BTN_SIZE);
	_ui->webcamImg->setIconSize(QSize(LARGE_BTN_SIZE,LARGE_BTN_SIZE));

	_ui->print->resize(SMALL_BTN_SIZE,SMALL_BTN_SIZE);
	_ui->print->setIconSize(QSize(SMALL_BTN_SIZE,SMALL_BTN_SIZE));

	_ui->sendEmail->resize(SMALL_BTN_SIZE,SMALL_BTN_SIZE);
	_ui->sendEmail->setIconSize(QSize(SMALL_BTN_SIZE,SMALL_BTN_SIZE));

	_ui->facebookShare->resize(SMALL_BTN_SIZE,SMALL_BTN_SIZE);
	_ui->facebookShare->setIconSize(QSize(SMALL_BTN_SIZE,SMALL_BTN_SIZE));

	_ui->showHidePoly->resize(SMALL_BTN_SIZE,SMALL_BTN_SIZE);
	_ui->showHidePoly->setIconSize(QSize(SMALL_BTN_SIZE,SMALL_BTN_SIZE));

	_wbm = NULL;
}


void MainWindowTouch::resizeEvent(QResizeEvent * event)
{
	const QSize &size = this->size();

	const int voffset = 20;
	const int h = float(size.width()) / PaperConstants::PAGE_WIDTH()*PaperConstants::PAGE_HEIGHT();
	const int editH = size.height() - h;
	_ui->mainView->resize(size.width(), h);
	_ui->mainView->move(0, editH);

	_ui->webcamImg->move(size.width() / 2 - LARGE_BTN_SIZE / 2, size.height() - LARGE_BTN_SIZE-voffset);

	_ui->print->move(size.width() - SMALL_BTN_SIZE * 3 - 2 * BTN_OFFSET, size.height() - SMALL_BTN_SIZE-voffset);
	_ui->sendEmail->move(size.width() - SMALL_BTN_SIZE * 2 - BTN_OFFSET, size.height() - SMALL_BTN_SIZE-voffset);
	_ui->facebookShare->move(size.width() - SMALL_BTN_SIZE, size.height() - SMALL_BTN_SIZE-voffset);


	_ui->showHidePoly->move(0, size.height() - SMALL_BTN_SIZE-voffset);

	{
		const int logoH = size.width() / 2000.0f*382.0f;
		_ui->usi_logo->resize(size.width(), logoH);
		_ui->usi_logo->move(0, editH - logoH);
	}

	QWidget::resizeEvent(event);
}


void MainWindowTouch::showHidePolyPressed()
{
	_ui->showHidePoly->setIcon(QIcon(":/icons/show"));
	_ui->mainView->setDrawForPrinting(true);
	_ui->mainView->update();
}

void MainWindowTouch::showHidePolyReleased()
{
	_ui->showHidePoly->setIcon(QIcon(":/icons/hide"));
	_ui->mainView->setDrawForPrinting(false);
	_ui->mainView->update();
}


void MainWindowTouch::openWebcamPreview()
{
	if(!_wbm)
		_wbm = new WebcamManager(this);


	_wbm->startCounter();
	if(_wbm->exec())
	{
		_ui->mainView->setTexture(_wbm->image());
	}
}


MainWindowTouch::~MainWindowTouch()
{
	delete _ui;
	delete _logo;
	delete _wbm;
}

void MainWindowTouch::getImage(QImage &img)
{
	QImage imgTmp(_ui->mainView->size(), QImage::Format_RGB32);

	
	_ui->mainView->setDrawForPrinting(true);
	QPainter painter(&imgTmp);
	_ui->mainView->render(&painter);
	_ui->mainView->setDrawForPrinting(false);
	_ui->mainView->update();
	

	const float w=_ui->mainView->width();
	const float h=_ui->mainView->height();

	float frameW, frameH;
	PaperConstants::Scale(w, h, frameW, frameH);

	img=imgTmp.copy(QRect((w-frameW)/2,(h-frameH)/2,frameW,frameH));
}

void MainWindowTouch::print()
{
	QImage img;
	getImage(img);


	QPainter painter(&_printer);


	if(!_logo)
	{
		const QFileInfo file(QString::fromStdString(PaperConstants::PRINT_LAYER()));
		const QImage img = QImage(file.absoluteFilePath());
		_logo = new QImage(img.scaled(painter.device()->width(),painter.device()->height(),Qt::KeepAspectRatio));
	}
	
	const int w=painter.device()->width();
	const int h=painter.device()->height();

	std::cout<<w<<std::endl;
	std::cout<<h<<std::endl;

	QImage imgTmp=img.scaled(w,h,Qt::KeepAspectRatio);

	painter.drawImage(QPoint((w-imgTmp.width())/2,(h-imgTmp.height())/2),imgTmp);
	painter.drawImage(QPoint(0,0),*_logo);
	painter.end();
	
}

void MainWindowTouch::sendMail()
{
	QImage img;
	getImage(img);
	MailManager mm(img,this);
	mm.exec();
}

void MainWindowTouch::facebookShare()
{
	QImage img;
	getImage(img);
	_fbm.setImage(&img);
	_fbm.exec();
}



void MainWindowTouch::preserveBoundayToggled(int)
{
	_ui->mainView->togglePreseveBounday();
}

void MainWindowTouch::keyReleaseEvent(QKeyEvent * event)
{
	if(event->key()==Qt::Key_Escape)
		QApplication::quit();
	else
		QWidget::keyReleaseEvent(event);
}