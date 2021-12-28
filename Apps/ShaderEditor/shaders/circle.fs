#version 330 core
#define PI 3.14159265358979323846

uniform float time;
uniform vec2 resolution;

out vec4 FragColor;

in vec3 Pos;

float angle(vec2 coord) {
	float pi = acos(-1.0);
	float rad = atan(coord.y/coord.x);
	if (coord.x < 0) {
		rad = -pi + rad;
	}
	return rad;
}

float circle(vec2 coord, float radius) {
    return step(length(coord) + (sin(angle(coord)*5)+sin(angle(coord)*7) + sin(angle(coord)*14)) * 0.05, radius);
}

void main() {
	vec2 uv = gl_FragCoord.xy/resolution;
	vec3 col = vec3(0.2, 0.2, 0.2);
	FragColor = vec4(col*circle(uv - vec2(0.5, 0.5), 0.3), 1.0);
}