#version 120

varying vec2 txtCoord;
uniform sampler2D textureSampler;

void main()
{
    vec2 txt=txtCoord;
    gl_FragColor = texture2D(textureSampler, txt);
}
