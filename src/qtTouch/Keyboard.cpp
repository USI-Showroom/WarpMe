#include "Keyboard.hpp"

#include <iostream>
#include <cmath>



Keyboard::Keyboard(QWidget *parent)
: QWidget(parent)
{
	buildLayout();


	_buttons.resize(_layout.size());
	_cols.resize(_buttons.size());
	_spaces.resize(_buttons.size());
	_maxCols = 0;
	
	for (int i = 0; i < _layout.size(); ++i)
	{
		_buttons[i].resize(_layout[i].size());
		int count=0;

		for (int j = 0; j < _layout[i].size(); ++j)
		{
			KeyboardButton *btn=new KeyboardButton(_layout[i][j],this);
			
			connect(btn,SIGNAL(keyPressed(QString)),this,SIGNAL(keyPressed(QString)));
			connect(btn,SIGNAL(deletePressed()),this,SIGNAL(deletePressed()));
			connect(btn,SIGNAL(shiftPressed()),this,SIGNAL(shiftPressed()));

			_buttons[i][j]=btn;
			count+=_buttons[i][j]->size();
		}
		_cols[i]=count;
		_spaces[i]=_layout[i].size()-1;

		if(_maxCols<count){
			_maxCols=count;
			_maxSpaces=_spaces[i];
		}
	}
}

Keyboard::~Keyboard()
{
	for (int i = 0; i < _buttons.size(); ++i){
		for (int j = 0; j < _buttons[i].size(); ++j){
			delete _buttons[i][j];
		}
	}
}


void Keyboard::resizeEvent(QResizeEvent * event)
{
	const QSize &size = this->size();

	const int w=size.width();

	const int space=std::floor(w/_maxCols);
	const int btnSize=std::floor(space*0.9f);
	const int offset=std::floor((w - btnSize*_maxCols)/float(_maxSpaces));


	setMinimumSize(QSize(size.width(),(btnSize+offset)*_layout.size()));

	int voffset=0;
	for (int i = 0; i < _buttons.size(); ++i){
		const float startOffset = (w - btnSize*_cols[i]-_spaces[i]*offset)/2.0f;

		int index=0;
		if(i==4) voffset=offset;

		for (int j = 0; j < _buttons[i].size(); ++j){
			KeyboardButton *btn=_buttons[i][j];
			btn->resize(btnSize);
			btn->move(startOffset+btnSize*index+offset*j,(btnSize+offset)*i+voffset);
			index+=btn->size();
		}
	}
}



void Keyboard::buildLayout()
{
	_layout.clear();

	{
		_layout.push_back(std::vector<int>());
		std::vector<int> &tmp=_layout.back();
		tmp.push_back('~');
		tmp.push_back('`');
		tmp.push_back('-');
		tmp.push_back('_');
		tmp.push_back('+');
		tmp.push_back('=');
		tmp.push_back('[');
		tmp.push_back(']');
		tmp.push_back('{');
		tmp.push_back('}');
	}

	{
		_layout.push_back(std::vector<int>());
		std::vector<int> &tmp=_layout.back();
		tmp.push_back('!');
		tmp.push_back('@');
		tmp.push_back('#');
		tmp.push_back('$');
		tmp.push_back('%');
		tmp.push_back('^');
		tmp.push_back('&');
		tmp.push_back('*');
		tmp.push_back('(');
		tmp.push_back(')');
	}

	{
		_layout.push_back(std::vector<int>());
		std::vector<int> &tmp=_layout.back();
		tmp.push_back('.');
		tmp.push_back(',');
		tmp.push_back(':');
		tmp.push_back(';');
		tmp.push_back('\'');
		tmp.push_back('"');
		tmp.push_back('?');
		tmp.push_back('\\');
		tmp.push_back('/');
		tmp.push_back('|');
	}

	{
		_layout.push_back(std::vector<int>());
		std::vector<int> &tmp=_layout.back();
		tmp.push_back('1');
		tmp.push_back('2');
		tmp.push_back('3');
		tmp.push_back('4');
		tmp.push_back('5');
		tmp.push_back('6');
		tmp.push_back('7');
		tmp.push_back('8');
		tmp.push_back('9');
		tmp.push_back('0');
	}

	{
		_layout.push_back(std::vector<int>());
		std::vector<int> &tmp=_layout.back();
		tmp.push_back('q');
		tmp.push_back('w');
		tmp.push_back('e');
		tmp.push_back('r');
		tmp.push_back('t');
		tmp.push_back('y');
		tmp.push_back('u');
		tmp.push_back('i');
		tmp.push_back('o');
		tmp.push_back('p');
	}

	{
		_layout.push_back(std::vector<int>());
		std::vector<int> &tmp=_layout.back();
		tmp.push_back('a');
		tmp.push_back('s');
		tmp.push_back('d');
		tmp.push_back('f');
		tmp.push_back('g');
		tmp.push_back('h');
		tmp.push_back('j');
		tmp.push_back('k');
		tmp.push_back('l');
	}


	{
		_layout.push_back(std::vector<int>());
		std::vector<int> &tmp=_layout.back();
		tmp.push_back(-2);
		tmp.push_back('z');
		tmp.push_back('x');
		tmp.push_back('c');
		tmp.push_back('v');
		tmp.push_back('b');
		tmp.push_back('n');
		tmp.push_back('m');
		tmp.push_back(-3);
	}

	{
		_layout.push_back(std::vector<int>());
		std::vector<int> &tmp=_layout.back();
		tmp.push_back('@');
		tmp.push_back('<');
		tmp.push_back('>');
		tmp.push_back(-1);
		tmp.push_back(-4);
	}

}
