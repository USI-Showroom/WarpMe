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
#include <QProcess>

static const bool portrait=false;


WebcamManager::WebcamManager(QWidget *parent)
:QDialog(parent), _ui(new Ui::WebcamManager),
_camera(NULL), _imageCapture(NULL), 
_isCapturingImage(false), _applicationExiting(false),
_preview(new QCameraViewfinder()), _updateTimer(this), _started(false), _soundPlayed(false),
_process(this)
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
	_imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);
    // connect(_imageCapture, SIGNAL(readyForCaptureChanged(bool)), this, SLOT(readyForCapture(bool)));
    connect(_imageCapture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(processCapturedImage(int,QImage)));
    connect(_imageCapture, SIGNAL(imageSaved(int,QString)), this, SLOT(imageSaved(int,QString)));
    connect(_imageCapture, SIGNAL(error(int,QCameraImageCapture::Error,QString)), this, SLOT(displayCaptureError(int,QCameraImageCapture::Error,QString)));

    _camera->setCaptureMode(QCamera::CaptureStillImage);
    _camera->start();

    if(parent && parent->isFullScreen())
        showFullScreen();
    
	//connect(&_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(execFinished(int, QProcess::ExitStatus)));

    // _camera->exposure();
    _updateTimer.setInterval(30); // ms
    connect(&_updateTimer, SIGNAL(timeout()), this, SLOT(update())); 
    _updateTimer.start();


    _clickSound.setSource(QUrl::fromLocalFile(":/sounds/camera"));
}

void WebcamManager::resizeEvent(QResizeEvent *event)
{
    _preview->resize(640,480);

	const float scaleW = float(_preview->width()) / _ui->preview->height();
	const float scaleH = float(_preview->height()) / _ui->preview->width();
	const float scaling = 1.0f / std::max(scaleW, scaleH);

	const float frameH = _preview->width()*scaling;
	const float frameW = frameH/PaperConstants::PAGE_HEIGHT*PaperConstants::PAGE_WIDTH;
    
    _ui->frame->setGeometry((_ui->preview->width() - frameW) / 2.0,(_ui->preview->height()-frameH)/2.0,frameW,frameH);
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
		if (!_elapsed.isValid())
			_elapsed.start();
        _started=true;
    }
	else if (state == QCamera::LoadedStatus)
	{
		QImageEncoderSettings settings = _imageCapture->encodingSettings();
		settings.setCodec("image/jpeg");
		settings.setQuality(QMultimedia::VeryHighQuality);
		//settings.setResolution(2304, 1296); //1536
		settings.setResolution(_camera->supportedViewfinderResolutions().back());
		_imageCapture->setEncodingSettings(settings);
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
    //QMessageBox::warning(this, tr("Image Capture Error"), errorString);
    _isCapturingImage = false;
}

void WebcamManager::processCapturedImage(int requestId, const QImage& img)
{
    const float h=img.width();
    const float w=img.height();

    float frameW, frameH;
    PaperConstants::Scale(w, h, frameW, frameH);
	
	QTransform rot;
	rot.rotate(90);
    _img=img.mirrored(true,false).transformed(rot).copy(QRect((w-frameW)/2,(h-frameH)/2,frameW,frameH));
}

void WebcamManager::takeImage()
{
    _isCapturingImage = true;
#ifdef WIN32
	if (_isCapturingImage) return;

	_process.start("C:\\users\\warpme\\Desktop\\webcamGrabber.bat");
	while (!_process.waitForFinished(-1));


	QString msg = _process.readAll();
	std::cout << msg.toStdString() << std::endl;

	processCapturedImage(-1, QImage("C:\\users\\warpme\\Desktop\\image.png"));
	accept();

#else
	_imageCapture->capture();
#endif
}

void WebcamManager::execFinished(int exitCode, QProcess::ExitStatus exitStatus)
 {
	processCapturedImage(-1, QImage("C:\\users\\warpme\\Desktop\\image.png"));
	accept();
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
	const float scaleW = float(_preview->width()) / _ui->preview->height();
	const float scaleH = float(_preview->height()) / _ui->preview->width();
	const float scaling = 1.0f / std::max(scaleW, scaleH);

    QPixmap pixmap(QSize(_preview->height()*scaling,_preview->width()*scaling));
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


    //if(portrait)
    {
        //transform.translate(pixmap.height()/2, pixmap.width()/2);
        transform.rotate(-90);
		//transform.translate(-pixmap.width(), pixmap.height()/3.0f);
    }

	{

		transform.scale(scaling, scaling);
		transform.translate(-_preview->width()/2.0f, 0);
	}

    painter.save();
    painter.setTransform(transform);
	//painter.drawPixmap(-_ui->preview->height() / 2 / scaling, (_ui->preview->height() - pixmap.height()) / 2, pixmap);
	painter.drawPixmap(-_ui->preview->height() / 2 / scaling,0, pixmap);
	painter.restore();

    painter.setPen(QColor(245, 128, 37));
    painter.setFont(QFont("Arial", 60));
    const int time=_started?ceil((3000-_elapsed.elapsed())/1000.0):3;

    if(_elapsed.elapsed()>  2650 && !_soundPlayed){
        _clickSound.play();
        _soundPlayed=true;
    }

    if(time>0)
        painter.drawText(QRect(0,_ui->preview->height()/2-30, _ui->preview->width(),60),Qt::AlignCenter,QString::number(time));
    else
        takeImage();

    painter.end();
}