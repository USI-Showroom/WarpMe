// Copyright (c) 2015 Teseo Schneider (Universita della Svizzera italiana)
// -------------------------------------------------------------------------------------------
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
// -----------------------------------------------------------------------

#include <QFileInfo>

#ifdef WIN32
#include <windows.h>
#include <gl\gl.h>
#include <gl\GLU.h>
#else
// #include <OpenGL/gl3.h>
#endif
#include <QGLWidget>
#include <QDateTime>
#include <iostream>
#include "MainView.hpp"
#include <cmath>
#include <fstream>
#include <qopengl.h>
#include <cassert>
#include "PaperConstants.hpp"

MainView::MainView(QWidget *parent)
: super(parent), 
_shader(this), _colorShader(this), _circleShader(this),
_grid(100,100), _texture(NULL), _morphMode(false), _preserveBounday(true), _currentIndex(-1)
{ 
    _boundayPoly.resize(4);
}

#ifdef TOUCH_SCREEN_MODE
void MainView::resetEllipse()
{
    const int nPieces=10;
    _sourcePoly.clear();

    for(int i=nPieces-1;i>=0;--i)
    {
        const double t=2*M_PI/(nPieces)*i;

        _sourcePoly.push_back(QVector2D(0.7*cos(t),0.2+0.7*sin(t)));
    }


    _targetPoly.insert(_targetPoly.begin(),_sourcePoly.begin(), _sourcePoly.end());
}
#endif

MainView::~MainView()
{
#ifdef WIN32
    funs.glBindVertexArray(0);
    funs.glDeleteVertexArrays(1, &_vao);

    funs.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    funs.glDeleteBuffers(1, &_ibo);

    funs.glBindBuffer(GL_ARRAY_BUFFER,0);
    funs.glDeleteBuffers(1, &_vbo);
#else
    glBindVertexArrayAPPLE(0);
    glDeleteVertexArrays(1, &_vao);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &_ibo);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDeleteBuffers(1, &_vbo);
#endif

    if(_texture)
    {
        _texture->destroy();
        delete _texture;
    }

    _circleTexture->destroy();
    delete _circleTexture;
}

void MainView::toggleMorphMode()
{
    _morphMode = !_morphMode;

    _targetPoly.clear();

    if(_morphMode)
    {
        const int nEdges=_sourcePoly.size();
        QVector2D prev,tmp;

        float totalAngle=0;

        for(int i=0;i<nEdges;++i)
        {
            const int ip1=(i+1)%nEdges;
            prev=_sourcePoly[(i-1+nEdges)%nEdges];

            const QVector2D &current=_sourcePoly[i];
            const QVector2D &next=_sourcePoly[ip1];

            const QVector2D &prevE=current-prev;
            const QVector2D &nextE=next-current;

            const float alpha=atan2(prevE.x()*nextE.y()-prevE.y()*nextE.x(), QVector2D::dotProduct(nextE,prevE));

            totalAngle+=alpha;
        }

        if(totalAngle>0)
        {
            std::reverse(_sourcePoly.begin(), _sourcePoly.end());
        }



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
    const QFileInfo textureFile(path);
    setTexture(QImage(textureFile.absoluteFilePath()));
}

void MainView::setTexture(const QImage &imgIn)
{
    checkGLError("begin_setTexture");

    if(_texture)
    {
        _texture->destroy();
        delete _texture;
    }

    _morphMode=false;

    _targetPoly.clear();
    _sourcePoly.clear();

    const QImage img = imgIn.mirrored();
    _texture = new QOpenGLTexture(img);
    _texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    _texture->setMagnificationFilter(QOpenGLTexture::Linear);

    _grid.generate(img.width(),img.height(),this->width(),this->height(),_boundayPoly);

#ifdef TOUCH_SCREEN_MODE
    resetEllipse();
    toggleMorphMode();
#endif

    updateVBO();
    update();
    repaint();

    QTimer::singleShot(200, this, SLOT(update()));
}

void MainView::updateVBO()
{
    checkGLError("begin_genvao");

#ifdef WIN32
    funs.glBindVertexArray(_vao);
    checkGLError("end_genvao");

    funs.glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    funs.glBufferData(GL_ARRAY_BUFFER, _grid.vertices().size() * sizeof(GLfloat), &_grid.vertices()[0], GL_DYNAMIC_DRAW);

    funs.glVertexAttribPointer(_posLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    funs.glVertexAttribPointer(_textureLoc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));
    funs.glBindBuffer(GL_ARRAY_BUFFER, 0);
#else
    glBindVertexArrayAPPLE(_vao);
    checkGLError("end_genvao");

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, _grid.vertices().size() * sizeof(GLfloat), &_grid.vertices()[0], GL_DYNAMIC_DRAW);

    glVertexAttribPointer(_posLoc, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), 0);
    glVertexAttribPointer(_textureLoc, 2, GL_FLOAT, GL_FALSE, 4*sizeof(GLfloat), (void *)(2* sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER,0);
#endif
    checkGLError("end_bufferdata");
}

