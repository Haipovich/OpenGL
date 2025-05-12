#version 330 core
layout (location = 0) in vec2 aGridPos;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float amplitude;

float surfaceFunction(float x, float y) {
    return amplitude * sin(x * 2.0) * cos(y * 2.0);
}

vec3 calculateNormal(float x, float y) {
    float eps = 0.001;
    float z_x = (surfaceFunction(x + eps, y) - surfaceFunction(x - eps, y)) / (2.0 * eps);
    float z_y = (surfaceFunction(x, y + eps) - surfaceFunction(x, y - eps)) / (2.0 * eps);

    vec3 normal = vec3(-z_x, -z_y, 1.0);
    return normalize(normal);
}

void main() {
    float z = surfaceFunction(aGridPos.x, aGridPos.y);
    vec3 localPos = vec3(aGridPos.x, aGridPos.y, z);

    vec3 localNormal = calculateNormal(aGridPos.x, aGridPos.y);

    FragPos = vec3(model * vec4(localPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * localNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
