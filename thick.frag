uniform sampler2D t0;
uniform float height;
uniform float width;

vec4 pixels[25];
	
vec2 origin = gl_TexCoord[0].xy;

vec4 sample( vec2 pos ){
	if( pos.x < 0.0 || pos.y < 0.0 || pos.x > 1.0 || pos.y > 1.0 )
		return vec4( 0.0, 0.0, 0.0, 0.0 );
	return texture2D( t0, pos );
}

vec4 sampleRel( int x, int y ) {
	return sample(vec2(
		( origin.x ) + ( float(x) * 1.0/width),
		( origin.y ) + ( float(y) * 1.0/height)
	)
	);
}

void main() {
	gl_FragColor.a = 1.0;
	int channel;
	for(channel=0;channel<3; channel++) {
		gl_FragColor[channel] = 0.0;
		if( sample(origin)[channel] < 0.5  ) {
			continue;
		}
		if(
/*			sampleRel(-1,-1)[channel] > 0.5 ||*/
/*			sampleRel(-1,0)[channel] > 0.5 ||*/
			sampleRel(0,-1)[channel] > 0.5 
		)
		{
			gl_FragColor[channel] = 0.1;
			continue;
		}
		gl_FragColor[channel] = 1.0;
	}
}

