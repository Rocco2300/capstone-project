#include <Globals.hpp>

layout (triangles) in;
layout (line_strip, max_vertices =  6) out;

uniform mat4 projection;

in VS_OUT {
    vec3 normal;
} gsIn[];

void generateLine(int index) {
    gl_Position = projection * gl_in[index].gl_Position;
    EmitVertex();

    gl_Position = projection * (gl_in[index].gl_Position + vec4(gsIn[index].normal, 0.0) * 0.4);
    EmitVertex();
    EndPrimitive();
}

void main() {
    generateLine(0);
    generateLine(1);
    generateLine(2);
}
