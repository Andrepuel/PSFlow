uniform sampler2D t0;

void main() {
	vec4 color = texture2D( t0,  gl_TexCoord[0].xy );
	int i;

	float total = 0.0;
	for(i=0;i<3;++i) {
		total += color[i];
	}
	total /= 3.0;

	gl_FragColor.r = total;
	gl_FragColor.g = total;
	gl_FragColor.b = total;

	return;
}
