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


CameraCapture::CameraCapture(QObject *parent)
:QThread(parent), _started(false)
{
    if(_videoCapture.open(0))
    {
        _videoCapture.set(CV_CAP_PROP_FRAME_WIDTH,2304);
        _videoCapture.set(CV_CAP_PROP_FRAME_HEIGHT,1296);

        // _videoCapture.set(CV_CAP_PROP_FPS,1);
        // _videoCapture.set(CV_CAP_PROP_BRIGHTNESS,1296);
        // _videoCapture.set(CV_CAP_PROP_CONTRAST,1296);
        // _videoCapture.set(CV_CAP_PROP_SATURATION,1296);
        // _videoCapture.set(CV_CAP_PROP_HUE,1296);
        // _videoCapture.set(CV_CAP_PROP_GAIN,1296);
        // _videoCapture.set(CV_CAP_PROP_EXPOSURE,2);

        _started = true;
    }
}

void CameraCapture::run()
{
    cv::Mat frame;

    while(true)
    {
        if(!_started) continue;

        _videoCapture >> frame;

        if( frame.empty() ) continue;

        emit imageReady(QImage((uchar*) frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888));
    }
}

WebcamManager::WebcamManager(QWidget *parent)
:QDialog(parent), _ui(new Ui::WebcamManager),
_updateTimer(this), _soundPlayed(false), _caputure(this)
{
    _ui->setupUi(this);
    setModal(true);

    connect(&_caputure,SIGNAL(imageReady(const QImage)),this,SLOT(newImage(const QImage)));

    // _caputure.start();

    _clickSound.setSource(QUrl::fromLocalFile(":/sounds/camera"));

    _updateTimer.setInterval(30); // ms
    connect(&_updateTimer, SIGNAL(timeout()), this, SLOT(update())); 
    // _updateTimer.start();


    if(parent && parent->isFullScreen())
        showFullScreen();
    else
        resize(parent->size());

}

void WebcamManager::newImage(const QImage &img) 
{
 _img = img; 
}



WebcamManager::~WebcamManager()
{
    delete _ui;
// _videoCapture.close();
    _caputure.quit();
}

void WebcamManager::processCapturedImage(const QImage& img)
{
    const float h=img.width();
    const float w=img.height();

    float frameW, frameH;
    PaperConstants::Scale(h, w, frameH, frameW);

    // QTransform rot;
    // rot.rotate(90);
    // _img=img.mirrored(true,false).transformed(rot).copy(QRect((w-frameW)/2,(h-frameH)/2,frameW,frameH));
    // img.mirrored(false,true).save("asd0.png");

    _img=img.copy(QRect((h-frameH)/2, (w-frameW)/2,frameH,frameW));
    _img.save("asd.png");
}


void WebcamManager::paintEvent(QPaintEvent * event)
{
    if(_img.isNull()) return;




//     // const float scaleW = float(_preview->width()) / _ui->preview->height();
//     // const float scaleH = float(_preview->height()) / _ui->preview->width();
//     // const float scaling = 1.0f / std::max(scaleW, scaleH);

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

    qreal scale = m11;
    m11 = -m11;
    if(m31 > 0)
        m31 = 0;
    else
        m31 = _img.width() * scale;
    transform.setMatrix(m11, m12, m13, m21, m22, m23, m31, m32, m33);

//     transform.rotate(-90);

//     // transform.scale(scaling, scaling);
//     // transform.translate(-_preview->width()/2.0f, 0);

//     painter.save();
//     painter.setTransform(transform);
// //painter.drawPixmap(-_ui->preview->height() / 2 / scaling, (_ui->preview->height() - pixmap.height()) / 2, pixmap);
// // painter.drawPixmap(-_ui->preview->height() / 2 / scaling,0, pixmap);


    const int w=this->width();
    const int h=this->height();
    QImage tmp=_img.scaled(w,h,Qt::KeepAspectRatio);
    painter.drawImage(QPoint((w-tmp.width())/2,(h-tmp.height())/2),tmp);
    

//     painter.restore();

    painter.setPen(QColor(245, 128, 37));
    painter.setFont(QFont("Arial", 60));
    const int time=ceil((3000-_elapsed.elapsed())/1000.0);

    if(_elapsed.elapsed()>  2650 && !_soundPlayed){
        _clickSound.play();
        _soundPlayed=true;
    }

    if(time>0){
        painter.drawText(QRect(0,_ui->preview->height()/2-30, _ui->preview->width(),60),Qt::AlignCenter,QString::number(time));

        painter.end();
    }
    else{
        painter.end();

        _caputure.quit();
        _updateTimer.stop();
        processCapturedImage(_img);

        accept();
    }

    
}