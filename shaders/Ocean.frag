layout (location = 0) out vec4 color;

in vec4 viewPosition;
in vec4 surfaceNormal;
in vec4 fragmentPosition;

void main() {
    vec3 normal = vec3(surfaceNormal);
    vec3 viewPos = vec3(viewPosition);
    vec3 fragPos = vec3(fragmentPosition);

    float azimuth = radians(15.0);
    float inclination = radians(35.0);
    float lightX = sin(inclination) * cos(azimuth);
    float lightY = sin(inclination) * cos(azimuth);
    float lightZ = cos(inclination);
    vec3 lightDir = normalize(vec3(lightX, lightY, lightZ));
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    vec4 waterColor = vec4(0.1, 0.3, 0.7, 1.0);
    vec4 sunlightColor = vec4(1.0, 0.99, 0.87, 1.0);
    vec4 ambientLightColor = vec4(1.0, 0.34, 0.2, 1.0);

    float ambientStrength = 0.1;
    float diffuseStrength = max(0.0, dot(normal, lightDir));
    float specularStrength = pow(max(dot(normal, halfwayDir), 0.0), 128.0);

    vec4 ambient = ambientLightColor * ambientStrength;
    vec4 diffuse = diffuseStrength * sunlightColor;
    vec4 specular = sunlightColor * specularStrength;

    color = (ambient + diffuse) * waterColor;
}