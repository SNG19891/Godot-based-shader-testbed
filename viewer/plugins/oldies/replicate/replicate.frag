uniform sampler2D colorMap;
uniform float TEXTURE_SIZE;

void main()
{
   vec2 st = 3.0*(gl_FragCoord.xy - vec2(0.5)) / TEXTURE_SIZE;

   vec4 c = texture2D(colorMap, st);

   gl_FragColor = texture2D(colorMap, st);
    
}

