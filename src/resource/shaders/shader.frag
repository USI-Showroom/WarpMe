#version 120

varying vec2 txtCoord;
uniform sampler2D textureSampler;

void main()
{
    vec2 txt=txtCoord;
    gl_FragColor = texture2D(textureSampler, txt);
    
    // float nSquares=50.0;
    // txt*=nSquares;

    // int tx=int(floor(txt.x));
    // int ty=int(floor(txt.y));

    // int sum=tx+ty;
    // int div=sum/2;
    // int modulus=sum-2*div;

    // float val = float((modulus < 1) ? 0.5 : 1);
    // gl_FragColor = vec4(val,val,val,1);    
    // gl_FragColor = vec4(txt,0,1);
}
