#version 450

layout(binding = 0) uniform UBO {
    mat4 MVP;
    vec3 viewPos;
    vec3 viewDir;
} ubo;

layout(location = 0) in vec3 inColor;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inPosition;

layout(location = 0) out vec4 outColor;

void main()
{
    vec3 lightPos = vec3(100.0, 100.0, 0.0);
    vec3 lightDir = vec3(0.0, -1.0, 0.0)
    vec3 lightColor = vec3(1.0, 1.0, 1.0); // do not go below 0.1f

    float ambient = 0.05;
    float shininess = 2.0;

    vec3 lightDir   = normalize(lightPos - inPosition);
    vec3 viewDir    = normalize(viewPos - inPosition);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float spec = pow(max(dot(inNormal, halfwayDir), 0.0), shininess);
    vec3 specular = lightColor * spec;
    outColor = vec4((inColor * specular) + ambient, 1.0);
}