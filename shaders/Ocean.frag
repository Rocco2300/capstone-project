layout (location = 0) out vec4 color;

in vec4 viewPosition;
in vec4 surfaceNormal;
in vec4 fragmentPosition;

float easeOutCubic(float x) {
    return 1.0 - pow(1.0 - x, 3);
    //return x == 1.0 ? 1.0 : 1.0 - pow(2.0, -10. * x);
}

float easeInCubic(float x) {
    //return x * x * x;
    //return 1.0 - cos((x * 3.1415) / 2.0);
    //if (x < 0.5) {
    //    return x - 0.025;
    //} else {
    //    return x;
    //}
    return x;
}

void main() {
    vec3 normal = vec3(surfaceNormal);
    vec3 viewPos = vec3(viewPosition);
    vec3 fragPos = vec3(fragmentPosition);

    float azimuth = radians(0.0);
    float inclination = radians(90.0);
    float lightX = sin(inclination) * cos(azimuth);
    float lightY = sin(inclination) * cos(azimuth);
    float lightZ = cos(inclination);
    vec3 lightDir = normalize(vec3(lightX, lightY, lightZ));
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    vec4 waterColor = vec4(0.1, 0.3, 0.7, 1.0);
    vec4 sunlightColor = vec4(1.0, 0.99, 0.87, 1.0);
    vec4 ambientLightColor = vec4(1.0, 0.34, 0.2, 1.0);

    float ambientStrength = 0.2;
    float specularStrength = 0.5;
    float diffuseStrength = max(0.0, dot(normal, lightDir));
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 128.0);

    vec4 ambient = ambientLightColor * ambientStrength;
    vec4 diffuse = diffuseStrength * sunlightColor;
    vec4 specular = sunlightColor * spec * specularStrength;

    color = (ambient + diffuse + specular) * waterColor;
}