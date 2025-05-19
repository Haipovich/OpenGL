#ifndef REVOLUTION_SURFACE_H
#define REVOLUTION_SURFACE_H

#include <vector>
#include <MyMath/vec3.h>
#include <MyMath/mat4.h>
#include "Shader.h"

struct Vertex {
    MyMath::vec3 Position;
    MyMath::vec3 Normal;
};

class RevolutionSurface {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;

    RevolutionSurface();
    ~RevolutionSurface();

    void generateSurface(const std::vector<MyMath::vec3>& curvePoints, int numSegments, char axis = 'X');
    void calculateNormals();

    void setupBuffers();
    void Draw(Shader& shader, const MyMath::mat4& modelMatrix);
    void clearSurface();

private:
    bool buffersGenerated = false;
};

#endif 
