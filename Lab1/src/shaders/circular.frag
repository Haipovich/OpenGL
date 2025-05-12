#version 330 core
in vec2 TexCoords;

out vec4 FragColor;

void main() {
    vec2 center = vec2(0.5, 0.5);
    float ring_density = 30.0;
    float speed = 2.0;

    float dist = distance(TexCoords, center);

    float pattern = fract(dist * ring_density);

    float color = step(0.5, pattern);

    FragColor = vec4(vec3(color), 1.0);
}
