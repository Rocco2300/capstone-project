layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoord;

uniform int size;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

uniform sampler2D normal;
uniform sampler2D displacement;

out vec4 outColor;

void main() {
    vec2 coords = position.xz / (size * 0.25);
    vec4 height = texture(displacement, coords);
    vec4 finalPos = position;
    finalPos.y = height.x;
    finalPos = model * finalPos;

    vec3 normalVec = texture(normal, coords).xyz;
    normalVec = normalize(normalVec * 2.0 - 1.0);
    vec3 lightPos = vec3(128.0, 128.0, 0.0);
    vec3 lightDir = normalize(finalPos.xyz - lightPos);
    //vec3 lightDir = normalize(vec3(-1.0, -1.0, 0.0));
    float shadow  = max(0.0, dot(-lightDir, normalVec));

    vec4 waterColor = vec4(0.1, 0.3, 0.7, 1.0);
    outColor = shadow * waterColor;

    gl_Position = projection * view * finalPos;
}