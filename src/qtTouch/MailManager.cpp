// Copyright (c) 2015 Teseo Schneider (Universita della Svizzera italiana)
// -------------------------------------------------------------------------------------------
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
// -----------------------------------------------------------------------

#include "MailManager.hpp"
#include "ui_MailManager.h"

#include <iostream>
#include <QBuffer>
#include "smtpclient.h"
#include "mimetext.h"
#include "mimehtml.h"
#include "mimeattachment.h"


static const char* email="warpme@usi.ch";
SmtpClient client("mail.usi.ch", 587, SmtpClient::TlsConnection);

MailManager::MailManager(const QImage &img, QWidget *parent)
:QDialog(parent), _ui(new Ui::MailManager), _img(img)
{
    _ui->setupUi(this);

    client.setUser(email);
    client.setPassword("tasagef4");
    client.setAuthMethod(SmtpClient::AuthLogin);

    _ui->email->setFocus();

    if(parent && parent->isFullScreen())
        showFullScreen();
    else
        resize(parent->size());


    connect(_ui->keyboard,SIGNAL(keyPressed(QString)),this,SLOT(keyPressed(QString)));
    connect(_ui->keyboard,SIGNAL(deletePressed()),this,SLOT(deletePressed()));
    connect(_ui->keyboard,SIGNAL(shiftPressed()),this,SLOT(shiftPressed()));

    _nextUpper=false;
    
}

void MailManager::sendMail()
{
    MimeMessage message;

    message.setSender(new EmailAddress(email, "WarpMe - USI"));
    message.addRecipient(new EmailAddress(_ui->email->text(), _ui->email->text()));
    message.setSubject("WarpMe – Your picture");

    MimeHtml text;
	text.setHtml("Wow, amazing picture!<br>"
		"Thank you for using WarpMe.<br>"
		"Learn how to shape your future with Informatics at USI - Universit&agrave; della Svizzera Italiana at <a href=\"http://www.inf.usi.ch/\">www.inf.usi.ch</a><br><br>"
		"We hope to see you soon :)<br><br><br>"

        "Faculty of Informatics<br>"
        "Universit&agrave; della Svizzera Italiana<br>"
        "Via Giuseppe Buffi 13<br>"
        "6900 Lugano, Switzerland<br>"
        "tel +41 58 666 46 90<br>"
        "email <a href=\"mailto:decanato.inf@usi.ch\">decanato.inf@usi.ch</a><br><br>"
        );
    message.addPart(&text);


    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    _img.save(&buffer, "PNG");


    MimeAttachment attachment(byteArray,"picture.png");
    attachment.setContentType("image/png");
    message.addPart(&attachment);


    if (!client.connectToHost()) {
        std::cerr<<"unable to connect to host"<<std::endl;
        accept();
        return;
    }
    if (!client.login()) {
        std::cerr<<"unable to login to mailserver"<<std::endl;
        accept();
        return;
    }
    if (!client.sendMail(message)) {
        std::cerr<<"unable to send message"<<std::endl;
        accept();
        return;
    }
    client.quit();

    accept();
}

MailManager::~MailManager()
{
    //asd
}


void MailManager::keyPressed(QString key)
{
    if(_nextUpper)
        key=key.toUpper();
    

    const int pos=_ui->email->cursorPosition();
    QString tmp=_ui->email->text();
    tmp.insert(pos,key);
    _ui->email->setText(tmp);
    _ui->email->setCursorPosition(pos+1);

    _nextUpper=false;
}

void MailManager::deletePressed()
{
    const int pos=_ui->email->cursorPosition();
    QString tmp=_ui->email->text();
    tmp.remove(pos-1,1);
    _ui->email->setText(tmp);
    _ui->email->setCursorPosition(pos-1);

    _nextUpper=false;
}

void MailManager::shiftPressed()
{
    _nextUpper=true;
}
