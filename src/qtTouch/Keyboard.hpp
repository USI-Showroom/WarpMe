#ifndef KEYBOARD_HPP__
#define KEYBOARD_HPP__

#include <QWidget>
#include "KeyboardButton.hpp"
#include <vector>

class Keyboard : public QWidget
{
	Q_OBJECT

public:
	Keyboard(QWidget *parent = NULL);
	virtual ~Keyboard();

	void resizeEvent(QResizeEvent * event);

signals:
	void keyPressed(QString key);
	void deletePressed();
	void shiftPressed();


private:
	std::vector<std::vector<KeyboardButton *> > _buttons;
	std::vector<std::vector<int> > _layout;
	std::vector<int> _cols;
	std::vector<int> _spaces;
	int _maxCols;
	int _maxSpaces;

	void buildLayout();
};

#endif //KEYBOARD_HPP__
