#version 460

layout (location = 0) out vec4 color;

in vec3 fragPos;
in vec3 normalVec;

void main() {
    //vec3 lightPos = vec3(128.0, -128.0, 0.0);
    //vec3 lightDir = normalize(fragPos.xyz - lightPos);
    vec3 lightDir  = normalize(vec3(1.0, 1.0, 0.0));
    float diffuse  = max(0.0, dot(lightDir, normalVec));

    vec4 waterColor = vec4(0.1, 0.3, 0.7, 1.0);
    color = diffuse * waterColor;
}