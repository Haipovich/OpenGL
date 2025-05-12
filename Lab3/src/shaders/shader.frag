#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 viewPos;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float blendFactor;

void main() {
    vec3 norm = normalize(Normal);

    vec3 albedo = mix(texture(texture1, TexCoords).rgb, texture(texture2, TexCoords).rgb, blendFactor);

    float ambientStrength = 0.15;
    vec3 ambientLight = ambientStrength * lightColor;
    vec3 ambientMaterial = objectColor;
    vec3 ambient = ambientLight * albedo;


    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuseLight = diff * lightColor;
    vec3 diffuseMaterial = mix(texture(texture1, TexCoords).rgb, texture(texture2, TexCoords).rgb, blendFactor); // Blended texture color for diffuse
    vec3 diffuse = diffuseLight * albedo;


    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specularLight = specularStrength * spec * lightColor;
    vec3 specularMaterial = vec3(0.8, 0.8, 0.8);
    vec3 specular = specularLight * specularMaterial;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
