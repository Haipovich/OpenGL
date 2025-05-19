#ifndef REVOLUTION_SURFACE_H
#define REVOLUTION_SURFACE_H

#include <vector>
#include <MyMath/vec3.h>
#include <MyMath/mat4.h>
#include "Shader.h"

struct Vertex {
    MyMath::vec3 Position;
    MyMath::vec3 Normal;
    // MyMath::vec2 TexCoords; // Если нужны текстуры
};

class RevolutionSurface
{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;

    RevolutionSurface();
    ~RevolutionSurface();

    // curvePoints - точки исходной кривой (2D, предполагаем XY плоскость, вращение вокруг оси X)
    // numSegments - количество сегментов при вращении (насколько гладким будет тело)
    void generateSurface(const std::vector<MyMath::vec3>& curvePoints, int numSegments, char axis = 'X');
    void calculateNormals(); // Может быть частью generateSurface

    void setupBuffers();
    void Draw(Shader& shader, const MyMath::mat4& modelMatrix);
    void clearSurface();

private:
    bool buffersGenerated = false;
};

#endif 
