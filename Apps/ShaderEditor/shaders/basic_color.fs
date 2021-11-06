#version 330 core

uniform float time;
uniform vec2 resolution;

out vec4 FragColor;

in vec3 Pos;


// Credit to Danguafer https://www.shadertoy.com/view/XdB3zw
void main() {
    float t = time/4;
	vec2 p = (2.0*gl_FragCoord.xy-resolution.xy)/resolution.y;
	vec2 mp = vec2(0.5, 0.5)*0.5+0.5;
		
	float s = 1.0;
	for (int i=0; i < 7; i++) {
		s = max(s,abs(p.x)-0.375);
		p = abs(p*2.25)-mp*1.25;
		p *= mat2(cos(t+mp.x),-sin(t+mp.y),sin(t+mp.y),cos(t+mp.x));
	}
	
	vec3 col = vec3(4.0,2.0,1.0)/abs(atan(p.y,p.x))/s;
	
	FragColor = vec4(col,1.0);
}