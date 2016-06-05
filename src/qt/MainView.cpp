#include <QFileInfo>

#ifdef WIN32
#include <windows.h>
#include <gl\gl.h>
#include <gl\GLU.h>
#endif
#include <QGLWidget>
#include <QDateTime>
#include <iostream>
#include "MainView.hpp"
#include <cmath>
#include <fstream>
#include <OpenGL/gl3.h>

MainView::MainView(QWidget *parent)
: super(parent), _shader(this), _grid(100,100), _texture(NULL), _morphMode(false), _preserveBounday(false), _currentIndex(-1)
{ 
    _boundayPoly.resize(4);
}

MainView::~MainView()
{ 
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &_vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &_ibo);

    glBindBuffer(GL_ARRAY_BUFFER,0);
    glDeleteBuffers(1, &_vbo);

    if(_texture)
    {
        _texture->destroy();
        delete _texture;
    }
}

void MainView::toggleMorphMode()
{
    _morphMode = !_morphMode;

    _targetPoly.clear();

    if(_morphMode)
    {
        _targetPoly.insert(_targetPoly.begin(),_sourcePoly.begin(), _sourcePoly.end());
    }

    update();
}

void MainView::togglePreseveBounday()
{
    _preserveBounday = !_preserveBounday;

    update();
}

void MainView::setUniforms()
{
    GLuint vertLoc = _shader.uniformLocation("nVertices");
    if(!_morphMode)
    {
        _shader.setUniformValue(vertLoc,-1);
        return;
    }

    _shader.setUniformValue(vertLoc,GLint(_sourcePoly.size()));

    GLuint sourceLoc = _shader.uniformLocation("source");
    _shader.setUniformValueArray(sourceLoc,&_sourcePoly[0], _sourcePoly.size());

    GLuint targetLoc = _shader.uniformLocation("target");
    _shader.setUniformValueArray(targetLoc,&_targetPoly[0], _targetPoly.size());

    GLuint boundayLoc = _shader.uniformLocation("bounday");
    _shader.setUniformValueArray(boundayLoc,&_boundayPoly[0], _boundayPoly.size());

    GLuint preserveBoundaryLoc = _shader.uniformLocation("preserveBoundary");
    _shader.setUniformValue(preserveBoundaryLoc,_preserveBounday);

    checkGLError("setUniforms");
}

void MainView::setTexture(const QString &path)
{
    if(_texture)
    {
        _texture->destroy();
        delete _texture;
    }

     _targetPoly.clear();
     _sourcePoly.clear();

    const QFileInfo textureFile(path);
    const QImage img = QImage(textureFile.absoluteFilePath()).mirrored();
    _texture = new QOpenGLTexture(img);
    _texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    _texture->setMagnificationFilter(QOpenGLTexture::Linear);

    _grid.generate(img.width(),img.height(),this->width(),this->height(),_boundayPoly);

    updateVBO();

    update();
    update();
}

void MainView::updateVBO()
{
    checkGLError("begin_genvao");
    glBindVertexArray(_vao);
    checkGLError("end_genvao");

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, _grid.vertices().size() * sizeof(GLfloat), &_grid.vertices()[0], GL_DYNAMIC_DRAW);

    glVertexAttribPointer(_posLoc, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), 0);
    glVertexAttribPointer(_textureLoc, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (void *)(2* sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER,0);
    checkGLError("end_bufferdata");
}

