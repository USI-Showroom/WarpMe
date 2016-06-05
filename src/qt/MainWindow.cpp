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
	QString fileName = QFileDialog::getOpenFileName(this, tr("Open Image"), "/home/jana", tr("Image Files (*.png *.jpg)"));
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