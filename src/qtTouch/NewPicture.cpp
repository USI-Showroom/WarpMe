// Copyright (c) 2015 Teseo Schneider (Universita della Svizzera italiana)
// -------------------------------------------------------------------------------------------
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
// -----------------------------------------------------------------------

#include "NewPicture.hpp"
#include "ui_NewPicture.h"
#include "PaperConstants.hpp"

#include <QMessageBox>
#include <iostream>
#include <QDirIterator>

static const char *IMAGE_PROP_NAME="image_path";



NewPicture::NewPicture(QWidget *parent)
:QDialog(parent), _ui(new Ui::NewPicture)
{
    _ui->setupUi(this);
    setModal(true);


    QDirIterator it(":/photos",  QDirIterator::Subdirectories);
    QLayout *layout = _ui->scrollPanelLayout;

    while (it.hasNext()) {
        QString imgPath=it.next();

        QPushButton *btn = new QPushButton(QIcon(imgPath),"");
        btn->setProperty(IMAGE_PROP_NAME, imgPath);
        connect( btn, SIGNAL(clicked()), this, SLOT(imageClicked()));

        layout->addWidget(btn);
    }
}


void NewPicture::imageClicked()
{
    _path = sender()->property(IMAGE_PROP_NAME).toString();

    accept();
}

void NewPicture::resizeEvent(QResizeEvent *event)
{
    const float w=_ui->scrollPanel->width();
    const float h=_ui->scrollPanel->height()-30;

    float frameW, frameH;
    PaperConstants::Scale(w, h, frameW, frameH);


    QLayout *layout = _ui->scrollPanelLayout;


    for (int i = 0; i < layout->count(); ++i)
    {
        QPushButton *button = static_cast<QPushButton *>(layout->itemAt(i)->widget());
        if (button != NULL){
            button->setMinimumSize(frameW,frameH);
            button->setIconSize(QSize(frameW,frameH));
        }
    }
}


NewPicture::~NewPicture()
{
    delete _ui;
}