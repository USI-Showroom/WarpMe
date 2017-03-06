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

    message.setSender(new EmailAddress(email, "Orientati @ USI"));
    message.addRecipient(new EmailAddress(_ui->email->text(), _ui->email->text()));
    message.setSubject("WarpMe - La tua foto ad Orientati 2017");

    MimeHtml text;
	text.setHtml("Wow, bella foto!</br>"
		"Grazie per aver utilizzato l'applicazione WarpMe ad <a href=\"http://www.orientati.ch/\">Orientati 2017</a></br>"
		"Tieniti informato sulle Scienze Informatiche all'USI - Universit&agrave; della Svizzera Italiana sul sito <a href=\"http://www.inf.usi.ch/\">www.inf.usi.ch</a></br></br>"
		"Speriamo di rivederti presto :)</br></br>");
    message.addPart(&text);


    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    _img.save(&buffer, "PNG");


    MimeAttachment attachment(byteArray,"image.png");
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
