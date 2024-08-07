#include <Globals.hpp>

layout (location = 0) in vec4 position;
layout (location = 1) in vec2 texCoord;

layout(binding = DISPLACEMENT_UNIT) uniform sampler2D displacement;
layout(binding = NORMAL_UNIT) uniform sampler2D normal;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;
uniform vec4 cameraPosition;

uniform double size;
uniform double spacing;

out vec3 viewPosition;
out vec3 surfaceNormal;
out vec3 fragmentPosition;

void main() {
    //vec2 coords = position.xz * (1.0 / float(spacing));
    //vec3 displacement = imageLoad(displacement, ivec2(coords)).rgb;
    vec2 coords = (position.xz * (1.0 / float(spacing))) / float(size);
    vec3 displacement = texture(displacement, coords).rgb;
    vec4 finalPos = position;
    finalPos.xz -= displacement.xz;
    finalPos.y += displacement.y;

    gl_Position = projection * view * model * finalPos;

    viewPosition = cameraPosition.xyz;

    vec3 normalVec = texture(normal, coords).xyz;
    mat3 normalMat = mat3(transpose(inverse(model)));

    surfaceNormal = normalize(normalMat * normalVec);
    fragmentPosition = vec3(model * finalPos);
}