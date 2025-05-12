#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "MyMath/MyMath.h"

const unsigned int GRID_SIZE = 32;
const float GRID_STEP = 0.1f;
const float AMPLITUDE = 1.0f;

const MyMath::vec3 CAMERA_POS(0.0f, 0.0f, 3.0f);
const MyMath::vec3 LIGHT_POS(1.0f, 1.0f, 1.0f);
const MyMath::vec3 LIGHT_COLOR(1.0f, 1.0f, 1.0f);
const MyMath::vec3 OBJECT_COLOR(0.1f, 0.5f, 0.8f);

float g_scale = 1.0f;
MyMath::vec3 g_translation(0.0f, 0.0f, 0.0f);
float g_rotationX = 0.0f;
float g_rotationY = 0.0f;

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

    GLFWwindow* window = glfwCreateWindow(800, 600, "Lab 2: Surface (GPU Calculation)", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

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
    vertices.reserve(GRID_SIZE * GRID_SIZE * 2);
    std::vector<unsigned int> indices;
    indices.reserve((GRID_SIZE - 1) * (GRID_SIZE - 1) * 6);

    for (unsigned int i = 0; i < GRID_SIZE; ++i) {
        for (unsigned int j = 0; j < GRID_SIZE; ++j) {
            float x = (static_cast<float>(i) - static_cast<float>(GRID_SIZE - 1) / 2.0f) * GRID_STEP;
            float y = (static_cast<float>(j) - static_cast<float>(GRID_SIZE - 1) / 2.0f) * GRID_STEP;
            vertices.push_back(x);
            vertices.push_back(y);
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
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);


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

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