void MainView::initializeGL() {

    printf("Supported GLSL version is %s.\n", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));


    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glClearColor(0.8, 0.8, 0.8, 1);


    QOpenGLShader vertShader(QOpenGLShader::Vertex);
    QFileInfo vertFile(":/shaders/shader.vert");
    vertShader.compileSourceFile(vertFile.absoluteFilePath());


    QOpenGLShader fragShader(QOpenGLShader::Fragment);
    QFileInfo fragFile(":/shaders/shader.frag");
    fragShader.compileSourceFile(fragFile.absoluteFilePath());

    _shader.addShader(&vertShader);
    _shader.addShader(&fragShader);

    _shader.link();


    _posLoc     = _shader.attributeLocation("posIn");
    _textureLoc = _shader.attributeLocation("textureIn");

    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);

    setTexture(":/img/default");

    glBindVertexArray(_vao);
    glGenBuffers(1, &_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _grid.indices().size() * sizeof(GLuint), &_grid.indices()[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    checkGLError("end_ibufferdata");

    checkGLError("end_init");
}



QVector2D MainView::mouseToOpenGl(QMouseEvent *e) const
{
    const float x = 2*float(e->x())/this->width() -1;
    const float y = 2*float(this->height()-e->y())/this->height()-1;
    return QVector2D(x,y);
}

void MainView::mousePressEvent(QMouseEvent *e)
{
    _currentPosition = mouseToOpenGl(e);

    for(int i=0;i<_targetPoly.size();++i)
    {
        const float dist = _currentPosition.distanceToPoint(_targetPoly[i]);
        if(dist<0.05)
        {
            _currentIndex=i;
            return;
        }
    }

    _currentIndex = -1;
}

void MainView::mouseMoveEvent(QMouseEvent *e)
{
    if(!_morphMode) return;
    if(_currentIndex<0) return;

    QVector2D newPos = mouseToOpenGl(e);
    _targetPoly[_currentIndex] += newPos-_currentPosition;

    _currentPosition = newPos;

    update();
}

void MainView::mouseReleaseEvent(QMouseEvent *e)
{ 
    if(_morphMode)
    {
        _currentIndex = -1;
        return;
    }

    _sourcePoly.push_back(mouseToOpenGl(e));
    update();
}


void MainView::paintEvent(QPaintEvent *e) 
{
    const std::vector<QVector2D> &currentPoly = _morphMode?_targetPoly:_sourcePoly;

    QPainter painter(this);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glLoadIdentity();
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // glLineWidth(10);

    _shader.bind();
    setUniforms();

    _texture->bind();

    
    glBindVertexArray(_vao);
    glEnableVertexAttribArray(_posLoc);
    glEnableVertexAttribArray(_textureLoc);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
    glDrawElements(GL_TRIANGLE_STRIP, _grid.indices().size(), GL_UNSIGNED_INT, (void*)0  );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(_posLoc);
    glDisableVertexAttribArray(_textureLoc);
    glBindVertexArray(0);

    checkGLError("end_unbind");

    _shader.release();


    glColor3f(0.1f,0.1f,0.1f);
    glLineWidth(_morphMode?2:10);
    if(currentPoly.size()>=3)
    {
        glBegin(GL_LINE_LOOP);
        for(int i=0;i<currentPoly.size();++i)
        {
            glVertex2f(currentPoly[i].x(),currentPoly[i].y());
        }
        glEnd();

        if(_morphMode)
        {
            glPointSize(20);
            glBegin(GL_POINTS);
            for(int i=0;i<currentPoly.size();++i)
            {
                glVertex2f(currentPoly[i].x(),currentPoly[i].y());
            }
            glEnd();  
        }
    }

    if(_preserveBounday)
    {
        glColor3f(1.0f,0.0f,0.0f);
        glLineWidth(5);
        glBegin(GL_LINE_LOOP);
        for(int i=0;i<_boundayPoly.size();++i)
        {
            glVertex2f(_boundayPoly[i].x(),_boundayPoly[i].y());
        }
        glEnd();
    }

    glPopMatrix();


    painter.end();
    checkGLError("draw -> image");
}

void MainView::checkGLError(const std::string &msg)
{
#ifndef NDEBUG
    GLenum error = GL_NO_ERROR;
    int index=0;
    do {
        error = glGetError();
        if (error != GL_NO_ERROR)
            std::cerr<<"["<<msg<<"] "<<index++<<" "<<error<<std::endl;
    } while (error != GL_NO_ERROR);
#endif
}

