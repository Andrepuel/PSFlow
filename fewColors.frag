uniform sampler2D t0;

uniform float p0;

void main() {
	float realAmount = p0-1;

	gl_FragColor = texture2D( t0,  gl_TexCoord[0].xy );

	gl_FragColor.a = 1.0;

	int i = 0;
	for(;i<3;++i) {
		gl_FragColor[i] = round(gl_FragColor[i]*realAmount)/realAmount;
	}
}
