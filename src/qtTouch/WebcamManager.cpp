// Copyright (c) 2015 Teseo Schneider (Universita della Svizzera italiana)
// -------------------------------------------------------------------------------------------
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
// -----------------------------------------------------------------------

#include "WebcamManager.hpp"
#include "ui_WebcamManager.h"
#include "MainWindowTouch.hpp"

#include <QCameraInfo>
#include <QMessageBox>
#include <QCloseEvent>
#include <iostream>



WebcamManager::WebcamManager(QWidget *parent)
:QDialog(parent), _ui(new Ui::WebcamManager),
_camera(NULL), _imageCapture(NULL), 
_isCapturingImage(false), _applicationExiting(false)
{
    _ui->setupUi(this);
    _ui->takePictureBtn->setEnabled(true);
    setModal(true);

    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    foreach (const QCameraInfo &cameraInfo, cameras) {
        _camera = new QCamera(cameraInfo);
        std::cout<<cameraInfo.description().toStdString()<<std::endl;
    }

    if(!_camera) return;


    _camera->setViewfinder(_ui->preview);

    connect(_camera, SIGNAL(statusChanged(QCamera::Status)), this, SLOT(updateCameraStatus(QCamera::Status)));
    // connect(_camera, SIGNAL(error(QCamera::Error)), this, SLOT(displayCameraError()));
    connect(_camera, SIGNAL(lockStatusChanged(QCamera::LockStatus,QCamera::LockChangeReason)),this, SLOT(updateLockStatus(QCamera::LockStatus,QCamera::LockChangeReason)));

    _imageCapture = new QCameraImageCapture(_camera);


    // connect(_imageCapture, SIGNAL(readyForCaptureChanged(bool)), this, SLOT(readyForCapture(bool)));
    connect(_imageCapture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(processCapturedImage(int,QImage)));
    connect(_imageCapture, SIGNAL(imageSaved(int,QString)), this, SLOT(imageSaved(int,QString)));
    connect(_imageCapture, SIGNAL(error(int,QCameraImageCapture::Error,QString)), this, SLOT(displayCaptureError(int,QCameraImageCapture::Error,QString)));

    _camera->setCaptureMode(QCamera::CaptureStillImage);
    _camera->start();

    if(parent && parent->isFullScreen())
        showFullScreen();
    

    // _camera->exposure();
}

void WebcamManager::resizeEvent(QResizeEvent *event)
{
    const float w=_ui->preview->width();
    const float h=_ui->preview->height();

    const float scaleW=w/MainWindowTouch::PAGE_WIDTH;
    const float scaleH=h/MainWindowTouch::PAGE_HEIGHT;

    const float scale=std::min(scaleW,scaleH);
    
    const float frameW=MainWindowTouch::PAGE_WIDTH*scale;
    const float frameH=MainWindowTouch::PAGE_HEIGHT*scale;

    _ui->frame->setGeometry((w-frameW)/2,(h-frameH)/2,frameW,frameH);
}




WebcamManager::~WebcamManager()
{
    delete _ui;
    delete _camera;
    delete _imageCapture;
}

void WebcamManager::updateCameraStatus(QCamera::Status state)
{
    // std::cout<<"here"<<state<<std::endl;
}

void WebcamManager::updateLockStatus(QCamera::LockStatus ls,QCamera::LockChangeReason lcr)
{
    // std::cout<<"there"<<ls<<" "<<lcr<<std::endl;
}


void WebcamManager::readyForCapture(bool ready)
{
    // _ui->takePictureBtn->setEnabled(ready);
}

void WebcamManager::imageSaved(int id, const QString &fileName)
{
    Q_UNUSED(id);
    Q_UNUSED(fileName);

    _isCapturingImage = false;
// if (_applicationExiting)
    accept();
}

void WebcamManager::displayCaptureError(int id, const QCameraImageCapture::Error error, const QString &errorString)
{
    Q_UNUSED(id);
    Q_UNUSED(error);
    QMessageBox::warning(this, tr("Image Capture Error"), errorString);
    _isCapturingImage = false;
}

void WebcamManager::processCapturedImage(int requestId, const QImage& img)
{
    const float scale=std::max(img.width()/float(_ui->preview->width()),img.height()/float(_ui->preview->height()));
    const QRect area=_ui->frame->geometry();
    _img=img.copy(QRect(area.x()*scale,area.y()*scale,area.width()*scale,area.height()*scale));
}

void WebcamManager::takeImage()
{
    _isCapturingImage = true;
    _imageCapture->capture();
}


void WebcamManager::closeEvent(QCloseEvent *event)
{
    if (_isCapturingImage) {
        setEnabled(false);
        _applicationExiting = true;
        event->ignore();
    } else {
        event->accept();
    }
}