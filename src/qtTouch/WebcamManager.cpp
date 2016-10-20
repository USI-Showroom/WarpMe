#include "WebcamManager.hpp"
#include "ui_WebcamManager.h"

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
    }

    if(!_camera) return;


    _camera->setViewfinder(_ui->preview);

    // connect(_camera, SIGNAL(stateChanged(QCamera::State)), this, SLOT(updateCameraState(QCamera::State)));
    // connect(_camera, SIGNAL(error(QCamera::Error)), this, SLOT(displayCameraError()));
    // connect(_camera, SIGNAL(lockStatusChanged(QCamera::LockStatus,QCamera::LockChangeReason)),this, SLOT(updateLockStatus(QCamera::LockStatus,QCamera::LockChangeReason)));

    _imageCapture = new QCameraImageCapture(_camera);


    // connect(_imageCapture, SIGNAL(readyForCaptureChanged(bool)), this, SLOT(readyForCapture(bool)));
    connect(_imageCapture, SIGNAL(imageCaptured(int,QImage)), this, SLOT(processCapturedImage(int,QImage)));
    connect(_imageCapture, SIGNAL(imageSaved(int,QString)), this, SLOT(imageSaved(int,QString)));
    connect(_imageCapture, SIGNAL(error(int,QCameraImageCapture::Error,QString)), this, SLOT(displayCaptureError(int,QCameraImageCapture::Error,QString)));

    _camera->setCaptureMode(QCamera::CaptureStillImage);
    _camera->start();
}




WebcamManager::~WebcamManager()
{
    delete _ui;
    delete _camera;
    delete _imageCapture;
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
    _img=img;
// Q_UNUSED(requestId);
// img.save("asd.png");
// ui->lastImagePreviewLabel->setPixmap(QPixmap::fromImage(scaledImage));

// // Display captured image for 4 seconds.
// displayCapturedImage();
// QTimer::singleShot(4000, this, SLOT(displayViewfinder()));
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