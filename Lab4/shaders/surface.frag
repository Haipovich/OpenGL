#version 330 core
// Корректный фрагментный шейдер v1.0.2
in vec3 FragPos;       // Позиция фрагмента в мировых координатах (из вершинного шейдера)
in vec3 Normal;        // Нормаль фрагмента в мировых координатах (из вершинного шейдера)
// in vec2 TexCoords; // Текстурные координаты, если используются

out vec4 FragColor;   // Выходной цвет фрагмента

// Параметры освещения и материала
uniform vec3 lightPos;     // Позиция источника света (в мировых координатах)
uniform vec3 lightColor;   // Цвет источника света
uniform vec3 objectColor;  // Базовый цвет объекта
uniform vec3 viewPos;      // Позиция наблюдателя/камеры (в мировых координатах)

// uniform sampler2D objectTexture; // Для текстурирования, если будет

void main()
{
    // Нормализация нормали (на всякий случай, интерполяция может немного исказить длину)
    vec3 norm = normalize(Normal);

    // Ambient (фоновое освещение)
    float ambientStrength = 0.15; // Интенсивность фонового света
    vec3 ambient = ambientStrength * lightColor * objectColor;

    // Diffuse (рассеянное освещение)
    vec3 lightDir = normalize(lightPos - FragPos); // Направление от фрагмента к источнику света
    float diff = max(dot(norm, lightDir), 0.0);    // Косинус угла между нормалью и направлением света
    vec3 diffuse = diff * lightColor * objectColor;

    // Specular (бликовое освещение) - модель Блинна-Фонга
    float specularStrength = 0.6; // Интенсивность блика
    vec3 viewDir = normalize(viewPos - FragPos); // Направление от фрагмента к наблюдателю
    vec3 halfwayDir = normalize(lightDir + viewDir); // Промежуточный вектор (для Блинна-Фонга)
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0); // 32.0 - "shininess" (блеск)
    vec3 specular = specularStrength * spec * lightColor; // Блики обычно имеют цвет источника света

    /* // Альтернатива: Стандартная модель Фонга для бликов
    vec3 reflectDir = reflect(-lightDir, norm); // Отраженное направление света
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * lightColor;
    */

    // Итоговый цвет = фоновое + рассеянное + бликовое
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
