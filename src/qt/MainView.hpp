#include <deque>

#include <QOpenGLWidget>
#include <QPaintEvent>
#include <QOpenGLShaderProgram>

#include <QOpenGLFunctions_2_0>
#include <QOpenGLFunctions>

#include <QKeyEvent>
#include <QMouseEvent>

#include <QImage>

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

	void toggleMorphMode();
	void togglePreseveBounday();
	inline void clear() { _targetPoly.clear(); _sourcePoly.clear(); }

private:
	void checkGLError(const std::string &msg);
	void setUniforms();

	void deleteAll();
	void generateVAO();

	QVector2D mouseToOpenGl(QMouseEvent *e) const;

protected:
	// void paintGL();
	void initializeGL();
	void paintEvent(QPaintEvent *e);





private:
	QOpenGLShaderProgram _shader;
	Grid _grid;
	GLuint _vao, _vbo, _ibo;
	GLuint _posLoc, _textureLoc;
	QOpenGLTexture *_texture;
	
	std::vector<QVector2D> _sourcePoly;
	std::vector<QVector2D> _boundayPoly;
	std::vector<QVector2D> _targetPoly;

	bool _morphMode, _preserveBounday;
	int _currentIndex;
	QVector2D _currentPosition;
};
