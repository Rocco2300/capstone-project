#include <Globals.hpp>

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoord;

layout(rgba32f, binding = DISPLACEMENT_UNIT) uniform image2D displacement;
layout(rgba32f, binding = NORMAL_UNIT) uniform image2D normal;

uniform mat4 view;
uniform mat4 model;

uniform double spacing;

out VS_OUT {
    vec3 normal;
} vsOut;

void main() {
    vec2 coords = position.xz * (1.0 / float(spacing));
    vec3 displacement = imageLoad(displacement, ivec2(coords)).rgb;
    vec4 finalPos = position;
    finalPos.xz -= displacement.xz;
    finalPos.y += displacement.y;

    gl_Position = view * model * finalPos;

    vec3 normalVec = imageLoad(normal, ivec2(coords)).xyz;
    mat3 normalMat = mat3(transpose(inverse(view * model)));
    vsOut.normal = normalize(normalMat * normalVec);
}
