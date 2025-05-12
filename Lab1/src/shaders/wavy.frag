#version 330 core
in vec2 TexCoords;

out vec4 FragColor;

void main() {
    float frequency = 4.0;
    float amplitude = 0.1;
    float stripe_density = 15.0;

    float y_wave = TexCoords.y + amplitude * sin(TexCoords.x * frequency * 6.2831853);

    float pattern = fract(y_wave * stripe_density);

    float color = step(0.5, pattern);

    FragColor = vec4(vec3(color), 1.0);
}
