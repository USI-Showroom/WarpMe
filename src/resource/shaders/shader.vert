// Copyright (c) 2015 Teseo Schneider (Universita della Svizzera italiana)
// -------------------------------------------------------------------------------------------
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
// -----------------------------------------------------------------------

#version 120

#define MAX_VERTICES 50
#define EPSILON 1e-3

uniform int nVertices;
uniform vec2 source[MAX_VERTICES];
uniform vec2 target[MAX_VERTICES];

uniform vec2 bounday[4];
uniform bool preserveBoundary;


attribute vec2 posIn;
attribute vec2 textureIn;

varying vec2 txtCoord;


vec2 computePosition(vec2 point)
{       
	int ip1, im1;

	vec2 s[MAX_VERTICES+4];
	float r[MAX_VERTICES+4];
	float t[MAX_VERTICES+4];


	for(int i=0;i<nVertices;++i)
	{
		s[i]=source[i]-point;
		r[i]=length(s[i]);

		if(r[i]<EPSILON){
			return target[i];
		}
	}


	if(preserveBoundary){
		for(int i=0;i<4;++i)
		{
			s[nVertices+i]=bounday[i]-point;
			r[nVertices+i]=length(s[nVertices+i]);

			if(r[nVertices+i]<EPSILON){
				return bounday[i];
			}
		}   
	}


	float Ai, Di;

	for(int i=0;i<nVertices;++i)
	{   
		ip1 = (i+1>=nVertices) ? 0 : (i+1);

		Ai=s[i].x*s[ip1].y-s[ip1].x*s[i].y;
		Di=dot(s[i],s[ip1]);

		if(abs(Ai)<EPSILON*EPSILON && Di<0){
			return (r[ip1]*target[i]+r[i]*target[ip1])/(r[i]+r[ip1]);
		}

		t[i]=Ai/(r[i]*r[ip1]+Di);
	}

	if(preserveBoundary){
		for(int i=0;i<4;++i)
		{   
			ip1 = (i+1>=4) ? 0 : (i+1);

			Ai=s[nVertices+i].x*s[nVertices+ip1].y-s[nVertices+ip1].x*s[nVertices+i].y;
			Di=dot(s[nVertices+i],s[nVertices+ip1]);

			if(abs(Ai)<EPSILON*EPSILON && Di<0){
				return (r[nVertices+ip1]*bounday[i]+r[nVertices+i]*bounday[ip1])/(r[nVertices+i]+r[nVertices+ip1]);
			}

			t[nVertices+i]=Ai/(r[nVertices+i]*r[nVertices+ip1]+Di);
		}
	}



	
	vec2 res=vec2(0,0);
	float totalW=0;
	float w;



	for(int i=0;i<nVertices;++i)
	{  
		im1 = (i > 0) ? (i-1) : (nVertices-1);

		w=(t[im1]+t[i])/r[i];
		res+=w*target[i];
		totalW+=w;
	}

	if(preserveBoundary){
		for(int i=0;i<4;++i)
		{  
			im1 = (i > 0) ? (i-1) : (3);

			w=(t[nVertices+im1]+t[nVertices+i])/r[nVertices+i];
			res+=w*bounday[i];
			totalW+=w;
		}
	}

	res/=totalW;
	
	return res;
}



void main()
{
	if(nVertices<3)
		gl_Position = vec4(posIn,0,1);
	else
		gl_Position = vec4(computePosition(posIn),0,1);

	txtCoord = textureIn;
}
