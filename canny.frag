uniform sampler2D t0;
uniform float height;
uniform float width;

vec4 pixels[25];

vec4 sample( vec2 pos ){
	if( pos.x < 0.0 || pos.y < 0.0 || pos.x > 1.0 || pos.y > 1.0 )
		return vec4( 0.0, 0.0, 0.0, 0.0 );
	return texture2D( t0, pos );
}

void main() {
	vec2 origin = gl_TexCoord[0].xy;

 
	int i = -2, j = -2;
	for(; i <= 2; i++){
		for(j = -2; j <= 2; j++) {
			float x = ( origin.x ) + ( float(i) * 1.0/width);
			float y = ( origin.y ) + ( float(j) * 1.0/height);
			vec2 pos = vec2( x, y );
			pixels[ 5*(i+2) + (j+2) ] = sample( pos );
		}
	}
		
	pixels[0] *= 2.0;		
	pixels[1] *= 4.0;		
	pixels[2] *= 5.0;		
	pixels[3] *= 4.0;		
	pixels[4] *= 2.0;		

	pixels[5] *= 4.0;		
	pixels[6] *= 9.0;		
	pixels[7] *= 12.0;		
	pixels[8] *= 9.0;		
	pixels[9] *= 4.0;		

	pixels[10] *= 5.0;		
	pixels[11] *= 12.0;		
	pixels[12] *= 15.0;		
	pixels[13] *= 12.0;		
	pixels[14] *= 5.0;		

	pixels[15] *= 4.0;		
	pixels[16] *= 9.0;		
	pixels[17] *= 12.0;		
	pixels[18] *= 9.0;		
	pixels[19] *= 4.0;		

	pixels[20] *= 2.0;		
	pixels[21] *= 4.0;		
	pixels[22] *= 5.0;		
	pixels[23] *= 4.0;		
	pixels[24] *= 2.0;		

	vec4 total = vec4( 0.0, 0.0, 0.0, 0.0 );	
	for(i = 0; i < 5; i++){
		for(j = 0; j < 5; j++){
			total += pixels[i*5 + j];
		}	
	}
	total /= 159.0;

	gl_FragColor = total;
}

