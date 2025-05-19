#ifndef CURVE_H
#define CURVE_H

#include <vector>
#include <MyMath/vec3.h>
#include "Shader.h"

class Curve {
public:
    unsigned int VAO, VBO;
    std::vector<MyMath::vec3> curvePoints;

    Curve();
    ~Curve();

    void generateCurve(const std::vector<MyMath::vec3>& controlPoints, int segmentsPerControlPoint = 10);
    
    void setupBuffers();
    void updateBuffers();
    void Draw(Shader& shader);
    void clearCurve();

private:
    bool buffersGenerated = false;
};

#endif 
