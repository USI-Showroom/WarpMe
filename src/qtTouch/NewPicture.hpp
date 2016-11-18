// Copyright (c) 2015 Teseo Schneider (Universita della Svizzera italiana)
// -------------------------------------------------------------------------------------------
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
// -----------------------------------------------------------------------

#ifndef __NEW_PICTURE_HPP__
#define __NEW_PICTURE_HPP__

#include <QObject>
#include <QDialog>


namespace Ui {
	class NewPicture;
}

class NewPicture : public QDialog
{
	Q_OBJECT

public:
	NewPicture(QWidget *parent = 0);
	~NewPicture();

	inline const QString &image() const { return _path; }

private slots:
	void imageClicked();

protected:
	void resizeEvent(QResizeEvent *event);

private:
	Ui::NewPicture* _ui;
	QString _path;
};



#endif // __NEW_PICTURE_HPP__
