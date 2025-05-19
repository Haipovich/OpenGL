#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "MyMath/MyMath.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

const unsigned int GRID_SIZE = 32;
const float GRID_STEP = 0.1f;
const float AMPLITUDE = 1.0f;

const MyMath::vec3 CAMERA_POS(0.0f, 0.0f, 3.0f);
const MyMath::vec3 LIGHT_POS(1.0f, -10.0f, 1.0f);
const MyMath::vec3 LIGHT_COLOR(1.0f, 1.0f, 1.0f);
const MyMath::vec3 OBJECT_COLOR(0.1f, 0.5f, 0.8f);

float g_scale = 1.0f;
MyMath::vec3 g_translation(0.0f, 0.0f, 0.0f);
float g_rotationX = 0.0f;
float g_rotationY = 0.0f;
float g_blendFactor = 0.5f;

std::string loadShaderFromFile(const std::string& filePath) {
    std::ifstream shaderFile;
    shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        shaderFile.open(filePath);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        return shaderStream.str();
    } catch (std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        std::cerr << "File path: " << filePath << std::endl;
        return "";
    }
}

GLuint loadTexture(const char* path) {
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        if (glewIsSupported("GL_EXT_texture_filter_anisotropic")) {
            GLfloat maxAnisotropy;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
        }
        stbi_image_free(data);
    } else {
        std::cerr << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
        return 0;
    }
    return textureID;
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        g_scale += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        g_scale = std::max(0.01f, g_scale - 0.01f);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        g_rotationY -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        g_rotationY += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        g_rotationX -= 1.0f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        g_rotationX += 1.0f;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        g_blendFactor = std::max(0.0f, g_blendFactor - 0.01f);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        g_blendFactor = std::min(1.0f, g_blendFactor + 0.01f);

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Lab 3: Textures", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    stbi_set_flip_vertically_on_load(true);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    std::string vertexShaderCode = loadShaderFromFile("/home/Haipovich/dev/Projects/study/OpenGL/Lab3/src/shaders/shader.vert");
    std::string fragmentShaderCode = loadShaderFromFile("/home/Haipovich/dev/Projects/study/OpenGL/Lab3/src/shaders/shader.frag");

    const char* vertexShaderSource = vertexShaderCode.c_str();
    const char* fragmentShaderSource = fragmentShaderCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    std::vector<float> vertices;
    vertices.reserve(GRID_SIZE * GRID_SIZE * 4);
    std::vector<unsigned int> indices;
    indices.reserve((GRID_SIZE - 1) * (GRID_SIZE - 1) * 6);

    for (unsigned int i = 0; i < GRID_SIZE; ++i) {
        for (unsigned int j = 0; j < GRID_SIZE; ++j) {
            float x = (static_cast<float>(i) - static_cast<float>(GRID_SIZE - 1) / 2.0f) * GRID_STEP;
            float y = (static_cast<float>(j) - static_cast<float>(GRID_SIZE - 1) / 2.0f) * GRID_STEP;
            vertices.push_back(x);
            vertices.push_back(y);

            float u = static_cast<float>(i) / (GRID_SIZE - 1);
            float v = static_cast<float>(j) / (GRID_SIZE - 1);
            vertices.push_back(u);
            vertices.push_back(v);
        }
    }

    for (unsigned int i = 0; i < GRID_SIZE - 1; ++i) {
        for (unsigned int j = 0; j < GRID_SIZE - 1; ++j) {
            unsigned int topLeft = i * GRID_SIZE + j;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (i + 1) * GRID_SIZE + j;
            unsigned int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);    indices.push_back(bottomLeft); indices.push_back(topRight);
            indices.push_back(topRight);   indices.push_back(bottomLeft); indices.push_back(bottomRight);
        }
    }

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    GLuint texture1 = loadTexture("/home/Haipovich/dev/Projects/study/OpenGL/Lab3/src/textures/texture1.jpg");
    GLuint texture2 = loadTexture("/home/Haipovich/dev/Projects/study/OpenGL/Lab3/src/textures/texture2.jpg");


    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        MyMath::mat4 model = MyMath::mat4::identity();
        model = MyMath::translate(model, g_translation);
        model = MyMath::rotate(model, MyMath::radians(g_rotationY), MyMath::vec3(0.0f, 1.0f, 0.0f));
        model = MyMath::rotate(model, MyMath::radians(g_rotationX), MyMath::vec3(1.0f, 0.0f, 0.0f));
        model = MyMath::scale(model, MyMath::vec3(g_scale));

        MyMath::mat4 view = MyMath::mat4::lookAt(
                CAMERA_POS,                     
                MyMath::vec3(0.0f, 0.0f, 0.0f),
                MyMath::vec3(0.0f, 1.0f, 0.0f) 
        );

        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float aspectRatio = (height > 0) ? static_cast<float>(width) / static_cast<float>(height) : 1.0f;
        MyMath::mat4 projection = MyMath::mat4::perspective(
                MyMath::radians(45.0f),
                aspectRatio,
                0.1f,
                100.0f
        );

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, model.value_ptr());
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, view.value_ptr());
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, projection.value_ptr());
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, &LIGHT_POS.x);
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightColor"), 1, &LIGHT_COLOR.x);
        glUniform3fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, &OBJECT_COLOR.x);
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, &CAMERA_POS.x);
        glUniform1f(glGetUniformLocation(shaderProgram, "amplitude"), AMPLITUDE);
        glUniform1f(glGetUniformLocation(shaderProgram, "blendFactor"), g_blendFactor);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture2"), 1);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);
    glDeleteTextures(1, &texture1);
    glDeleteTextures(1, &texture2);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
