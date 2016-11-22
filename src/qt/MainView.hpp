// Copyright (c) 2015 Teseo Schneider (Universita della Svizzera italiana)
// -------------------------------------------------------------------------------------------
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
// -----------------------------------------------------------------------

#include <deque>

#include <QOpenGLWidget>
#include <QPaintEvent>
#include <QOpenGLShaderProgram>

#include <QOpenGLFunctions_2_0>
#include <QOpenGLFunctions>

#include <QKeyEvent>
#include <QMouseEvent>

#ifdef WIN32
#include <QOpenGLFunctions_3_0>
#endif

#include <QImage>
#include <iostream>

#include "Grid.hpp"



class MainView : public QOpenGLWidget
{
	Q_OBJECT
private:
	typedef QOpenGLWidget  super;

	
public:
	MainView(QWidget *parent = NULL);
	~MainView();


	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);

	void setTexture(const QString &path);
	void setTexture(const QImage &imgIn);

	void toggleMorphMode();
	void togglePreseveBounday();
	inline void clear() { _targetPoly.clear(); _sourcePoly.clear(); update(); }

	inline bool morphMode() const { return _morphMode; }
	inline void setDrawForPrinting(const bool val) { _drawForPrinting = val; }

private:
	void checkGLError(const std::string &msg);
	void setUniforms();

	void updateVBO();

	QVector2D mouseToOpenGl(QMouseEvent *e) const;

#ifdef TOUCH_SCREEN_MODE
	void resetEllipse();
#endif

protected:
	void initializeGL();
	void paintGL();

#ifdef WIN32
	void resizeGL(int width, int height)
	{
		glViewport(0, 0, (GLint)width, (GLint)height);
	}
#endif

// #ifdef TOUCH_SCREEN_MODE
// 	void resizeEvent(QResizeEvent * event)
// 	{
// 		// makeCurrent();
// 		// glViewport(0, 0, (GLint)event->size().width(), (GLint)event->size().height());
// 		QWidget::resizeEvent(event);
// 	}
// #endif


private:
	QOpenGLShaderProgram _shader, _colorShader, _circleShader;
	Grid _grid;
	GLuint _vao, _vbo, _ibo;
	// QOpenGLVertexArrayObject *_vao;
	// QOpenGLBuffer *_vbo, *_ibo;

	GLuint _posLoc, _textureLoc;
	QOpenGLTexture *_texture, *_circleTexture;


	
	std::vector<QVector2D> _sourcePoly;
	std::vector<QVector2D> _boundayPoly;
	std::vector<QVector2D> _targetPoly;

	bool _morphMode, _preserveBounday, _drawForPrinting;
	int _currentIndex;
	QVector2D _currentPosition;

#ifdef WIN32
	QOpenGLFunctions_3_0 funs;
#endif // WIN32

};
