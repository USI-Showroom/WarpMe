// Copyright (c) 2015 Teseo Schneider (Universita della Svizzera italiana)
// -------------------------------------------------------------------------------------------
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
// -----------------------------------------------------------------------

#ifndef REGULAR_GRID_HPP__
#define REGULAR_GRID_HPP__

#include <vector>
#include <QVector2D>
#include <QtOpenGL>

class Grid
{
public:
    Grid(const int stepX, const int stepY);
    ~Grid();

    inline void generate(const float width, const float height, const float targetW, const float targetH, std::vector<QVector2D> &poly)
    {
        generateVertices(width, height, targetW, targetH, poly);
        generateIndices();
    }

    inline const std::vector<float> &vertices() const { return _vertices; }
    inline const std::vector<GLuint> &indices() const { return _indices; }

    inline int nVertices() const { return _stepX * _stepY * 2; }
    inline int nIndices() const { return (_stepX * _stepY) + (_stepX - 1) * (_stepY - 2); }

private:
    void generateVertices(const float width, const float height, const float targetW, const float targetH, std::vector<QVector2D> &poly);
    void generateIndices();

    int _stepX, _stepY;
    std::vector<float> _vertices;
    std::vector<GLuint> _indices;
};
#endif //REGULAR_GRID_HPP__