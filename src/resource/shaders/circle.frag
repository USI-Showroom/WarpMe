#version 120

#define MAX_SELECTIONS 15

uniform sampler2D circleTexture;

void main()
{
    vec4 spriteColor=texture2D(circleTexture, gl_PointCoord);
    gl_FragColor = spriteColor;
    // gl_FragColor.a = spriteColor.g;
}

