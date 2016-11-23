// Copyright (c) 2015 Teseo Schneider (Universita della Svizzera italiana)
// -------------------------------------------------------------------------------------------
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
// -----------------------------------------------------------------------

#ifndef __WEBCAM_MANAGER_HPP__
#define __WEBCAM_MANAGER_HPP__

#include <QObject>
#include <QCamera>
#include <QDialog>
#include <QCameraImageCapture>
#include <QTimer>
#include <QElapsedTimer>
#include <QSoundEffect>

namespace Ui {
	class WebcamManager;
}

class QCameraViewfinder;

class WebcamManager : public QDialog
{
	Q_OBJECT

public:
	WebcamManager(QWidget *parent = 0);
	~WebcamManager();

	inline const QImage &image() const { return _img; }
private slots:
	void updateCameraStatus(QCamera::Status state);
	void updateLockStatus(QCamera::LockStatus ls,QCamera::LockChangeReason lcr);
	void takeImage();
	void displayCaptureError(int, QCameraImageCapture::Error, const QString &errorString);
	void readyForCapture(bool ready);


	void processCapturedImage(int requestId, const QImage &img);

	void imageSaved(int id, const QString &fileName);



protected:
	void closeEvent(QCloseEvent *event);
	void resizeEvent(QResizeEvent *event);
	void paintEvent(QPaintEvent * event);
private:
	Ui::WebcamManager* _ui;

	QCamera *_camera;
	QCameraImageCapture *_imageCapture;
	
	bool _isCapturingImage, _applicationExiting;
	
	QImage _img;
	QCameraViewfinder *_preview;
	QTimer _updateTimer;
	QElapsedTimer _elapsed;
	bool _started, _soundPlayed;
	QSoundEffect _clickSound;
};



#endif // __WEBCAM_MANAGER_HPP__
