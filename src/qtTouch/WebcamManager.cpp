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
#include <QPainterPath>
#include <iostream>
#include <math.h>
#include <QProcess>

using namespace cv;

int roundUp(int numToRound, int multiple)
{
    if (multiple == 0)
        return numToRound;

    int remainder = abs(numToRound) % multiple;

    if (remainder == 0)
        return numToRound;

    if (numToRound < 0)
        return -(abs(numToRound) - remainder);

    return numToRound + multiple - remainder;
}

Point CameraCapture::faceFromEyes(Point &priorCenter, const Mat &face)
{
    std::vector<Rect> eyes;
    int avg_x = 0;
    int avg_y = 0;

    // Try to detect eyes, if no face is found
    _eyesCascade.detectMultiScale(face, eyes, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

    // Iterate over eyes
    for (int j = 0; j < eyes.size(); ++j)
    {

        // centerpoint of eyes
        Point eye_center(priorCenter.x + eyes[j].x + eyes[j].width / 2, priorCenter.y + eyes[j].y + eyes[j].height / 2);

        // Average center of eyes
        avg_x += eye_center.x;
        avg_y += eye_center.y;
    }

    // Use average location of eyes
    if (eyes.size() > 0)
    {
        priorCenter.x = avg_x / eyes.size();
        priorCenter.y = avg_y / eyes.size();
    }

    return priorCenter;
}

QRectF CameraCapture::detectFace(Mat &frame)
{
    cv::Point priorCenter = _center;

    std::vector<Rect> faces;

    Mat frameGray, frameLab;

    int h = frame.size().height - 1;
    int w = frame.size().width - 1;

    int minNeighbors = 2;

    bool faceFound = false;

    cvtColor(frame, frameGray, COLOR_BGR2GRAY); // Convert to gray
    equalizeHist(frameGray, frameGray);         // Equalize histogram

    // Detect face with open source cascade
    _faceCascade.detectMultiScale(frameGray, faces, 1.1, minNeighbors, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));

    for (int i = 0; i < faces.size(); ++i)
    {
        // Find center of face
        Point center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);

        // Generate width and height of face, round to closest 1/4 of frame height
        h = roundUp(faces[i].height, frame.size().height / 4);
        w = 3 * h / 5;

        // If priorCenter not yet initialized, initialize
        if (priorCenter.x == 0)
        {
            priorCenter = center;
            faceFound = true;
            break;
        }

        // Check to see if it's probably the same user
        if (abs(center.x - priorCenter.x) < frame.size().width / 6 && abs(center.y - priorCenter.y) < frame.size().height / 6)
        {
            // Check to see if the user moved enough to update position
            if (abs(center.x - priorCenter.x) < 7 && abs(center.y - priorCenter.y) < 7)
            {
                center = priorCenter;
            }

            // Smooth new center compared to old center
            center.x = (center.x + 2 * priorCenter.x) / 3;
            center.y = (center.y + 2 * priorCenter.y) / 3;
            priorCenter = center;

            // output frame of only face
            faceFound = true;
            break; // exit, primary users face probably found
        }
    }

    QRectF res;

    const float fw = frame.cols;
    const float fh = frame.rows;

    if (faceFound)
        res = QRectF((priorCenter.x - w / 2.0f) / fw, (priorCenter.y - h / 2.0f) / fh, w / fw, h / fh);
    //     // Findface from eyes
    //     Rect r(priorCenter.x, priorCenter.y, w, h);

    //     if(priorCenter.x + w > frameGray.size().width - 2 && priorCenter.y + h > frameGray.size().height - 2)
    //     {
    //         priorCenter = faceFromEyes(priorCenter, frameGray(r));
    //     }
    // }

    // if(output.size().width > 2)
    // {
    //     ellipse(frame, priorCenter, Size(w/2, h/2),
    //         0, 0, 360, Scalar( 255, 0, 255 ), 4, 8, 0);
    // }

    _center = priorCenter;

    return res;
}

