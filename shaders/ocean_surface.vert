#version 460

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoord;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

uniform sampler2D normal;
uniform sampler2D displacement;

out vec3 fragPos;
out vec3 normalVec;

void main() {
    vec2 coords = position.xz / 256.0;
    vec4 height = texture(displacement, coords);
    vec4 finalPos = position;
    finalPos.y = height.x;

    normalVec = texture(normal, coords).xyz;
    normalVec = normalize(normalVec * 2.0 - 1.0);
    fragPos = (model * vec4(finalPos)).xyz;
    gl_Position = projection * view * model * finalPos;
}