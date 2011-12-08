uniform sampler2D t0;
uniform sampler2D t1;

void main() {
	gl_FragColor = texture2D( t0,  gl_TexCoord[0].xy );
	vec4 other = texture2D( t1,  gl_TexCoord[0].xy );

	gl_FragColor.a = 1.0;

	int i = 0;
	for(;i<3;++i) {
		gl_FragColor[i] *= other[i];
	}
	return;
}
