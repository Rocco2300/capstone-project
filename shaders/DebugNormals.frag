layout (location = 0) out vec4 color;

in vec4 viewPosition;
in vec4 surfaceNormal;
in vec4 fragmentPosition;

void main() {
    color = vec4(0.7, 0.2, 0.1, 1.0);
}
