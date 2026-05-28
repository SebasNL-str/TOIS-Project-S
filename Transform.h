#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

struct Transform
{
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f); // En grados // Degrees
    glm::vec3 scale = glm::vec3(1.0f);

    // --- Funciones de transformación | Transform func ---
    void translate(const glm::vec3& delta) {
        position += delta;
    }

    void rotateX(float angleDegrees) {
        rotation.x += angleDegrees;
    }

    void rotateY(float angleDegrees) {
        rotation.y += angleDegrees;
    }

    void rotateZ(float angleDegrees) {
        rotation.z += angleDegrees;
    }

    void scaleX(float factor) {
        scale.x *= factor;
    }

    void scaleY(float factor) {
        scale.y *= factor;
    }

    void scaleZ(float factor) {
        scale.z *= factor;
    }

    void uniformScale(float factor) {
        scale *= factor;
    }

    // --- Construcción de la matriz model | Model matrix construction ---
    glm::mat4 getMatrix() const {
        glm::mat4 model(1.0f);

        // Primero traslación: define el centro en el mundo || Traslation to the origin
        model = glm::translate(model, position);

        // Rotaciones alrededor de ese centro | Rotations within that center
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));

        // Escala respecto a ese mismo centro | Scaling within that center
        model = glm::scale(model, scale);

        return model;
    }
};
