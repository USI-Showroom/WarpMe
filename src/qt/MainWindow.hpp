// Copyright (c) 2015 Teseo Schneider (Universita della Svizzera italiana)
// -------------------------------------------------------------------------------------------
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
// -----------------------------------------------------------------------

#ifndef __MAINWINDOW_HPP__
#define __MAINWINDOW_HPP__

#include <vector>
#include <QMainWindow>


namespace Ui {
    class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionOpen_image_triggered();
    void on_actionClear_triggered();
    void on_actionMorph_mode_triggered();
    void on_actionPreserve_bounday_triggered();


private:
    Ui::MainWindow* _ui;
};



#endif // __MAINWINDOW_HPP__
