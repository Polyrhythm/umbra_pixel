// shadertype=glsl

#ifdef GL_ES
precision highp float;
#endif

uniform float time;
uniform vec2 resolution;

out vec4 gl_FragColor;

const int MAX_STEPS = 16;
const float EPSILON = 0.001;

float iSphere(vec3 p, float radius) {
	return length(p) - radius;
}

float distanceField(vec3 p) {
	float d1 = iSphere(p, 0.5);

	return d1;
}

bool raymarch(vec3 ro, vec3 rd) {
	float t = 0.0f;
	for (int i = 0; i < MAX_STEPS; ++i) {
		vec3 p = ro + rd * t;
	    float d = distanceField(p);

		if (d < EPSILON) {
			return true;
		}

		t += d;
	}

	return false;
}

void main(void) {
	vec2 q = gl_FragCoord.xy / resolution.xy;
	q *= resolution.xy / resolution.x;
	vec2 uv = -1.0 + 2.0 * q;

	vec3 eye = vec3(0.0, 0.0, -1.0);
	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = vec3(1.0, 0.0, 0.0);

	vec3 ro = right * uv.x + up * uv.y + vec3(0.0, 0.1 * sin(time), 0.0);
	vec3 rd = normalize(cross(right, up));

	// Sky color
	vec3 colour = vec3(0.2, 0.4, 0.8);

	bool t = raymarch(ro, rd);

	if (t) {
		colour = vec3(0.8, 0.2, 0.4);
	}

	gl_FragColor = vec4(colour, 1.0);
}