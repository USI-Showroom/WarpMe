#ifndef KEYBOARD_BUTTON_HPP__
#define KEYBOARD_BUTTON_HPP__

#include <QPushButton>

class KeyboardButton : public QPushButton 
{
Q_OBJECT

public:
	KeyboardButton(const int letter, QWidget *parent = NULL);
	virtual ~KeyboardButton();

	void resize(const int size);

	inline int size() const { return _size; }

	
private slots:
	void btnClicked(bool checked = false);
signals:
	void keyPressed(QString key);
	void deletePressed();
	void shiftPressed();
	void enterPressed();

private:
	int _letter;
	int _size;
	QString _char;

protected:
	void mousePressEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);

};

#endif //KEYBOARD_BUTTON_HPP__

