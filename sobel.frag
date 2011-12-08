uniform sampler2D t0;
uniform float height;
uniform float width;

vec4 gx[9];
vec4 gy[9];

vec4 sample( vec2 pos ){
	if( pos.x < 0.0 || pos.y < 0.0 || pos.x > 1.0 || pos.y > 1.0 )
		return vec4( 0.0, 0.0, 0.0, 0.0 );
	return texture2D( t0, pos );
}

void main() {
	vec2 origin = gl_TexCoord[0].xy;

	int i = -1, j = -1;
	for(; i <= 1; i++){
		for(j = -1; j <= 1; j++) {
			float x = ( origin.x ) + ( float(i) * 1.0/width);
			float y = ( origin.y ) + ( float(j) * 1.0/height);
			vec2 pos = vec2( x, y );
			gx[ 3*(i+1) + (j+1) ] = sample( pos );
			gy[ 3*(i+1) + (j+1) ] = sample( pos );
		}
	}
	
	gx[0] *= -1.0;		
	gx[1] *= -2.0;		
	gx[2] *= -1.0;		

	gx[3] *= 0.0;		
	gx[4] *= 0.0;		
	gx[5] *= 0.0;		

	gx[6] *= 1.0;		
	gx[7] *= 2.0;		
	gx[8] *= 1.0;		

	gy[0] *= -1.0;		
	gy[1] *= 0.0;		
	gy[2] *= 1.0;		

	gy[3] *= -2.0;		
	gy[4] *= 0.0;		
	gy[5] *= 2.0;		

	gy[6] *= -1.0;		
	gy[7] *= 0.0;		
	gy[8] *= 1.0;		
		
	vec4 totalX = vec4( 0.0, 0.0, 0.0, 0.0 );
	vec4 totalY = vec4( 0.0, 0.0, 0.0, 0.0 );

	int k;
	for(i = 0; i < 3; i++){
		for(j = 0; j < 3; j++){
			totalX.r += gx[i*3 + j].r;
			totalY.r += gy[i*3 + j].r;
			totalX.g += gx[i*3 + j].g;
			totalY.g += gy[i*3 + j].g;
			totalX.b += gx[i*3 + j].b;
			totalY.b += gy[i*3 + j].b;
		}	
	}
	
	float totalR = sqrt( pow( totalX.r, 2) + pow( totalY.r, 2)  );
	float totalG = sqrt( pow( totalX.g, 2) + pow( totalY.g, 2)  );
	float totalB = sqrt( pow( totalX.b, 2) + pow( totalY.b, 2)  );
	gl_FragColor.r = totalR;
	gl_FragColor.g = totalG;
	gl_FragColor.b = totalB;
	gl_FragColor.a = 1.0;	
}

