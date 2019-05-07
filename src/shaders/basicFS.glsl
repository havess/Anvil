#version 330 core

out vec4 colour;

in vec3 Norm;
in vec3 FragPos;

uniform vec3 objectColour;
uniform vec3 lightColour;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main(){

  float ambientStren = 0.1;
  vec3 ambient = ambientStren * lightColour;

  vec3 n = normalize(Norm);
  vec3 lightDir = normalize(lightPos - FragPos);
  float diff = max(dot(n, lightDir), 0.0);
  vec3 diffuse = diff * lightColour;

  float specularStrength = 0.5;
  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, n);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
  vec3 specular = specularStrength * spec * lightColour;  

  colour = vec4((ambient+diffuse+spec) * objectColour, 1.0);
}