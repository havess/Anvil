#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in float sign;
layout(location = 2) in vec3 prev;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform float thickness;
uniform float aspect;

void main(){
  mat4 mvp = proj * view * model;
  vec4 prevProj = mvp * vec4(prev, 1.0);
  vec4 curProj = mvp * vec4(pos, 1.0);
  //vec4 nextProj = mvp * vec4(next, 1.0);

  // de-homogenize
  vec2 aspectv = vec2(aspect, 1.0);
  vec2 prevScreen = prevProj.xy/prevProj.w * aspectv;
  vec2 curScreen = curProj.xy/curProj.w * aspectv;
  //vec2 nextScreen = nextProj.xy/nextProj.w * aspectv;

  vec2 dir = normalize(curScreen - prevScreen);
  vec2 normal = vec2(-dir.y, dir.x) * thickness * 0.5;
  normal.x /= aspect;

  gl_Position = curProj + vec4(normal * sign, 0.0, 0.0);
  //gl_Position = curProj + vec4(0.0, sign, 0.0, 0.0);
}
