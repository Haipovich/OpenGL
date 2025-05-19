#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

#include <MyMath/vec3.h>
#include <MyMath/mat4.h>

class Shader {
public:
    GLuint Program;
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath = nullptr);
    void Use();
    
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setVec2(const std::string &name, float x, float y) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
    void setVec3(const std::string &name, const MyMath::vec3& value) const;
    void setVec4(const std::string &name, float x, float y, float z, float w) const;
    void setMat4(const std::string &name, const MyMath::mat4& mat) const;

private:
    void checkCompileErrors(GLuint shader, std::string type);
};

#endif 
