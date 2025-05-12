#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "MyMath/MyMath.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

GLuint wavyShaderProgram;
GLuint circularShaderProgram;
GLuint currentShaderProgram;

const MyMath::vec3 CAMERA_POS(0.0f, 0.0f, 1.5f);

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

GLuint compileShader(GLenum type, const char* source, const std::string& shaderName = "") {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED" << (!shaderName.empty() ? " (" + shaderName + ")" : "") << "\n" << (type == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT") << "\n" << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }
    std::cout << "Shader compiled successfully" << (!shaderName.empty() ? ": " + shaderName : "") << std::endl;
    return shader;
}

GLuint linkShaderProgram(GLuint vertexShader, GLuint fragmentShader) {
     GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        glDeleteProgram(shaderProgram);
        return 0;
    }
     std::cout << "Shader program linked successfully." << std::endl;
    return shaderProgram;
}


void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
         static int space_last_state = GLFW_RELEASE;
         int space_current_state = glfwGetKey(window, GLFW_KEY_SPACE);
         if (space_current_state == GLFW_PRESS && space_last_state == GLFW_RELEASE) {
            if (currentShaderProgram == wavyShaderProgram) {
                currentShaderProgram = circularShaderProgram;
                std::cout << "Switched to Circular Pattern" << std::endl;
            } else {
                currentShaderProgram = wavyShaderProgram;
                std::cout << "Switched to Wavy Pattern" << std::endl;
            }
         }
         space_last_state = space_current_state;
    }

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        g_scale += 0.01f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        g_scale = std::max(0.1f, g_scale - 0.01f);
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

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lab 1: Wavy and Circular Rectangles", nullptr, nullptr);
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

    std::string vertexShaderPath = "/home/Haipovich/dev/Projects/study/OpenGL/Lab1/src/shaders/shader.vert";
    std::string wavyFragmentShaderPath = "/home/Haipovich/dev/Projects/study/OpenGL/Lab1/src/shaders/wavy.frag";
    std::string circularFragmentShaderPath = "/home/Haipovich/dev/Projects/study/OpenGL/Lab1/src/shaders/circular.frag";

    std::string vertexShaderCode = loadShaderFromFile(vertexShaderPath);
    std::string wavyFragmentShaderCode = loadShaderFromFile(wavyFragmentShaderPath);
    std::string circularFragmentShaderCode = loadShaderFromFile(circularFragmentShaderPath);

    if (vertexShaderCode.empty() || wavyFragmentShaderCode.empty() || circularFragmentShaderCode.empty()) {
        std::cerr << "Failed to load one or more shaders." << std::endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    const char* vertexShaderSource = vertexShaderCode.c_str();
    const char* wavyFragmentShaderSource = wavyFragmentShaderCode.c_str();
    const char* circularFragmentShaderSource = circularFragmentShaderCode.c_str();

    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource, vertexShaderPath);
    if (vertexShader == 0) { glfwDestroyWindow(window); glfwTerminate(); return -1; }

    GLuint wavyFragmentShader = compileShader(GL_FRAGMENT_SHADER, wavyFragmentShaderSource, wavyFragmentShaderPath);
     if (wavyFragmentShader == 0) { glDeleteShader(vertexShader); glfwDestroyWindow(window); glfwTerminate(); return -1; }

    GLuint circularFragmentShader = compileShader(GL_FRAGMENT_SHADER, circularFragmentShaderSource, circularFragmentShaderPath);
     if (circularFragmentShader == 0) { glDeleteShader(vertexShader); glDeleteShader(wavyFragmentShader); glfwDestroyWindow(window); glfwTerminate(); return -1; }

    wavyShaderProgram = linkShaderProgram(vertexShader, wavyFragmentShader);
    if (wavyShaderProgram == 0) {
        glDeleteShader(vertexShader); glDeleteShader(wavyFragmentShader); glDeleteShader(circularFragmentShader);
        glfwDestroyWindow(window); glfwTerminate(); return -1;
    }

    circularShaderProgram = linkShaderProgram(vertexShader, circularFragmentShader);
     if (circularShaderProgram == 0) {
        glDeleteShader(vertexShader); glDeleteShader(wavyFragmentShader); glDeleteShader(circularFragmentShader);
        glDeleteProgram(wavyShaderProgram);
        glfwDestroyWindow(window); glfwTerminate(); return -1;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(wavyFragmentShader);
    glDeleteShader(circularFragmentShader);

    currentShaderProgram = circularShaderProgram;
    std::cout << "Starting with Wavy Pattern. Press SPACE to switch." << std::endl;

     float vertices[] = {
        -1.0f,  1.0f,     0.0f, 1.0f,
        -1.0f, -1.0f,     0.0f, 0.0f,
         1.0f, -1.0f,     1.0f, 0.0f,
         1.0f,  1.0f,     1.0f, 1.0f
    };
    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3 
    };


    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(currentShaderProgram);

        MyMath::mat4 model = MyMath::mat4::identity();
        model = MyMath::translate(model, g_translation);
        model = MyMath::rotate(model, MyMath::radians(g_rotationY), MyMath::vec3(0.0f, 1.0f, 0.0f));
        model = MyMath::rotate(model, MyMath::radians(g_rotationX), MyMath::vec3(1.0f, 0.0f, 0.0f));
        model = MyMath::scale(model, MyMath::vec3(g_scale, g_scale, g_scale));

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

        glUniformMatrix4fv(glGetUniformLocation(currentShaderProgram, "model"), 1, GL_FALSE, model.value_ptr());
        glUniformMatrix4fv(glGetUniformLocation(currentShaderProgram, "view"), 1, GL_FALSE, view.value_ptr());
        glUniformMatrix4fv(glGetUniformLocation(currentShaderProgram, "projection"), 1, GL_FALSE, projection.value_ptr());

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(wavyShaderProgram);
    glDeleteProgram(circularShaderProgram);


    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
