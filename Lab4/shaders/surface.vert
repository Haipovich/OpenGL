#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
// layout (location = 2) in vec2 aTexCoords; // Для текстур, если понадобятся

out vec3 FragPos;
out vec3 Normal;
// out vec2 TexCoords; // Для текстур

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

// Матрица для преобразования нормалей (обычно transpose(inverse(modelView)))
// Но если model не содержит неоднородного масштабирования, можно использовать mat3(model)
// Безопаснее всего использовать transpose(inverse(model))
uniform mat3 normalMatrix; // Опционально, можно передавать заранее вычисленную

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0));
    
    // Используем переданную normalMatrix или вычисляем на лету
    // Normal = normalMatrix * aNormal; 
    Normal = mat3(transpose(inverse(model))) * aNormal; 
    
    // if (aTexCoords is available)
    // TexCoords = aTexCoords;
}
