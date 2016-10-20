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


MainWindowTouch::MainWindowTouch(QWidget *parent) :
QMainWindow(parent), _ui(new Ui::MainWindowTouch)
{
	_ui->setupUi(this);
// #ifndef DEBUG
// 	showFullScreen();
// #endif

	_ui->morph->setText(_ui->mainView->morphMode()?"Draw":"Morph");
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
}

void MainWindowTouch::print()
{

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