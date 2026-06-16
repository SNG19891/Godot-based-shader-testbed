uniform sampler2D colorMap;
uniform float SIZE;
uniform float time;

void main()
{
    vec2 st = gl_FragCoord.xy / SIZE;
    float a = 0.01*sin(10.0*time+ 30.0*st.s);
    gl_FragColor = 0.4*texture2D(colorMap, st+vec2(a,a));    
}

