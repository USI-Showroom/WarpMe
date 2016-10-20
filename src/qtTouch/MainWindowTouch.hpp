// Copyright (c) 2015 Teseo Schneider (Universita della Svizzera italiana)
// -------------------------------------------------------------------------------------------
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
// -----------------------------------------------------------------------

#ifndef __MAINWINDOW_Touch_HPP__
#define __MAINWINDOW_Touch_HPP__

#include <vector>
#include <QMainWindow>

#include <QPrinter>

namespace Ui {
    class MainWindowTouch;
}


class MainWindowTouch : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindowTouch(QWidget *parent = 0);
    ~MainWindowTouch();

private slots:
    void selectImg();
    void clear();
    void morph();
    void openWebcamPreview();
    void print();
    void getImgFromPhone();
    void preserveBoundayToggled(int);


private:
    Ui::MainWindowTouch* _ui;
    QPrinter _printer;
    QImage *_logo;
};



#endif // __MAINWINDOW_Touch_HPP__
