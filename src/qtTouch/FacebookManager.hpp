// Copyright (c) 2015 Teseo Schneider (Universita della Svizzera italiana)
// -------------------------------------------------------------------------------------------
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
// -----------------------------------------------------------------------

#ifndef __FACEBOOK_MANAGER_HPP__
#define __FACEBOOK_MANAGER_HPP__

#include <QObject>
#include <QDialog>



namespace Ui {
	class FacebookManager;
}

class QNetworkReply;

class FacebookManager : public QDialog
{
	Q_OBJECT

public:
	FacebookManager(const QImage &img, QWidget *parent = 0);
	~FacebookManager();

private slots:
	void postPicture();
	void replyFinished(QNetworkReply *reply);

	void keyPressed(QString key);
	void deletePressed();
	void shiftPressed();

private:
	Ui::FacebookManager* _ui;
	const QImage &_img;

	bool _nextUpper;
};



#endif // __FACEBOOK_MANAGER_HPP__
