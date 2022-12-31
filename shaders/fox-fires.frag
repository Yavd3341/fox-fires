uniform vec2 pos;
uniform vec2 size;
uniform vec2 offsets;
uniform vec4 heights;

uniform vec4  colors[3];
uniform float time;
uniform float ctime;
uniform float seed;
uniform float strokes;

uniform bool f_fill;
uniform bool f_static;

vec3 mod289(vec3 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec2 mod289(vec2 x) { return x - floor(x * (1.0 / 289.0)) * 289.0; }
vec3 permute(vec3 x) { return mod289(((x*34.0)+1.0)*x); }

float noise(vec2 v) {
    const vec4 C = vec4(0.211324865405187,
                        0.366025403784439,
                        -0.577350269189626,
                        0.024390243902439);

    vec2 i  = floor(v + dot(v, C.yy));
    vec2 x0 = v - i + dot(i, C.xx);

    vec2 i1 = vec2(0.0);
    i1 = (x0.x > x0.y)? vec2(1.0, 0.0):vec2(0.0, 1.0);
    vec2 x1 = x0.xy + C.xx - i1;
    vec2 x2 = x0.xy + C.zz;

    i = mod289(i);
    vec3 p = permute(
            permute( i.y + vec3(0.0, i1.y, 1.0))
                + i.x + vec3(0.0, i1.x, 1.0 ));

    vec3 m = max(0.5 - vec3(
                        dot(x0,x0),
                        dot(x1,x1),
                        dot(x2,x2)
                        ), 0.0);

    m = m*m ;
    m = m*m ;

    vec3 x = 2.0 * fract(p * C.www) - 1.0;
    vec3 h = abs(x) - 0.5;
    vec3 ox = floor(x + 0.5);
    vec3 a0 = x - ox;

    m *= 1.79284291400159 - 0.85373472095314 * (a0*a0+h*h);

    vec3 g = vec3(0.0);
    g.x  = a0.x  * x0.x  + h.x  * x0.y;
    g.yz = a0.yz * vec2(x1.x,x2.x) + h.yz * vec2(x1.y,x2.y);
    return 130.0 * dot(m, g);
}

void main() {
	vec2 lc = (gl_FragCoord.xy - pos) / size;
	float timesin = (sin((time + seed * 10.) * 3.1415 * 2.) + 1.) / 2.;
	float z = (sin((lc.x + time + seed * 10.) * 3.1415 * 2.) + 1.) / 2.;
	float jiggle = (1. - z) * heights.x * .1;
	lc.y = 1. - min(1., lc.y + jiggle);
	vec2 slice = mix(heights.xy, heights.zw, lc.x);
	float offset = mix(offsets.x, offsets.y, lc.x);

    float ctimen = ctime / 86400. * 2.;
    float ctimena = abs(ctimen);
	
	if (lc.y < offset || ctimena <= 0.7)
		discard;
	
	lc.y -= offset;
	if (lc.y < slice.x)
		gl_FragColor.a = mix(0., 1., lc.y / slice.x);
	else if (lc.y > slice.x + slice.y) {
        float over = lc.y - slice.x - slice.y;
        float width = 0.075;
        if (over > width) discard;
        over /= width;
		gl_FragColor.a = 1. - over;
	}
	else
		gl_FragColor.a = 1.;
    
    if (ctimena < 0.8)
        gl_FragColor.a *= mix(0., 1., (ctimena - 0.7) * 10.);
    
    float cs;
    int col = 0;
    if (f_static)
        cs = gl_FragColor.a;
    else {
        float threshold = 1. - time;
        float a = threshold - 0.5, b = threshold;
        if (lc.x > threshold) {
            a += 1.;
            b += 1.;
            col += 1;
        }
        cs = smoothstep(a, b, lc.x);
    }

    vec4 col1 = colors[col];
    vec4 col2 = colors[col + 1];
    gl_FragColor.rgb = (cs * col2 + (1. - cs) * col1).rgb;

	gl_FragColor.a *= mix(.5, 1., z);

    if (!f_fill)
        gl_FragColor.a *= (noise(vec2(lc.x * strokes, timesin * 5.) + vec2(seed * 100.)) * .5 + .5);
	
	//gl_FragColor = vec4(vec3(cs), 1.);
}
