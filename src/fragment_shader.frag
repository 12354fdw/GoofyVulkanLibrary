#version 450

layout(set = 0, binding = 0) uniform transform {
    mat4 MVP;
    vec3 viewPos;
} transformUBO;

/*
layout(set = 0, binding = 1) uniform lighting {
    vec3 lightPos;
    vec3 lightColor;
} lightUBO;
*/
layout(location = 0) in vec3 inColor;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inPosition;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 normal = normalize(inNormal);
    vec3 lightPos = vec3(0,0,0);
    vec3 lightColor = vec3(1,1,1);
    float ambient = 0.08;
    float shininess = 128.0;

    vec3 lightDir   = normalize(lightPos - inPosition);
    vec3 viewDir    = normalize(transformUBO.viewPos - inPosition);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    vec3 specular = lightColor * spec * 0.35;
    float diffuse = max(dot(normal, lightDir),0.0);

    vec3 lighting = ambient * inColor + diffuse * lightColor * inColor + specular;

    //outColor = vec4(inColor, 1.0);
    outColor = vec4(lighting,1.0);
}