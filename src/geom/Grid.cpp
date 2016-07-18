// Copyright (c) 2015 Teseo Schneider (Universita della Svizzera italiana)
// -------------------------------------------------------------------------------------------
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
// -----------------------------------------------------------------------

#include "Grid.hpp"
#include <cassert>

Grid::Grid(const int stepX, const int stepY)
:_stepX(stepX), _stepY(stepY)
{

}

Grid::~Grid()
{

}

void Grid::generateVertices(const float width, const float height, const float targetW, const float targetH,std::vector<QVector2D> &poly)
{
    assert(poly.size()==4);

	float xx = targetW/width;
	float yy = targetH/height;

	float w=2, h=2;

	if(xx<yy)
	{
		h = 2/width*height/targetH*targetW;
	}
	else
	{
		w = 2/height*width/targetW*targetH;
	}

	

	const float offsetX = -1.0f + (2.0f-w)/2.0f;
	const float offsetY = -1.0f + (2.0f-h)/2.0f;

    poly[0]=QVector2D(offsetX  ,offsetY);
    poly[1]=QVector2D(offsetX+w,offsetY);
    poly[2]=QVector2D(offsetX+w,offsetY+h);
    poly[3]=QVector2D(offsetX  ,offsetY+h);

	const int size=nVertices();
	_vertices.resize(2*size);

	const float scalingX=1.0/(_stepX-1.0);
	const float scalingY=1.0/(_stepY-1.0);
    
    int index = 0;

    for(int row=0; row<_stepY; row++ )
    {
        for(int col=0; col<_stepX; col++ )
        {
            _vertices[index++] = w * scalingX * col + offsetX;
            _vertices[index++] = h * scalingY * row + offsetY;

            _vertices[index++] = scalingX * col;
            _vertices[index++] = scalingY * row;
        }
    }

    assert(index==_vertices.size());
}

void Grid::generateIndices()
{
    _indices.resize(nIndices());

    int index = 0;

    for(int row=0; row<_stepY-1; row++)
    {
        if ((row&1)==0) // even rows
        { 
            for(int col=0; col<_stepX; col++ )
            {
                _indices[index++] = col + row * _stepX;
                _indices[index++] = col + (row+1) * _stepX;
            }
        }
        else
        { // odd rows
            for(int col=_stepX-1; col>0; col-- )
            {
                _indices[index++] = col + (row+1) * _stepX;
                _indices[index++] = col - 1 + + row * _stepX;
            }
        }
    }
    assert(index==_indices.size());
    // if ( (mHeight&1) && mHeight>2 ) {
        // mpIndices[i++] = (mHeight-1) * mWidth;
    // }
}