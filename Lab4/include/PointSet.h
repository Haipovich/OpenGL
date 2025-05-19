#ifndef POINT_SET_H
#define POINT_SET_H

#include <vector>
#include <MyMath/vec3.h> // Предполагаем, что точки будут 2D (x,y) но vec3 может быть удобен (z=0)
                        // или можно создать vec2 в MyMath
#include "Shader.h" // Включаем Shader.h для определения класса Shader

class PointSet
{
public:
    std::vector<MyMath::vec3> points;
    // VAO, VBO для отрисовки точек
    unsigned int VAO, VBO;

    PointSet();
    ~PointSet();

    void addPoint(float x, float y);
    void addPoint(const MyMath::vec3& point); // Для удобства
    const std::vector<MyMath::vec3>& getPoints() const;
    size_t getNumPoints() const;
    void clearPoints();

    void setupBuffers(); // Настройка VAO/VBO
    void updateBuffers(); // Обновление VBO при добавлении точек
    void Draw(Shader& shader);

private:
    bool buffersGenerated = false;
};

#endif 
