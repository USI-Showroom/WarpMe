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


static const char* email="usi.warpme@gmail.com";

MailManager::MailManager(const QImage &img, QWidget *parent)
:QDialog(parent), _ui(new Ui::MailManager), _img(img)
{
    _ui->setupUi(this);

    _client = new SmtpClient("smtp.gmail.com", 465, SmtpClient::SslConnection);
    _client->setUser(email);
    _client->setPassword("Aa1235678");

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

    message.setSender(new EmailAddress(email, "USI @ CeBIT"));
    message.addRecipient(new EmailAddress(_ui->email->text(), _ui->email->text()));
    message.setSubject("WarpMe – Your picture at CeBIT 2017");

    MimeHtml text;
	text.setHtml("Wow, amazing picture!</br>"
		"Thank you for using WarpMe at CeBIT 2017.</br>"
		"Learn how to shape your future with Informtaics at USI - Universit&agrave; della Svizzera Italiana at <a href=\"http://www.inf.usi.ch/\">www.inf.usi.ch</a></br></br>"
		"We hope to see you soon :)</br></br></br>"

        "Faculty of Informatics</br>"
        "Universit&agrave; della Svizzera italiana</br>"
        "Via Giuseppe Buffi 13</br>"
        "CH-6900 Lugano</br>"
        "Switzerland</br>"
        "+41 58 666 46 90</br>"
        "<a href=\"mailto:decanato.inf@usi.ch\">decanato.inf@usi.ch</a></br></br>"
        "ONLINE APPLICATION: <a href=\"https://iscrizione.lu.usi.ch/intro.aspx?lng=en\">application.lu.usi.ch</a></br></br></br>"
        );
    message.addPart(&text);


    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    _img.save(&buffer, "PNG");


    MimeAttachment attachment(byteArray,"picture.png");
    attachment.setContentType("image/png");
    message.addPart(&attachment);


    _client->connectToHost();
    _client->login();
    _client->sendMail(message);
    _client->quit();

    accept();
}

MailManager::~MailManager()
{
    delete _client;
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
