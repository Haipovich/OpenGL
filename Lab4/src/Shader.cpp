#include "Shader.h"
#include <MyMath/mat4.h> // Для MyMath::mat4::value_ptr()
#include <MyMath/vec3.h> // Для MyMath::vec3

Shader::Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath) {
    // 1. Получение исходного кода шейдеров из filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;

    // Убеждаемся, что объекты ifstream могут выбрасывать исключения:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        // Открытие файлов
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // Чтение содержимого файлов в потоки
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // Закрытие файловых обработчиков
        vShaderFile.close();
        fShaderFile.close();
        // Преобразование потока в строку
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        // Если путь к геометрическому шейдеру присутствует, также загружаем его
        if (geometryPath != nullptr) {
            gShaderFile.open(geometryPath);
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    } catch (std::ifstream::failure& e) {
        std::string errorMsg = "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: Path V: ";
        errorMsg += vertexPath; 
        errorMsg += " F: ";
        errorMsg += fragmentPath;
        if (geometryPath) { errorMsg += " G: "; errorMsg += geometryPath; }
        errorMsg += " What: ";
        errorMsg += e.what();
        throw std::ifstream::failure(errorMsg); // Перебрасываем с дополнительной информацией
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // 2. Компиляция шейдеров
    GLuint vertex, fragment;

    // Вершинный шейдер
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // Фрагментный шейдер
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // Геометрический шейдер (если есть)
    GLuint geometry = 0; // Инициализируем 0
    if (geometryPath != nullptr) {
        const char* gShaderCode = geometryCode.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        checkCompileErrors(geometry, "GEOMETRY");
    }

    // Шейдерная программа
    this->Program = glCreateProgram();
    glAttachShader(this->Program, vertex);
    glAttachShader(this->Program, fragment);
    if (geometryPath != nullptr)
        glAttachShader(this->Program, geometry);
    glLinkProgram(this->Program);
    checkCompileErrors(this->Program, "PROGRAM");

    // Удаляем шейдеры, так как они уже связаны с программой и больше не нужны
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometryPath != nullptr)
        glDeleteShader(geometry);
}

void Shader::Use() {
    glUseProgram(this->Program);
}

void Shader::checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::string errorMsg = "ERROR::SHADER_COMPILATION_ERROR of type: " + type + "\n" + infoLog;
            throw std::runtime_error(errorMsg);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::string errorMsg = "ERROR::PROGRAM_LINKING_ERROR of type: " + type + "\n" + infoLog;
            throw std::runtime_error(errorMsg);
        }
    }
}

// Uniform setters
void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(Program, name.c_str()), (int)value);
}
void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(Program, name.c_str()), value);
}
void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(Program, name.c_str()), value);
}

void Shader::setVec2(const std::string &name, float x, float y) const {
    glUniform2f(glGetUniformLocation(Program, name.c_str()), x, y);
}

void Shader::setVec3(const std::string &name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(Program, name.c_str()), x, y, z);
}

void Shader::setVec3(const std::string &name, const MyMath::vec3& value) const {
    glUniform3fv(glGetUniformLocation(Program, name.c_str()), 1, &value.x); // &value.x - предполагая, что vec3 хранит x,y,z последовательно
}

void Shader::setVec4(const std::string &name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(Program, name.c_str()), x, y, z, w);
}

void Shader::setMat4(const std::string &name, const MyMath::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(Program, name.c_str()), 1, GL_FALSE, mat.value_ptr());
} 
