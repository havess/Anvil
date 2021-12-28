#version 330 core

uniform float time;
uniform vec2 resolution;

out vec4 FragColor;

in vec3 Pos;

#define div 20.0f
#define grid_tolerance 1.0f

float plot(vec2 uv) {
	float y = (uv.y - 0.5) * 2.2;
	float x = uv.x - 0.5;
	return smoothstep(0.01, 0.0, abs(y - sin((x + time/6) * 4 * radians(180))));
}

bool on_grid(vec2 uv) {
	float x = uv.x * resolution.x;
	float y = uv.y * resolution.y;
	float nearest_x = round(x / div) * div;
	float nearest_y = round(y / div) * div;

	return  (abs(x - nearest_x) <= grid_tolerance) ||
			(abs(y - nearest_y) <= grid_tolerance);
}

void main() {
	vec2 uv = gl_FragCoord.xy/resolution;

	vec3 grid_col = {0.4, 0.4, 0.7};
	vec3 bg_col = {0.0, 0.2, 0.4};

	vec3 col = bg_col;
	if (on_grid(uv)) {
		col = grid_col;
	}

	float pct = plot(uv);
	col = (1.0 - pct)*col + pct*vec3(1.0f);
	FragColor = vec4(col,1.0);
}