void MainView::initializeGL() {
    printf("Supported GLSL version is %s.\n", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
    checkGLError("initializeGL");

#ifdef WIN32
    funs.initializeOpenGLFunctions();
#endif
    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    // glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
    // glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
    // glEnable(GL_LINE_SMOOTH);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifdef TOUCH_SCREEN_MODE
    glClearColor(0, 0, 0, 1);
#else
    glClearColor(0.8, 0.8, 0.8, 1);
#endif

    checkGLError("begin_shaders");

    {
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
    }

    {
        QOpenGLShader vertShader(QOpenGLShader::Vertex);
        QFileInfo vertFile(":/shaders/circle.vert");
        vertShader.compileSourceFile(vertFile.absoluteFilePath());


        QOpenGLShader fragShader(QOpenGLShader::Fragment);
        QFileInfo fragFile(":/shaders/circle.frag");
        fragShader.compileSourceFile(fragFile.absoluteFilePath());

        _circleShader.addShader(&vertShader);
        _circleShader.addShader(&fragShader);

        _circleShader.link();
    }

    {
        QOpenGLShader vertShader(QOpenGLShader::Vertex);
        QFileInfo vertFile(":/shaders/color.vert");
        vertShader.compileSourceFile(vertFile.absoluteFilePath());


        QOpenGLShader fragShader(QOpenGLShader::Fragment);
        QFileInfo fragFile(":/shaders/color.frag");
        fragShader.compileSourceFile(fragFile.absoluteFilePath());

        _colorShader.addShader(&vertShader);
        _colorShader.addShader(&fragShader);

        _colorShader.link();
    }

    checkGLError("end_shaders");

#ifdef WIN32
    funs.glGenVertexArrays(1, &_vao);
    funs.glGenBuffers(1, &_vbo);

#ifdef TOUCH_SCREEN_MODE
    setTexture(PaperConstants::FACE_IMAGE);
#else
    setTexture(":/img/default");
#endif
    funs.glBindVertexArray(_vao);
    funs.glGenBuffers(1, &_ibo);
    funs.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
    funs.glBufferData(GL_ELEMENT_ARRAY_BUFFER, _grid.indices().size() * sizeof(GLuint), &_grid.indices()[0], GL_STATIC_DRAW);
    funs.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    checkGLError("end_ibufferdata");
#else
    checkGLError("begin_ibufferdata");
    glGenVertexArraysAPPLE(1, &_vao);
    glGenBuffers(1, &_vbo);
    checkGLError("end_ibufferdata");


#ifdef TOUCH_SCREEN_MODE
    setTexture(PaperConstants::FACE_IMAGE);
#else
    setTexture(":/img/default");
#endif

    {
        const QFileInfo textureFile(":/img/circle");
        const QImage img = QImage(textureFile.absoluteFilePath()).mirrored();
        _circleTexture = new QOpenGLTexture(img);
        _circleTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
        _circleTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    }




    checkGLError("begin_ibufferdata_1");

    glBindVertexArrayAPPLE(_vao);
    checkGLError("begin_ibufferdata_2");

    glGenBuffers(1, &_ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _grid.indices().size() * sizeof(GLuint), &_grid.indices()[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    checkGLError("end_ibufferdata");
#endif
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


void MainView::paintGL()
{
    const std::vector<QVector2D> &currentPoly = _morphMode?_targetPoly:_sourcePoly;

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(10);


    _shader.bind();
    setUniforms();

    _texture->bind();

#ifdef WIN32
    funs.glBindVertexArray(_vao);
    funs.glEnableVertexAttribArray(_posLoc);
    funs.glEnableVertexAttribArray(_textureLoc);
    funs.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
    funs.glDrawElements(GL_TRIANGLE_STRIP, _grid.indices().size(), GL_UNSIGNED_INT, (void*)0);

    funs.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    funs.glDisableVertexAttribArray(_posLoc);
    funs.glDisableVertexAttribArray(_textureLoc);
    funs.glBindVertexArray(0);
#else
    glBindVertexArrayAPPLE(_vao);
    glEnableVertexAttribArray(_posLoc);
    glEnableVertexAttribArray(_textureLoc);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
    glDrawElements(GL_TRIANGLE_STRIP, _grid.indices().size(), GL_UNSIGNED_INT, (void*)0  );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(_posLoc);
    glDisableVertexAttribArray(_textureLoc);
    glBindVertexArrayAPPLE(0);
#endif

    checkGLError("end_unbind");

    _shader.release();


    if(!_drawForPrinting)
    {
        _colorShader.bind();


        
#ifdef TOUCH_SCREEN_MODE
        glColor3f(245.0f/255.0f,128.0f/255.0f,37.0f/255.0f);
        glLineWidth(5);
#else
        glColor3f(0.5f,0.5f,0.5f);
        glLineWidth(_morphMode?2:10);
#endif
        if(currentPoly.size()>=3)
        {
            glBegin(GL_LINE_LOOP);
            for(int i=0;i<currentPoly.size();++i)
            {
                glVertex2f(currentPoly[i].x(),currentPoly[i].y());
            }
            glEnd();
        }




#ifndef TOUCH_SCREEN_MODE
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
#endif
        _colorShader.release();


        glEnable(GL_POINT_SPRITE);
        _circleShader.bind();

        GLuint circleLoc = _circleShader.uniformLocation("circleTexture");
        _circleShader.setUniformValue(circleLoc,0);
        _circleTexture->bind(0);

        {
#ifdef TOUCH_SCREEN_MODE
            glPointSize(30);
#else
            glPointSize(20);
#endif
            glColor3f(1.0f,0.0f,0.0f);
            glBegin(GL_POINTS);
            for(int i=0;i<currentPoly.size();++i)
            {
                glVertex2f(currentPoly[i].x(),currentPoly[i].y());
            }

            glEnd();  
        }
        _circleShader.release();
    }

    glPopMatrix();

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

