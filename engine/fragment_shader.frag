// shadertype=glsl

precision highp float;

uniform float time;
uniform vec2 resolution;

const int MAX_STEPS = 32;
const float FAR_CULLING_PLANE = 100.0;
const float EPSILON = 0.001;

/* DISTANCE FIELDS */

float iPlane(vec3 p, vec3 normal, float dist) {
	return dot(p, normal) - dist;
}

float iSphere(vec3 p, float radius) {
	return length(p) - radius;
}

float iTorus(vec3 p, vec2 t) {
	vec2 q = vec2(length(p.xz) - t.x, p.y);
	return length(q) - t.y;
}

/* END DISTANCE FIELDS */

/* OPERATIONS */

mat4 rotation(in vec3 axis, in float angle) {
	axis = normalize(axis);
	float s = sin(angle);
	float c = cos(angle);
	float oc = 1.0 - c;
	
	 return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                 oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                 oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                 0.0,                                0.0,                                0.0,                                1.0);
}

vec3 opTx(vec3 p, mat4 m) {
	vec4 p4 = vec4(p, 1.0);
	vec4 result = inverse(m) * p4;

	return result.xyz;
}

/* END OPERATIONS */

float distanceField(vec3 p) {
	float d1 = iSphere(p, 0.5);
	float d2 = iTorus(opTx(p, rotation(vec3(1.0, 0.0, 0.0), 10.0 * sin(time * 0.02))), vec2(0.5, 0.2));

	return d2;
}

float raymarch(vec3 ro, vec3 rd) {
	float t = 0.0f;
	int steps;
	for (steps = 0; steps < MAX_STEPS; steps++) {
		vec3 p = ro + rd * t;
	    float d = distanceField(p);

		if (d >= FAR_CULLING_PLANE) {
			break;
		}

		if (d < EPSILON) {
			break;
		}

		t += d;
	}

	return 1.0 - float(steps) / float(MAX_STEPS);
}

void main(void) {
	vec2 q = gl_FragCoord.xy / resolution.xy;
	q *= resolution.xy / resolution.x;
	vec2 uv = -1.0 + 2.0 * q;

	vec3 eye = vec3(0.0, 0.0, -1.0);
	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = vec3(1.0, 0.0, 0.0);

	vec3 ro = right * uv.x + up * uv.y;
	ro.z = eye.z;
	vec3 rd = normalize(cross(right, up));

	// Sky color
	vec3 colour = vec3(0.2, 0.4, 0.8);

	float t = raymarch(ro, rd);

	if (t != 0) {
		colour = vec3(t, 0.2, 0.2);
	}

	outColour = vec4(colour, 1.0);
}