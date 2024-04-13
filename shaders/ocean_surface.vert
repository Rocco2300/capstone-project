#include <Globals.hpp>

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoord;

layout(rgba32f, binding = BUFFERS_UNIT) uniform image2DArray buffers;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

out vec4 outNormal;

void main() {
    vec2 coords = position.xz * (1.0 / 0.25);
    vec4 height = imageLoad(buffers, ivec3(coords, DISPLACEMENT_INDEX));
    vec4 finalPos = position;
    finalPos.y = height.x;
    finalPos = model * finalPos;

    gl_Position = projection * view * finalPos;
    outNormal = imageLoad(buffers, ivec3(coords, NORMAL_INDEX));
}