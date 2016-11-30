// Copyright (c) 2015 Teseo Schneider (Universita della Svizzera italiana)
// -------------------------------------------------------------------------------------------
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
// -----------------------------------------------------------------------

#include "WebcamManager.hpp"
#include "ui_WebcamManager.h"
#include "PaperConstants.hpp"

#include <QCameraInfo>
#include <QMessageBox>
#include <QCloseEvent>
#include <QPainter>
#include <iostream>
#include <math.h>

static const bool portrait=false;


WebcamManager::WebcamManager(QWidget *parent)
:QDialog(parent), _ui(new Ui::WebcamManager),
_camera(NULL), _imageCapture(NULL), 
_isCapturingImage(false), _applicationExiting(false),
_preview(new QCameraViewfinder()), _updateTimer(this), _started(false), _soundPlayed(false)
{
    _ui->setupUi(this);
    setModal(true);

    QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    foreach (const QCameraInfo &cameraInfo, cameras) {
        _camera = new QCamera(cameraInfo);
        std::cout<<cameraInfo.description().toStdString()<<std::endl;
    }

    if(!_camera) return;


    _camera->setViewfinder(_preview);

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
    _updateTimer.setInterval(30); // ms
    connect(&_updateTimer, SIGNAL(timeout()), this, SLOT(update())); 
    _updateTimer.start();


    _clickSound.setSource(QUrl::fromLocalFile(":/sounds/camera"));
}

void WebcamManager::resizeEvent(QResizeEvent *event)
{
    const float w=_ui->preview->width();
    const float h=_ui->preview->height();

    _preview->resize(w,h);

    float frameW, frameH;
    PaperConstants::Scale(w, h, frameW, frameH);

    _ui->frame->setGeometry((w-frameW)/2,(h-frameH)/2,frameW,frameH);
}




WebcamManager::~WebcamManager()
{
    delete _ui;
    delete _camera;
    delete _imageCapture;
    delete _preview;
}

void WebcamManager::updateCameraStatus(QCamera::Status state)
{
    if(state==QCamera::ActiveStatus){
        _elapsed.start();
        _started=true;
    }
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
    const float w=img.width();
    const float h=img.height();

    float frameW, frameH;
    PaperConstants::Scale(w, h, frameW, frameH);

    _img=img.mirrored(true,false).copy(QRect((w-frameW)/2,(h-frameH)/2,frameW,frameH));
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
        _updateTimer.stop();
        event->accept();
    }
}



void WebcamManager::paintEvent(QPaintEvent * event)
{
    QPixmap pixmap(_ui->preview->size());
    _preview->render(&pixmap);

    QPainter painter(this);

    QTransform transform;
    qreal m11 = transform.m11();    // Horizontal scaling
    qreal m12 = transform.m12();    // Vertical shearing
    qreal m13 = transform.m13();    // Horizontal Projection
    qreal m21 = transform.m21();    // Horizontal shearing
    qreal m22 = transform.m22();    // vertical scaling
    qreal m23 = transform.m23();    // Vertical Projection
    qreal m31 = transform.m31();    // Horizontal Position (DX)
    qreal m32 = transform.m32();    // Vertical Position (DY)
    qreal m33 = transform.m33();    // Addtional Projection Factor

    // We need this in a minute
    qreal scale = m11;

    // Horizontal flip
    m11 = -m11;

    // Re-position back to origin
    if(m31 > 0)
        m31 = 0;
    else
        m31 = pixmap.size().width() * scale;

    // Write back to the matrix
    transform.setMatrix(m11, m12, m13, m21, m22, m23, m31, m32, m33);


    if(portrait)
    {
        transform.translate(pixmap.width()/2, pixmap.height()/2);
        transform.rotate(90);
        transform.translate(-pixmap.width()/2, -pixmap.height()/2);
    }
    painter.save();
    painter.setTransform(transform);
    painter.drawPixmap(0,0,pixmap);
    painter.restore();

    painter.setPen(QColor(245, 128, 37));
    painter.setFont(QFont("Arial", 60));
    const int time=_started?ceil((3000-_elapsed.elapsed())/1000.0):3;

    if(_elapsed.elapsed()>  2650 && !_soundPlayed){
        _clickSound.play();
        _soundPlayed=true;
    }

    if(time>0)
        painter.drawText(QRect(0,pixmap.height()/2-30,pixmap.width(),60),Qt::AlignCenter,QString::number(time));
    else
        takeImage();

    painter.end();
}