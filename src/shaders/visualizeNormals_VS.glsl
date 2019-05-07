#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

out VS_OUT {
    vec3 normal;
} vs_out;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

void main()
{
    gl_Position = proj * view * model * vec4(pos, 1.0); 
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vs_out.normal = normalize(vec3(proj * vec4(normalMatrix * normal, 0.0)));
}