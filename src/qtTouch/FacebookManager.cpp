// Copyright (c) 2015 Teseo Schneider (Universita della Svizzera italiana)
// -------------------------------------------------------------------------------------------
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
// -----------------------------------------------------------------------

#include "FacebookManager.hpp"
#include "ui_FacebookManager.h"

#include <iostream>
#include <QBuffer>
#include "smtpclient.h"
#include "mimetext.h"
#include "mimeattachment.h"

#include <QUrlQuery>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QUuid>


FacebookManager::FacebookManager(const QImage &img, QWidget *parent)
:QDialog(parent), _ui(new Ui::FacebookManager), _img(img)
{
    _ui->setupUi(this);

    _ui->caption->setFocus();

    if(parent && parent->isFullScreen())
        showFullScreen();
    
}


void FacebookManager::replyFinished(QNetworkReply *reply)
{
    QString resp(reply->readAll());
    std::cout<<resp.toStdString()<<std::endl;

    accept();
}

void FacebookManager::postPicture()
{
    static const QString accessToken = "EAACEdEose0cBAELj0UTdLIVj8S16SFAwyyteSr3Xgr6JPuRH0EMNWpZAeaNsylnJwX6V7bUmNL9rSVmUQADLZB7FUrfZAQIYC7ZBw94aodFw4x33tx2cgeulPcV7xVFZClpZCFCKlx3zB6ArHfgtN2O7KtWYxESiHcs4ZC05W6eVAZDZD";

    const QUuid uuid;
    const QString boundary="Boundary-"+uuid.toString();
    const QString caption = _ui->caption->toPlainText();

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    QUrl url("https://graph.facebook.com/v2.4/me/photos"); //maybe change to post to album
    QNetworkRequest request(url);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary="+boundary);

    QByteArray body;

    body.append("--"+boundary+"\r\n");
    body.append("Content-Disposition:form-data; name=\"caption\"\r\n\r\n");
    body.append(caption+"\r\n");
    
    body.append("--"+boundary+"\r\n");
    body.append("Content-Disposition:form-data; name=\"access_token\"\r\n\r\n");
    body.append(accessToken+"\r\n");


    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    _img.save(&buffer, "PNG");


    body.append("--"+boundary+"\r\n");
    body.append("Content-Disposition:form-data; name=\"source\"; filename=\"photo.png\"\r\n");
    body.append("Content-Type: image/png\r\n\r\n");
    body.append(byteArray);
    body.append("\r\n");
    body.append("--"+boundary+"--\r\n");

    connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(replyFinished(QNetworkReply *)));

    manager->post(request, body);
}

FacebookManager::~FacebookManager()
{
}