#include "KeyboardButton.hpp"

#include <sstream>
#include <iostream>

KeyboardButton::KeyboardButton(const int letter, QWidget *parent)
	: QPushButton(parent), _letter(letter)
{
	if (letter == -1) //space
	{
		_char = " ";
		setText("space");
		_size = 5;
	}
	else if (letter == -2) //shift
	{
		_char = "";
		setText("shift");
		_size = 2;
	}
	else if (letter == -3) //delete
	{
		_char = "";
		setText("del");
		_size = 2;
	}
	else if (letter == -4) //new line
	{
		_char = "";
		setText("return");
		_size = 2;
	}
	else
	{
		std::stringstream tmp;
		tmp << (char)letter;
		_char = QString::fromStdString(tmp.str());
		setText(_char);
		_size = 1;
	}

	setStyleSheet("background-color: #f58025; color: white; border: none; font-size: 20pt;");

	connect(this, SIGNAL(clicked(bool)), this, SLOT(btnClicked(bool)));
}

KeyboardButton::~KeyboardButton()
{
}

void KeyboardButton::btnClicked(bool checked)
{
	if (_letter >= 0 || _letter == -1) //letter or space
		emit keyPressed(_char);
	else if (_letter == -3)
		emit deletePressed();
	else if (_letter == -2)
		emit shiftPressed();
	else if (_letter == -4)
		emit deletePressed();
}

void KeyboardButton::mousePressEvent(QMouseEvent *e)
{
	setStyleSheet("background-color: #fcd4b6; color: white; border: none; font-size: 20pt;");
	QPushButton::mousePressEvent(e);
}

void KeyboardButton::mouseReleaseEvent(QMouseEvent *e)
{
	setStyleSheet("background-color: #f58025; color: white; border: none; font-size: 20pt;");
	QPushButton::mouseReleaseEvent(e);
}

void KeyboardButton::resize(const int size)
{
	QAbstractButton::resize(size * _size, size);
	setIconSize(QSize(size, size));
}