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

#include <QPainter>


MainWindowTouch::MainWindowTouch(QWidget *parent) :
QMainWindow(parent), _ui(new Ui::MainWindowTouch), _printer(QPrinter::HighResolution), _logo(NULL)
{
	_ui->setupUi(this);
// #ifndef DEBUG
// 	showFullScreen();
// #endif

	_ui->morph->setText(_ui->mainView->morphMode()?"Draw":"Morph");


	std::cout<<"Using "<<_printer.printerName().toStdString()<<" printer"<<std::endl;

	_printer.setPageSize(QPrinter::A4);
	_printer.setCreator("Teseo Schneider @ USI");
	_printer.setDocName("Image morphi @ USI");
	_printer.setOrientation(QPrinter::Portrait);
	_printer.setPageMargins (15,15,15,15,QPrinter::Millimeter);
	_printer.setFullPage(false);

	// _printer.setOutputFileName("test.pdf");
	// _printer.setOutputFormat(QPrinter::PdfFormat);

	_printer.setOutputFileName("");
	_printer.setOutputFormat(QPrinter::NativeFormat);
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

void MainWindowTouch::print()
{
	QImage imgTmp(_ui->mainView->size(), QImage::Format_RGB32);

	{
		_ui->mainView->setDrawForPrinting(true);
		QPainter painter(&imgTmp);
		_ui->mainView->render(&painter);
		_ui->mainView->setDrawForPrinting(false);
	}


	QMatrix rot;
	rot.rotate(90);
	QImage img=imgTmp.transformed(rot);


	QPainter painter(&_printer);


	if(!_logo)
	{
		const QFileInfo file(":/img/logo");
		const QImage img = QImage(file.absoluteFilePath());
		_logo = new QImage(img.scaled(painter.device()->width(),painter.device()->height(),Qt::KeepAspectRatio));
	}
	
	const int w=painter.device()->width();
	const int h=painter.device()->height();

	imgTmp=img.scaled(w,h,Qt::KeepAspectRatio);

	painter.drawImage(QPoint((w-imgTmp.width())/2,(h-imgTmp.height())/2),imgTmp);
	painter.drawImage(QPoint(0,0),*_logo);
	painter.end();
}

void MainWindowTouch::getImgFromPhone()
{

}

void MainWindowTouch::selectImg()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "/home", tr("Image Files (*.png *.jpg)"));

	if(!fileName.isNull())
		_ui->mainView->setTexture(fileName);
}

void MainWindowTouch::clear()
{
	_ui->mainView->clear();
}

void MainWindowTouch::morph()
{
	
	_ui->mainView->toggleMorphMode();
	_ui->morph->setText(_ui->mainView->morphMode()?"Draw":"Morph");
}

void MainWindowTouch::preserveBoundayToggled(int)
{
	_ui->mainView->togglePreseveBounday();
}