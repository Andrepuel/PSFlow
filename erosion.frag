uniform sampler2D t0;
uniform float width;
uniform float height;

void checkNeighbour( float x, float y ){
	vec2 pos = gl_TexCoord[0].xy + vec2( x/width, y/height );
	if( texture2D( t0, pos ).r == 0.0 ){
		gl_FragColor = vec4( 0.0, 0.0, 0.0, 0.0 );
	}
}

void main(){		
	gl_FragColor = vec4( 1.0, 1.0, 1.0, 1.0 );
	
	checkNeighbour( 0.0, 0.0 );	
	checkNeighbour( 1.0, 0.0 );
	checkNeighbour( -1.0, 0.0 );
	checkNeighbour( 0.0, 1.0 );
	checkNeighbour( 0.0, -1.0 );

	return;
}
