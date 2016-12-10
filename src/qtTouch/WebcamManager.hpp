// Copyright (c) 2015 Teseo Schneider (Universita della Svizzera italiana)
// -------------------------------------------------------------------------------------------
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
// -----------------------------------------------------------------------

#ifndef __WEBCAM_MANAGER_HPP__
#define __WEBCAM_MANAGER_HPP__

#include <QObject>
#include <QDialog>
#include <QTimer>
#include <QElapsedTimer>
#include <QSoundEffect>
#include <QThread>

#include "opencv2/opencv.hpp"

namespace Ui {
	class WebcamManager;
}

class QCameraViewfinder;


class CameraCapture : public QThread
{
	Q_OBJECT
public:
	CameraCapture(QObject *parent=NULL);

	void run();
signals:
	void imageReady(const QImage &img, const QRectF &face);
private:
	bool _started;
	cv::VideoCapture _videoCapture;
	cv::CascadeClassifier _faceCascade, _eyesCascade;
	cv::Point _center;


	QRectF detectFace(cv::Mat &frame);
	cv::Point faceFromEyes(cv::Point &priorCenter, const cv::Mat &face);
    

};


class WebcamManager : public QDialog
{
	Q_OBJECT

public:
	WebcamManager(QWidget *parent = 0);
	~WebcamManager();

	inline const QImage &image() const { return _img; }

	inline void startCounter()
	{
		_caputure.start();
		_elapsed.restart();
		_updateTimer.start();
	}

private slots:
	void newImage(const QImage &img, const QRectF &face);

protected:
	void paintEvent(QPaintEvent * event);
private:
	Ui::WebcamManager* _ui;
	
	QImage _img;
	QRectF _face;

	QTimer _updateTimer;
	QElapsedTimer _elapsed;

	bool _soundPlayed;
	QSoundEffect _clickSound;

	CameraCapture _caputure;

	void processCapturedImage(const QImage &img);
};



#endif // __WEBCAM_MANAGER_HPP__
