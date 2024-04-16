layout (location = 0) out vec4 color;

in vec4 surfaceNormal;

void main() {
    //vec3 lightPos = vec3(2.0, 128.0, 0.0);
    //vec3 lightDir = normalize(finalPos.xyz - lightPos);
    vec3 norm = vec3(surfaceNormal);
    vec3 lightDir = normalize(vec3(-2.0, -1.0, 0.0));
    float shadow  = max(0.01, dot(-lightDir, norm));

    vec4 waterColor = vec4(0.1, 0.3, 0.7, 1.0);
    color = shadow * waterColor;
}