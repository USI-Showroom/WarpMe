// Copyright (c) 2015 Teseo Schneider (Universita della Svizzera italiana)
// -------------------------------------------------------------------------------------------
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
// -----------------------------------------------------------------------

#include <iostream>


#include "MainWindow.hpp"

#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
QMainWindow(parent), _ui(new Ui::MainWindow)
{
	_ui->setupUi(this);
}


MainWindow::~MainWindow()
{
	delete _ui;
}

void MainWindow::on_actionOpen_image_triggered()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "/home", tr("Image Files (*.png *.jpg)"));
	_ui->mainView->setTexture(fileName);
}

void MainWindow::on_actionClear_triggered()
{
	_ui->mainView->clear();
}

void MainWindow::on_actionMorph_mode_triggered()
{
	_ui->mainView->toggleMorphMode();
}

void MainWindow::on_actionPreserve_bounday_triggered()
{
	_ui->mainView->togglePreseveBounday();
}