#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <string>
#include <memory> // Для std::unique_ptr

#include "Shader.h"
#include "Camera.h"
#include "PointSet.h"
#include "Curve.h"
#include "RevolutionSurface.h"
#include <MyMath/MyMath.h> // Для mat4, vec3 и т.д. (включает vec3.h, mat4.h)

// Прототипы функций обратного вызова
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// Вспомогательная функция для обработки ввода с клавиатуры (удержание клавиш)
void processInput(GLFWwindow *window);
// Прототип для screenToWorldCoordinates, чтобы он был виден до его использования
MyMath::vec3 screenToWorldCoordinates(double xpos, double ypos, int screenWidth, int screenHeight);

// Настройки окна
unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 720;

// Камера
Camera camera(MyMath::vec3(0.0f, 0.5f, 3.0f)); // Начальная позиция камеры
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// Время
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Режимы работы
enum class AppMode {
    INPUT_POINTS,
    VIEW_SURFACE
};
AppMode currentMode = AppMode::INPUT_POINTS;
bool modeChanged = false;

// Объекты сцены (используем unique_ptr для управления ресурсами)
std::unique_ptr<PointSet> pointSet;
std::unique_ptr<Curve> curve;
std::unique_ptr<RevolutionSurface> revolutionSurface;

// Шейдеры
std::unique_ptr<Shader> pointShader;
std::unique_ptr<Shader> curveShader;
std::unique_ptr<Shader> surfaceShader;

