// Copyright (c) 2015 Teseo Schneider (Universita della Svizzera italiana)
// -------------------------------------------------------------------------------------------
// You can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation,
// either version 3 of the License, or (at your option) any later version.
// -----------------------------------------------------------------------

#version 120

varying vec2 txtCoord;
uniform sampler2D textureSampler;

void main()
{
    vec2 txt=txtCoord;
    gl_FragColor = texture2D(textureSampler, txt);
}
