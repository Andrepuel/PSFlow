uniform sampler2D t0;

void main() {
	vec4 color = texture2D( t0,  gl_TexCoord[0].xy );
	int i;

	float total = color[0];
	for(i=1;i<3;++i) {
		if( color[i] > total)
			total = color[i];
	}

	gl_FragColor.r = total;
	gl_FragColor.g = total;
	gl_FragColor.b = total;

	return;
}
