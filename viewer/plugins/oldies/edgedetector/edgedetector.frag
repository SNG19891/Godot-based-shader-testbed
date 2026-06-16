uniform sampler2D colorMap;
uniform float TEXTURE_SIZE;

void main()
{
    vec2 st = (gl_FragCoord.xy - vec2(0.5)) / TEXTURE_SIZE;
    float a = 6.0/TEXTURE_SIZE;

    float col  = texture2D(colorMap, st+vec2(0,0)).r;
    float colx = texture2D(colorMap, st+vec2(a,0)).r;
    float coly = texture2D(colorMap, st+vec2(0,a)).r;

    float dx = colx - col;
    float dy = coly - col;
    
    float grad = length(vec2(dx,dy));
            
    gl_FragColor = vec4(1-grad);
    
}

