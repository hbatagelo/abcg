#ifndef PARTICLE_HPP_
#define PARTICLE_HPP_

#include "abcgOpenGL.hpp"

class Particle {
public:
    Particle(const glm::vec2& pos, const glm::vec2& vel, const float mass, const glm::vec3& col);

    void updatePos(float dt);
    const glm::vec2& getPos() const;
    const glm::vec3& getColor() const;
    float getRadius() const;
    const glm::vec2& getMomentum() const;

    void interact(Particle& particle, float gravity);
    void check_for_absorption(Particle& particle);
    void resetForce();

    bool was_absorbed() const;

private:
    void addForce(const glm::vec2& force);
    bool intersect(const Particle& particle) const;

    glm::vec2 m_Pos_;
    glm::vec2 m_Vel_;
    glm::vec3 m_Col_;
    float m_Mass_;
    float m_Radius_;

    glm::vec2 m_Momentum_;
    glm::vec2 m_Acceleration_;
    glm::vec2 m_Res_force_;

    //if this particles was absorbed by another one
    //used to avoid absorbing the same particle twice
    bool m_Absorbed = false;
};

#endif
