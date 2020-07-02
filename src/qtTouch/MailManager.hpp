// Copyright (c) 2015 Teseo Schneider (Universita della Svizzera italiana)
// -------------------------------------------------------------------------------------------
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
// -----------------------------------------------------------------------

#ifndef __MAIL_MANAGER_HPP__
#define __MAIL_MANAGER_HPP__

#include <QObject>
#include <QDialog>



namespace Ui {
	class MailManager;
}
class SmtpClient;

class MailManager : public QDialog
{
	Q_OBJECT

public:
	MailManager(const QImage &img, QWidget *parent = 0);
	~MailManager();

private slots:
	void sendMail();

	void keyPressed(QString key);
	void deletePressed();
	void shiftPressed();

private:
	Ui::MailManager* _ui;
	const QImage &_img;
	bool _nextUpper;
};



#endif // __MAIL_MANAGER_HPP__
