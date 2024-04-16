#include <Globals.hpp>

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoord;

layout(rgba32f, binding = DISPLACEMENT_UNIT) uniform image2D displacement;
layout(rgba32f, binding = NORMAL_UNIT) uniform image2D normal;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;

out vec4 outNormal;

void main() {
    vec2 coords = position.xz * (1.0 / 0.25);
    vec3 displacement = imageLoad(displacement, ivec2(coords)).rgb;
    vec4 finalPos = position;
    finalPos.xz -= displacement.xz;
    finalPos.y += displacement.y;

    gl_Position = projection * view * model * finalPos;
    outNormal = imageLoad(normal, ivec2(coords));
}