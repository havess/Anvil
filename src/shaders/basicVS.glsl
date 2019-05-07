#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 Norm;
out vec3 FragPos;
  
void main(){
  gl_Position = proj * view * model * vec4(pos, 1.0);
  FragPos = vec3(model * vec4(pos, 1.0));
  Norm = mat3(transpose(inverse(model))) * normal; 
}