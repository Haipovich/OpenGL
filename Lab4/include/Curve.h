#ifndef CURVE_H
#define CURVE_H

#include <vector>
#include <MyMath/vec3.h>
#include "Shader.h" // Для передачи шейдера в метод Draw

class Curve
{
public:
    // VAO, VBO для отрисовки кривой
    unsigned int VAO, VBO;
    std::vector<MyMath::vec3> curvePoints; // Точки для отрисовки линии

    Curve();
    ~Curve();

    // Обновляет точки кривой на основе входных контрольных точек
    // controlPoints - это точки, заданные пользователем
    void generateCurve(const std::vector<MyMath::vec3>& controlPoints, int segmentsPerControlPoint = 10);
    
    void setupBuffers();
    void updateBuffers(); // Обновляет VBO, если curvePoints изменились
    void Draw(Shader& shader);
    void clearCurve();

private:
    bool buffersGenerated = false;
    // Вспомогательные функции для генерации кривой (например, Безье)
    // MyMath::vec3 calculateBezierPoint(const MyMath::vec3& p0, const MyMath::vec3& p1, const MyMath::vec3& p2, const MyMath::vec3& p3, float t);
};

#endif 