// Параметры для тела вращения
const int SURFACE_SEGMENTS = 32; // Количество сегментов при вращении
const char ROTATION_AXIS = 'Y';  // ИЗМЕНЕНО НА Y
float surfaceRotationAngleX = 0.0f;
float surfaceRotationAngleY = 0.0f;
const float ROTATION_SPEED = 50.0f; // Скорость вращения в градусах в секунду

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
    if (currentMode == AppMode::VIEW_SURFACE) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);

        // Управление вращением объекта
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
            surfaceRotationAngleY -= ROTATION_SPEED * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
            surfaceRotationAngleY += ROTATION_SPEED * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
            surfaceRotationAngleX -= ROTATION_SPEED * deltaTime;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
            surfaceRotationAngleX += ROTATION_SPEED * deltaTime;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    if (currentMode == AppMode::VIEW_SURFACE) {
        float xpos = static_cast<float>(xposIn);
        float ypos = static_cast<float>(yposIn);

        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;

        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    if (currentMode == AppMode::VIEW_SURFACE) {
        camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (currentMode == AppMode::INPUT_POINTS && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        MyMath::vec3 worldPos = screenToWorldCoordinates(xpos, ypos, SCR_WIDTH, SCR_HEIGHT);
        
        pointSet->addPoint(worldPos.x, worldPos.y); // z=0 предполагается в screenToWorld или addPoint
        pointSet->updateBuffers(); // Обновляем буферы точек

        if (pointSet->getNumPoints() >= 2) {
            // Генерируем кривую (например, ломаную или Безье)
            // Для Безье нужно будет больше логики в Curve::generateCurve
            // Пока что пусть Curve::generateCurve просто соединяет точки из pointSet
            curve->generateCurve(pointSet->getPoints(), 10); // 10 сегментов между контрольными точками (если Безье)
                                                           // Для ломаной segmentsPerControlPoint не так важен
            curve->updateBuffers(); // Обновляем буферы кривой
        }
    }
}

MyMath::vec3 screenToWorldCoordinates(double xpos, double ypos, int screenWidth, int screenHeight) {
    // Преобразование экранных координат (пиксели, Y сверху вниз) в NDC (от -1 до 1, Y снизу вверх)
    float ndcX = (static_cast<float>(xpos) / screenWidth) * 2.0f - 1.0f;
    float ndcY = 1.0f - (static_cast<float>(ypos) / screenHeight) * 2.0f;

    // Для режима ввода точек, мы хотим, чтобы эти NDC координаты отображались
    // в какой-то фиксированной плоскости XY.
    // Предположим, что наша "сцена" для рисования кривой имеет размеры,
    // соотносимые с NDC, например, X от -aspectRatio до +aspectRatio, Y от -1 до 1.
    float aspectRatio = static_cast<float>(screenWidth) / screenHeight;
    
    // Простейший вариант: точки кривой лежат в плоскости z=0,
    // а их x, y координаты напрямую соответствуют NDC, масштабированным по aspectRatio.
    // Это значит, что при ортографической проекции (-ar, ar, -1, 1, -1, 1)
    // клик в углу даст точку в углу этой области.
    return MyMath::vec3(ndcX * aspectRatio, ndcY, 0.0f);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, true);
        if (key == GLFW_KEY_P) { // Переключение режима
            if (currentMode == AppMode::INPUT_POINTS) {
                if (pointSet->getNumPoints() >= 2) {
                    currentMode = AppMode::VIEW_SURFACE;
                    modeChanged = true; // Флаг для первоначальной генерации поверхности
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Включаем захват мыши
                    firstMouse = true; // Сброс для камеры
                    std::cout << "Switched to VIEW_SURFACE mode." << std::endl;

                    // Очищаем точки и кривую предыдущего режима, если они больше не нужны
                    // pointSet->clearPoints(); // Опционально, если не хотим их видеть под поверхностью
                    // curve->clearCurve();   // Опционально
                } else {
                    std::cout << "Add at least 2 points to generate a surface." << std::endl;
                }
            }
            // Обратный переход не обязателен, но можно добавить по желанию
            // else {
            //    currentMode = AppMode::INPUT_POINTS;
            //    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // Освобождаем мышь
            //    std::cout << "Switched to INPUT_POINTS mode." << std::endl;
            // }
        }
        if (key == GLFW_KEY_C && currentMode == AppMode::INPUT_POINTS) { // Очистка точек
            pointSet->clearPoints();
            pointSet->updateBuffers();
            curve->clearCurve();
            curve->updateBuffers();
            std::cout << "Cleared all points." << std::endl;
        }
    }
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL Lab 4 - Revolution Surface", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);

    try {
        pointShader = std::make_unique<Shader>("../shaders/point.vert", "../shaders/point.frag");
        curveShader = std::make_unique<Shader>("../shaders/curve.vert", "../shaders/curve.frag");
        surfaceShader = std::make_unique<Shader>("../shaders/surface.vert", "../shaders/surface.frag");
    } catch (const std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        glfwTerminate();
        return -1;
    } catch (const std::runtime_error& e) {
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED: " << e.what() << std::endl;
        glfwTerminate();
        return -1;
    }

    pointSet = std::make_unique<PointSet>();
    curve = std::make_unique<Curve>();
    revolutionSurface = std::make_unique<RevolutionSurface>();

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (currentMode == AppMode::INPUT_POINTS) {
            float aspectRatio = (float)SCR_WIDTH / (float)SCR_HEIGHT;
            MyMath::mat4 ortho_projection = MyMath::mat4::orthographic(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
            MyMath::mat4 identity_view = MyMath::mat4::identity();
            MyMath::mat4 model = MyMath::mat4::identity();

            pointShader->Use();
            pointShader->setMat4("projection", ortho_projection);
            pointShader->setMat4("view", identity_view);
            pointShader->setMat4("model", model);
            pointShader->setVec3("pointColor", 1.0f, 1.0f, 0.0f); 
            pointSet->Draw(*pointShader);

            if (pointSet->getNumPoints() >= 2) {
                curveShader->Use();
                curveShader->setMat4("projection", ortho_projection);
                curveShader->setMat4("view", identity_view);
                curveShader->setMat4("model", model);
                curveShader->setVec3("curveColor", 0.0f, 1.0f, 0.0f); 
                curve->Draw(*curveShader);
            }
        } else {
            // Матрицы проекции и вида для 3D создаются здесь
            MyMath::mat4 projection = MyMath::mat4::perspective(MyMath::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
            MyMath::mat4 view = camera.GetViewMatrix();

            if (modeChanged) {
                if (pointSet->getNumPoints() >= 2) {
                    if (!curve->curvePoints.empty()){
                         revolutionSurface->generateSurface(curve->curvePoints, SURFACE_SEGMENTS, ROTATION_AXIS);
                    } else if (!pointSet->getPoints().empty()) {
                        revolutionSurface->generateSurface(pointSet->getPoints(), SURFACE_SEGMENTS, ROTATION_AXIS);
                    }
                    
                    if(!revolutionSurface->vertices.empty()){
                        revolutionSurface->setupBuffers();
                    }
                }
                modeChanged = false;
            }

            if (!revolutionSurface->vertices.empty()) {
                surfaceShader->Use();
                surfaceShader->setMat4("projection", projection);
                surfaceShader->setMat4("view", view);

                MyMath::mat4 surfaceModel = MyMath::mat4::identity();
                // Применяем вращение сначала вокруг X, потом вокруг Y
                surfaceModel = MyMath::rotate(surfaceModel, MyMath::radians(surfaceRotationAngleX), MyMath::vec3(1.0f, 0.0f, 0.0f));
                surfaceModel = MyMath::rotate(surfaceModel, MyMath::radians(surfaceRotationAngleY), MyMath::vec3(0.0f, 1.0f, 0.0f));

                surfaceShader->setMat4("model", surfaceModel);
                
                surfaceShader->setVec3("lightPos", 1.0f, 2.0f, 2.0f);
                surfaceShader->setVec3("lightColor", 1.0f, 1.0f, 1.0f);
                surfaceShader->setVec3("objectColor", 0.5f, 0.7f, 0.8f);
                surfaceShader->setVec3("viewPos", camera.Position);

                revolutionSurface->Draw(*surfaceShader, surfaceModel);
            }
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
