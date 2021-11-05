#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec3 LightPos;
} fs_in;

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float sheen;
};

uniform Material mat;
uniform sampler2D diffuseTexture;
uniform samplerCube depthMap;

uniform vec3 viewPos;
uniform float far_plane;

uniform bool hasTexture;
uniform bool shadows;

float isInShadow(vec3 fragPos)
{
    vec3 fragToLight = fragPos - fs_in.LightPos;
    float closestDepth = texture(depthMap, fragToLight).r;
    closestDepth *= far_plane;
    float currentDepth = length(fragToLight);
    float bias = 0.05; 
    if(shadows == false){
      return 0.0;
    }
    return (currentDepth -  bias) > closestDepth ? 1.0 : 0.0;
}

void main()
{           
    vec3 color = hasTexture ? texture(diffuseTexture, fs_in.TexCoords).rgb : mat.diffuse;
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);

    // ambient
    vec3 ambient = 0.3 * color;

    // diffuse
    vec3 lightDir = normalize(fs_in.LightPos - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), (hasTexture ? 64.0:mat.sheen));
    vec3 specular = spec * lightColor;    

    vec3 lighting = (ambient + (1.0 - isInShadow(fs_in.FragPos)) * (diffuse + specular)) * color;    
    
    FragColor = vec4(lighting, 1.0);
}  
