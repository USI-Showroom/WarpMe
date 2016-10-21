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
#include "mimeattachment.h"


static const char* email="usi.at.cebit@gmail.com";

MailManager::MailManager(const QImage &img, QWidget *parent)
:QDialog(parent), _ui(new Ui::MailManager), _img(img)
{
    _ui->setupUi(this);

    _client = new SmtpClient("smtp.gmail.com", 465, SmtpClient::SslConnection);
    _client->setUser(email);
    _client->setPassword("Aa1235678");

    _ui->email->setFocus();
}

void MailManager::sendMail()
{
    MimeMessage message;

    message.setSender(new EmailAddress(email, "Usi @ CeBIT"));
    message.addRecipient(new EmailAddress(_ui->email->text(), _ui->email->text()));
    message.setSubject("Nice deformed image");

    MimeText text;
    text.setText("Hi,\nThis is your image from Usi @ CeBIT.\nVisit us at www.usi.ch.\n");
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