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


FacebookManager::FacebookManager(QWidget *parent)
:QDialog(parent), _ui(new Ui::FacebookManager)
{
    _ui->setupUi(this);

    _ui->caption->setFocus();

    if(parent && parent->isFullScreen())
        showFullScreen();
    else
        resize(parent->size());
    


    connect(_ui->keyboard,SIGNAL(keyPressed(QString)),this,SLOT(keyPressed(QString)));
    connect(_ui->keyboard,SIGNAL(deletePressed()),this,SLOT(deletePressed()));
    connect(_ui->keyboard,SIGNAL(shiftPressed()),this,SLOT(shiftPressed()));

    _nextUpper=false;
}

int FacebookManager::exec()
{
	if (parentWidget() && parentWidget()->isFullScreen())
		showFullScreen();
	else if(parentWidget())
		resize(parentWidget()->size());

	return QDialog::exec();
}

void FacebookManager::replyFinished(QNetworkReply *reply)
{
    QString resp(reply->readAll());
    std::cout<<resp.toStdString()<<std::endl;
}

void FacebookManager::postPicture()
{
    // https://developers.facebook.com/apps
    // add other accounts

    // https://developers.facebook.com/tools/explorer/
    // select the app in Application 
    // click "get user access token"
    // select publish_action AND user_photos
    // click on the blue i
    // open access token tool
    // click extend access token

    //Put the access token and use
    // /v2.8/me/albums to get album id

    static const QString accessToken = "EAAOFZCzlH70gBAFfczEZCte5na0ZCRhyk7K9EmYEEjaitv38J6tnGCCGjfXZA7CXQ6D9GYmV8AtI0bK1ImpZAfSx2RUd5XRqz4gQSvZAuQuGzy2j0OC9ZB8kBq3NEMwri4oIJVY8Lg0fA3ZA68J4aDPzetKYXTYsa07ZAPHIjDQTZAIwZDZD";
    static const QString albumId = "1254221368001696"; 

    const QUuid uuid;
    const QString boundary="Boundary-"+uuid.toString();
    const QString caption = _ui->caption->toPlainText();

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    QUrl url("https://graph.facebook.com/v2.8/"+albumId+"/photos");
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
    _img->save(&buffer, "PNG");


    body.append("--"+boundary+"\r\n");
    body.append("Content-Disposition:form-data; name=\"source\"; filename=\"photo.png\"\r\n");
    body.append("Content-Type: image/png\r\n\r\n");
    body.append(byteArray);
    body.append("\r\n");
    body.append("--"+boundary+"--\r\n");

    connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(replyFinished(QNetworkReply *)));

    manager->post(request, body);
    
    accept();
}

FacebookManager::~FacebookManager()
{
}




void FacebookManager::keyPressed(QString key)
{
    if(_nextUpper)
        key=key.toUpper();
    

    const int pos=_ui->caption->textCursor().position();
    QString tmp=_ui->caption->toPlainText();
    tmp.insert(pos,key);
    _ui->caption->setPlainText(tmp);

    QTextCursor cursor = _ui->caption->textCursor();
    cursor.setPosition(pos+1);
    _ui->caption->setTextCursor(cursor);

    _nextUpper=false;
}

void FacebookManager::deletePressed()
{
    const int pos=_ui->caption->textCursor().position();
    QString tmp=_ui->caption->toPlainText();
    tmp.remove(pos-1,1);
    _ui->caption->setPlainText(tmp);

    QTextCursor cursor = _ui->caption->textCursor();
    cursor.setPosition(pos-1);
    _ui->caption->setTextCursor(cursor);


    _nextUpper=false;
}

void FacebookManager::shiftPressed()
{
    _nextUpper=true;
}
