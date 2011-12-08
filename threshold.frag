uniform sampler2D t0;

uniform float p0;
uniform float p1;

void main() {
	gl_FragColor = texture2D( t0,  gl_TexCoord[0].xy );

	gl_FragColor.a = 1.0;

	int i = 0;
	for(;i<3;++i) {
		if( gl_FragColor[i] > p1 || gl_FragColor[i] < p0 ) {
			gl_FragColor[i] = 0.0;
		} else {
			gl_FragColor[i] = 1.0;
		}
	}
	return;
}
