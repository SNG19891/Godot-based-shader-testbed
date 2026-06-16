uniform sampler2D colorMap;

uniform float TEXTURE_SIZE;

void main()
{
    vec2 st = (gl_FragCoord.xy - vec2(0.5)) / TEXTURE_SIZE;
    float a = 6.0/TEXTURE_SIZE;

    float color = texture2D(colorMap,st).r;
    float colorX = texture2D(colorMap,st + vec2(a,0)).r;
    float colorY = texture2D(colorMap,st + vec2(0,a)).r;

    vec2 gradient = vec2(colorX - color, colorY - color);

    gl_FragColor = vec4(1-length(gradient));
    
}

