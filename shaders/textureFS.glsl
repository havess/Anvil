#version 330 core

out vec4 colour;

in vec3 Norm;
in vec3 FragPos;
in vec3 ViewPos;
in vec3 LightPos;
in vec2 TexCoord;

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float sheen;
};

uniform Material mat;
uniform vec3 lightColour;
uniform sampler2D text;

void main(){

  vec3 ambient = mat.ambient * lightColour;
  ambient = vec3(0, 0, 0);

  vec3 n = normalize(Norm);
  vec3 lightDir = normalize(LightPos - FragPos);
  float diff = max(dot(n, lightDir), 0.0);
  vec3 diffuse = (diff*vec3(texture(text, TexCoord))) * lightColour;

  vec3 viewDir = normalize(ViewPos - FragPos);
  vec3 reflectDir = reflect(-lightDir, n);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), mat.sheen);
  vec3 specular = (mat.specular * spec) * lightColour;  

  colour = vec4(ambient+diffuse+spec, 1.0);
}
