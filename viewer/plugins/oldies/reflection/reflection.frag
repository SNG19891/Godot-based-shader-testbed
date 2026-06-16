uniform sampler2D colorMap;
varying vec4 P;
void main()
{
    vec4 clip = gl_ModelViewProjectionMatrix * P;
    vec3 ndc = clip.xyz / clip.w;
    vec2 st = ndc.xy * 0.5 + vec2(0.5);

    gl_FragColor = 0.7*texture2D(colorMap, st);
}

