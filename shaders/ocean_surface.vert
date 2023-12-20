#version 460

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoord;

uniform mat4 mvp;
uniform sampler2D tex;

void main() {
    vec2 coords = position.xz / (512.0 * 0.25);
    vec4 height = texture(tex, coords);
    vec4 finalPos = position;
    finalPos.y = height.x;
    gl_Position = mvp * finalPos;
}