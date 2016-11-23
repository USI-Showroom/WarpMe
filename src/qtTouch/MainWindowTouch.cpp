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

#include <QPainter>


static const int SMALL_BTN_SIZE = 40; //150
static const int LARGE_BTN_SIZE = 50; //190
static const int BTN_OFFSET = 10; //40


MainWindowTouch::MainWindowTouch(QWidget *parent) :
QMainWindow(parent), _ui(new Ui::MainWindowTouch), _printer(QPrinter::HighResolution), _logo(NULL)
{
	_ui->setupUi(this);
// #ifndef DEBUG
// 	showFullScreen();
// #endif

	std::cout<<"Using "<<_printer.printerName().toStdString()<<" printer"<<std::endl;

	_printer.setPageSize(QPageSize(QSizeF(PAGE_WIDTH,PAGE_HEIGHT),QPageSize::Inch,"photo",QPageSize::ExactMatch));//QPrinter::A4);
	_printer.setResolution(PAGE_DPI);
	_printer.setCreator("Teseo Schneider @ USI");
	_printer.setDocName("Image morphi @ USI");
	_printer.setOrientation(QPrinter::Portrait);
	_printer.setPageMargins (0,0,0,0,QPrinter::Millimeter);
	_printer.setFullPage(true);

	// _printer.setOutputFileName("test.pdf");
	// _printer.setOutputFormat(QPrinter::PdfFormat);

	_printer.setOutputFileName("");
	_printer.setOutputFormat(QPrinter::NativeFormat);


	


	_ui->webcamImg->resize(LARGE_BTN_SIZE,LARGE_BTN_SIZE);
	_ui->webcamImg->setIconSize(QSize(LARGE_BTN_SIZE,LARGE_BTN_SIZE));

	_ui->print->resize(SMALL_BTN_SIZE,SMALL_BTN_SIZE);
	_ui->print->setIconSize(QSize(SMALL_BTN_SIZE,SMALL_BTN_SIZE));

	_ui->sendEmail->resize(SMALL_BTN_SIZE,SMALL_BTN_SIZE);
	_ui->sendEmail->setIconSize(QSize(SMALL_BTN_SIZE,SMALL_BTN_SIZE));

	_ui->facebookShare->resize(SMALL_BTN_SIZE,SMALL_BTN_SIZE);
	_ui->facebookShare->setIconSize(QSize(SMALL_BTN_SIZE,SMALL_BTN_SIZE));

}

void MainWindowTouch::resizeEvent(QResizeEvent * event)
{
	const QSize &size = this->size();

	
	const float h=float(size.width())/float(PAGE_WIDTH)*PAGE_HEIGHT;
	_ui->mainView->resize(size.width(),h);
	_ui->mainView->move(0,size.height()-h+1);

	{
		const QSize &btnSize=_ui->webcamImg->size();
		_ui->webcamImg->move(size.width()/2-btnSize.width()/2,size.height()-btnSize.height());
	}

	{
		const QSize &btnSize=_ui->print->size();
		_ui->print->move(size.width()-btnSize.width()*3-2*BTN_OFFSET,size.height()-btnSize.height());
	}

	{
		const QSize &btnSize=_ui->sendEmail->size();
		_ui->sendEmail->move(size.width()-btnSize.width()*2-BTN_OFFSET,size.height()-btnSize.height());
	}

	{
		const QSize &btnSize=_ui->facebookShare->size();
		_ui->facebookShare->move(size.width()-btnSize.width(),size.height()-btnSize.height());
	}

	{
		_ui->usi_logo->resize(size.width(),size.width()/1242.0f*200.0f);
		_ui->usi_logo->move(0,0);

	}

	QWidget::resizeEvent(event);
}


void MainWindowTouch::openWebcamPreview()
{
	WebcamManager wbm(this);
	if(wbm.exec())
	{
		_ui->mainView->setTexture(wbm.image());
	}
}


MainWindowTouch::~MainWindowTouch()
{
	delete _ui;
	delete _logo;
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

	const float scaleW=w/PAGE_WIDTH;
	const float scaleH=h/PAGE_HEIGHT;

	const float scale=std::min(scaleW,scaleH);

	const float frameW=PAGE_WIDTH*scale;
	const float frameH=PAGE_HEIGHT*scale;

	img=imgTmp.copy(QRect((w-frameW)/2,(h-frameH)/2,frameW,frameH));
}

void MainWindowTouch::print()
{
	QImage img;
	getImage(img);


	QPainter painter(&_printer);


	if(!_logo)
	{
		const QFileInfo file(":/img/logo");
		const QImage img = QImage(file.absoluteFilePath());
		_logo = new QImage(img.scaled(painter.device()->width(),painter.device()->height(),Qt::KeepAspectRatio));
	}
	
	const int w=painter.device()->width();
	const int h=painter.device()->height();

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
	FacebookManager fbm(img,this);
	fbm.exec();
}



void MainWindowTouch::preserveBoundayToggled(int)
{
	_ui->mainView->togglePreseveBounday();
}