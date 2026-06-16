varying vec4 P;

void main()
{
    P = gl_Vertex;
	gl_Position    = gl_ModelViewProjectionMatrix * gl_Vertex;
    //vec3 ndc = gl_Position.xyz / gl_Position.w;
    //gl_TexCoord[0].st = ndc.xy * 0.5 + vec2(0.5);
}

