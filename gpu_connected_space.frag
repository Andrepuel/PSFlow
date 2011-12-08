uniform sampler2D t0;
uniform float height;
uniform float width;

vec4 sample( vec2 pos ){
	if( pos.x < 0.0 || pos.y < 0.0 || pos.x > 1.0 || pos.y > 1.0 )
		return vec4( 0.0, 0.0, 0.0, 0.0 );
	return texture2D( t0, pos );
}

void main() {
	vec2 origin = gl_TexCoord[0].xy;
 	float xSup = ( origin.x ) + ( float(0) * 1.0/width);
	float ySup = ( origin.y ) + ( float(-1) * 1.0/height);
	float xLef = ( origin.x ) + ( float(-1) * 1.0/width);
	float yLef = ( origin.y ) + ( float(0) * 1.0/height);
	float xRig = ( origin.x ) + ( float(+1) * 1.0/width);
	float yRig = ( origin.y ) + ( float(0) * 1.0/height);

	gl_FragColor = vec4(0.0,0.0,0.0,1.0);

	if( sample(origin) == sample(vec2(xSup,ySup)) ) {
		gl_FragColor[1] = 1.0;
	}
	if( sample(origin) == sample(vec2(xLef,yLef)) ) {
		gl_FragColor[1] = 1.0;
	}
	if( sample(origin) == sample(vec2(xRig,yRig)) ) {
		gl_FragColor[1] = 1.0;
	}
}