CameraCapture::CameraCapture(QObject *parent)
    : QThread(parent), _started(false), _center(0, 0)
{
    _videoCapture = VideoCapture(0, cv::CAP_DSHOW);

    if (!_videoCapture.isOpened())
    {
        std::cerr << "Unable to open video stream" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    if (!_faceCascade.load("../src/resource/opencv/haarcascade_frontalface_alt.xml"))
        std::cerr << "Unable to load face cascade" << std::endl; // load face classifiers
    if (!_eyesCascade.load("../src/resource/opencv/haarcascade_eye_tree_eyeglasses.xml"))
        std::cerr << "Unable to load eyes cascade" << std::endl; // load eye classifiers

    _videoCapture.set(CAP_PROP_FRAME_WIDTH, 2304 / 1.1);
    _videoCapture.set(CAP_PROP_FRAME_HEIGHT, 1296 / 1.1);

    // _videoCapture.set(CV_CAP_PROP_FPS,1);
    // _videoCapture.set(CV_CAP_PROP_BRIGHTNESS,1296);
    // _videoCapture.set(CV_CAP_PROP_CONTRAST,1296);
    // _videoCapture.set(CV_CAP_PROP_SATURATION,1296);
    // _videoCapture.set(CV_CAP_PROP_HUE,1296);
    // _videoCapture.set(CV_CAP_PROP_GAIN,1296);
    // _videoCapture.set(CV_CAP_PROP_EXPOSURE,2);

    _started = true;
}

void CameraCapture::run()
{
    cv::Mat frame;
    cv::Mat rgbFrame;

    while (true)
    {
        if (!_started)
            continue;

        _videoCapture >> frame;

        if (frame.empty())
            continue;

        QRectF rect; // = detectFace(frame);

        cv::cvtColor(frame, rgbFrame, COLOR_BGR2RGB);
        emit imageReady(QImage((uchar *)rgbFrame.data, rgbFrame.cols, rgbFrame.rows, rgbFrame.step, QImage::Format_RGB888), rect);
    }
}
void CameraCapture::capture(QImage &out)
{
    cv::Mat frame;

    _videoCapture.set(CAP_PROP_FRAME_WIDTH, 2304);
    _videoCapture.set(CAP_PROP_FRAME_HEIGHT, 1296);

    while (frame.empty())
        _videoCapture >> frame;

    _videoCapture.set(CAP_PROP_FRAME_WIDTH, 2304 / 2);
    _videoCapture.set(CAP_PROP_FRAME_HEIGHT, 1296 / 2);

    QImage img((uchar *)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);

    QTransform transform;
    qreal m11 = transform.m11(); // Horizontal scaling
    qreal m12 = transform.m12(); // Vertical shearing
    qreal m13 = transform.m13(); // Horizontal Projection
    qreal m21 = transform.m21(); // Horizontal shearing
    qreal m22 = transform.m22(); // vertical scaling
    qreal m23 = transform.m23(); // Vertical Projection
    qreal m31 = transform.m31(); // Horizontal Position (DX)
    qreal m32 = transform.m32(); // Vertical Position (DY)
    qreal m33 = transform.m33(); // Addtional Projection Factor

    qreal scale = m11;
    m11 = -m11;
    if (m31 > 0)
        m31 = 0;
    else
        m31 = img.width() * scale;
    transform.setMatrix(m11, m12, m13, m21, m22, m23, m31, m32, m33);

    transform.rotate(-90);

    out = img.transformed(transform);
}

WebcamManager::WebcamManager(QWidget *parent)
    : QDialog(parent), _ui(new Ui::WebcamManager),
      _updateTimer(this), _soundPlayed(false), _clickSound(this), _caputure(this)
{
    _ui->setupUi(this);
    setModal(true);

    connect(&_caputure, SIGNAL(imageReady(const QImage &, const QRectF &)), this, SLOT(newImage(const QImage &, const QRectF &)));

    // _caputure.start();

    _clickSound.setMedia(QUrl("qrc:/sounds/camera"));

    _updateTimer.setInterval(30); // ms
    connect(&_updateTimer, SIGNAL(timeout()), this, SLOT(update()));
    // _updateTimer.start();

    if (parent && parent->isFullScreen())
        showFullScreen();
    else
        resize(parent->size());
}

void WebcamManager::newImage(const QImage &img, const QRectF &face)
{
    QTransform transform;
    qreal m11 = transform.m11(); // Horizontal scaling
    qreal m12 = transform.m12(); // Vertical shearing
    qreal m13 = transform.m13(); // Horizontal Projection
    qreal m21 = transform.m21(); // Horizontal shearing
    qreal m22 = transform.m22(); // vertical scaling
    qreal m23 = transform.m23(); // Vertical Projection
    qreal m31 = transform.m31(); // Horizontal Position (DX)
    qreal m32 = transform.m32(); // Vertical Position (DY)
    qreal m33 = transform.m33(); // Addtional Projection Factor

    qreal scale = m11;
    m11 = -m11;
    if (m31 > 0)
        m31 = 0;
    else
        m31 = img.width() * scale;
    transform.setMatrix(m11, m12, m13, m21, m22, m23, m31, m32, m33);

    transform.rotate(-90);

    _img = img.transformed(transform);
    if (!_face.isNull())
        _face = face;
}

WebcamManager::~WebcamManager()
{
    delete _ui;
    // _videoCapture.close();
    _caputure.quit();
}

void WebcamManager::processCapturedImage(const QImage &img)
{
    const float h = img.width();
    const float w = img.height();

    float frameW, frameH;
    PaperConstants::Scale(h, w, frameH, frameW);

    // QTransform rot;
    // rot.rotate(90);
    // _img=img.mirrored(true,false).transformed(rot).copy(QRect((w-frameW)/2,(h-frameH)/2,frameW,frameH));
    // img.mirrored(false,true).save("asd0.png");

    _img = img.copy(QRect((h - frameH) / 2, (w - frameW) / 2, frameH, frameW));
    //_img.save("asd.png");
}

void WebcamManager::paintEvent(QPaintEvent *event)
{
    if (_img.isNull())
        return;

    //     // const float scaleW = float(_preview->width()) / _ui->preview->height();
    //     // const float scaleH = float(_preview->height()) / _ui->preview->width();
    //     // const float scaling = 1.0f / std::max(scaleW, scaleH);

    QPainter painter(this);

    //     transform.rotate(-90);

    //     // transform.scale(scaling, scaling);
    //     // transform.translate(-_preview->width()/2.0f, 0);

    //     painter.save();
    //     painter.setTransform(transform);
    // //painter.drawPixmap(-_ui->preview->height() / 2 / scaling, (_ui->preview->height() - pixmap.height()) / 2, pixmap);
    // // painter.drawPixmap(-_ui->preview->height() / 2 / scaling,0, pixmap);

    const int w = this->width();
    const int h = this->height();
    QImage tmp = _img.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QPoint offset((w - tmp.width()) / 2, (h - tmp.height()) / 2);
    painter.drawImage(offset, tmp);

    //QRect ellipse(_face.x()*tmp.height()+offset.x(),_face.y()*tmp.width()+offset.y(),_face.width()*tmp.height(),_face.height()*tmp.width());
    //std::cout<<ellipse.x()<<" "<<ellipse.y()<<" "<<ellipse.width()<<" "<<ellipse.height()<<std::endl;
    //painter.drawEllipse(ellipse);

    //     painter.restore();

    const int centerx = w / 2;
    const int centery = h / 2;

    QRect ellipse(centerx - 0.5 * 0.5 * w,
                  centery - 0.5 * 0.5 * h - 0.4 * 0.5 * h,
                  0.5 * w,
                  0.5 * h);

    painter.setPen(QPen(QColor(245, 128, 37), 3, Qt::SolidLine, Qt::SquareCap, Qt::BevelJoin));
    painter.drawEllipse(ellipse);

    {
        qreal offset = w - 30;
        qreal height = centery - 0.2 * 0.5 * h;

        qreal tipLenght = 0.25 * 0.5 * 0.5 * w;
        qreal tipHeight = 0.25 * 0.5 * 0.5 * w;
        qreal bodyOffset = 0.35 * 0.5 * 0.5 * w;
        qreal length = 0.5 * 0.5 * w * 0.6;

        qreal x = offset, y = height;
        QPainterPath arrow(QPointF(x, y)); //1

        x -= tipLenght;
        y -= tipHeight;
        arrow.lineTo(x, y); //2

        y += bodyOffset;
        arrow.lineTo(x, y); //3

        x -= length;
        arrow.lineTo(x, y); //4

        y = height;
        arrow.lineTo(x, y); //5

        y += tipHeight - bodyOffset;
        arrow.lineTo(x, y); //6

        x = offset - tipLenght;
        arrow.lineTo(x, y); //7

        y += bodyOffset;
        arrow.lineTo(x, y); //8

        x = offset;
        y = height;
        arrow.lineTo(x, y); //9

        // painter.setBrush();
        painter.fillPath(arrow, QBrush(QColor(245, 128, 37)));
    }

    painter.setFont(QFont("Arial", 60));
    const int time = ceil((5000 - _elapsed.elapsed()) / 1000.0);

    if (_elapsed.elapsed() > 2600 && !_soundPlayed)
    {
        _clickSound.setPosition(0);
        _clickSound.play();
        _soundPlayed = true;
    }

    if (time > 0)
    {
        painter.drawText(QRect(0, h / 2 + 0.1 * h, w, 60), Qt::AlignCenter, QString::number(time));

        painter.end();
    }
    else
    {
        painter.end();

        _caputure.terminate();
        while (!_caputure.isFinished())
            ;

        //_caputure.capture(_img);
        //_img.save("test.png");
        _updateTimer.stop();
        processCapturedImage(_img);
        //_clickSound.stop();
        //_clickSound.setMedia(QUrl("qrc:/sounds/camera"));

        accept();
    }
}