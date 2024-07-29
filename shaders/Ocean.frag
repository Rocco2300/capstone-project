layout (location = 0) out vec4 color;

in vec3 viewPosition;
in vec3 surfaceNormal;
in vec3 fragmentPosition;

uniform int wireframe;
uniform double azimuth;
uniform double inclination;

void main() {
    vec3 normal = normalize(vec3(surfaceNormal));
    vec3 viewPos = vec3(viewPosition);
    vec3 fragPos = vec3(fragmentPosition);

    float radius = 1.0;
    float azimuthFlt = float(azimuth);
    float inclinationFlt = float(inclination);
    float lightX = radius * sin(inclinationFlt) * cos(azimuthFlt);
    float lightY = radius * cos(inclinationFlt);
    float lightZ = radius * sin(inclinationFlt) * sin(azimuthFlt);

    vec3 lightDir = normalize(vec3(lightX, lightY, lightZ));
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    vec4 waterColor = vec4(0.1, 0.3, 0.7, 1.0);
    vec4 sunlightColor = vec4(1.0, 0.99, 0.87, 1.0);
    vec4 ambientLightColor = vec4(1.0, 0.34, 0.2, 1.0);

    float ambientStrength = 0.3;
    float specularStrength = 0.5;
    float diffuseStrength = max(0.0, dot(normal, lightDir));
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 128.0);

    vec4 ambient = ambientLightColor * ambientStrength;
    vec4 diffuse = diffuseStrength * sunlightColor;
    vec4 specular = sunlightColor * spec * specularStrength;

    color = (ambient + diffuse + specular) * waterColor;
    if (wireframe == 1) {
        color = waterColor;
    }
}