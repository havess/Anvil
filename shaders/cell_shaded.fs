#version 330 core
out vec4 FragColor;

uniform vec3 lightColour;

in vec3 Pos;
in vec3 Normal;

void main()
{
    vec3 lightPos = vec3(5, 5, 5);
    float intensity = dot(normalize(Normal), normalize(lightPos - Pos));
    if (intensity < 0.2) {
        intensity = 0.2;
    } else if (intensity < 0.6) {
        intensity = 0.6;
    } else {
        intensity = 1.0;
    }
    FragColor = intensity * vec4(lightColour, 1.0); // set all 4 vector values to 1.0
}
