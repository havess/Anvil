#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 lightModel;
uniform vec3 lightPos;
uniform vec3 viewPos;

out vec3 Norm;
out vec3 FragPos;
out vec3 LightPos;
out vec3 ViewPos;
out vec2 TexCoord;
  
void main(){
  gl_Position = proj * view * model * vec4(pos, 1.0);
  FragPos = vec3(model * vec4(pos, 1.0));
  LightPos = vec3(lightModel * vec4(lightPos, 1.0));
  ViewPos = viewPos;
  Norm = mat3(transpose(inverse(model))) * normal; 
  TexCoord = uv;
}
