#include "Curve.h"
#include "Shader.h" // Для Shader в Draw
#include <GL/glew.h> // Для OpenGL функций

Curve::Curve() : VAO(0), VBO(0), buffersGenerated(false) {
    // Конструктор
}

Curve::~Curve() {
    if (buffersGenerated) {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
}

// Простая реализация: соединяет контрольные точки линиями.
// controlPoints - точки, заданные пользователем.
// segmentsPerControlPoint - здесь не используется, но оставлен для совместимости с идеей Безье.
void Curve::generateCurve(const std::vector<MyMath::vec3>& controlPoints, int segmentsPerControlPoint) {
    curvePoints.clear();
    if (controlPoints.size() < 2) {
        if (buffersGenerated) updateBuffers(); // Очистить буфер, если он был
        return; // Невозможно построить кривую из менее чем 2 точек
    }

    // Просто копируем контрольные точки для отрисовки ломаной линии
    curvePoints = controlPoints;

    // Если буферы уже созданы, их нужно обновить
    // Если нет, они будут созданы при первом вызове Draw или updateBuffers
    // updateBuffers(); // Вызывается в main.cpp после generateCurve
}

void Curve::setupBuffers() {
    if (curvePoints.empty()) return;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, curvePoints.size() * sizeof(MyMath::vec3), curvePoints.data(), GL_DYNAMIC_DRAW);

    // Атрибут позиции
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MyMath::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    buffersGenerated = true;
}

void Curve::updateBuffers() {
    if (!buffersGenerated && !curvePoints.empty()) {
        setupBuffers();
        return;
    }
    if(!buffersGenerated && curvePoints.empty()){
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    if (!curvePoints.empty()) {
        glBufferData(GL_ARRAY_BUFFER, curvePoints.size() * sizeof(MyMath::vec3), curvePoints.data(), GL_DYNAMIC_DRAW);
    } else {
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW); // Очищаем буфер
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Curve::Draw(Shader& shader) {
    if (curvePoints.empty() || !buffersGenerated) return;

    // shader.Use(); // Предполагается, что шейдер уже активирован в main.cpp
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINE_STRIP, 0, static_cast<GLsizei>(curvePoints.size()));
    glBindVertexArray(0);
}

void Curve::clearCurve(){
    curvePoints.clear();
    if(buffersGenerated){
        updateBuffers();
    }
} 
