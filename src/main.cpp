// Copyright (c) 2015 Teseo Schneider (Universita della Svizzera italiana)
// -------------------------------------------------------------------------------------------
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
// -----------------------------------------------------------------------

#include <QApplication>
#include <QMetaType>
#include <QtOpenGL>

#ifdef TOUCH_SCREEN_MODE
#include "MainWindowTouch.hpp"
#else
#include "MainWindow.hpp"
#endif

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QFont f=QApplication::font();
	f.setFamily("Helvetica");
	QApplication::setFont(f);
	a.setAttribute(Qt::AA_UseHighDpiPixmaps);


#ifdef TOUCH_SCREEN_MODE
	MainWindowTouch w;
#else
	MainWindow w;
#endif
	w.show();




	return a.exec();
}
