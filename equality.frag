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
	vec3 total = vec3( 0.0, 0.0, 0.0 );	
 
	int i = -2, j = -2;
	int channel;
	for(; i <= 2; i++){
		for(j = -2; j <= 2; j++) {
			float x = ( origin.x ) + ( float(i) * 1.0/width);
			float y = ( origin.y ) + ( float(j) * 1.0/height);
			vec2 pos = vec2( x, y );
			for(channel=0;channel<3; channel++) {
				float diff = abs( sample(origin)[channel] - sample(pos)[channel] );
				total[channel] += diff;
			}
		}
	}
	
	for(channel=0;channel<3; channel++) {
		if( total[channel] > 1.0 ) total[channel] = 1.0;
	}
	total.rgb = vec3(1.0,1.0,1.0)-total.rgb;

	gl_FragColor.rgb = total;
	gl_FragColor.a = 1.0;
}

