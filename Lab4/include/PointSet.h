#ifndef POINT_SET_H
#define POINT_SET_H

#include <vector>
#include <MyMath/vec3.h>
#include "Shader.h"

class PointSet {
public:
    std::vector<MyMath::vec3> points;
    unsigned int VAO, VBO;

    PointSet();
    ~PointSet();

    void addPoint(float x, float y);
    void addPoint(const MyMath::vec3& point);
    const std::vector<MyMath::vec3>& getPoints() const;
    size_t getNumPoints() const;
    void clearPoints();

    void setupBuffers();
    void updateBuffers();
    void Draw(Shader& shader);

private:
    bool buffersGenerated = false;
};

#endif 
