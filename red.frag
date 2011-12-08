uniform sampler2D t0;

void main() {
	gl_FragColor = texture2D( t0,  gl_TexCoord[0].xy );
	gl_FragColor.g = 0.0;
	gl_FragColor.b = 0.0;

	return;
}
