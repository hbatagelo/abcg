#include "particle.hpp"
#include <iostream>

Particle::Particle(const glm::vec2& pos, const glm::vec2& vel, const float mass, const glm::vec3& col) 
:
m_Pos_(pos),
//we divide the initial velocity by 3 so the particle don't start really fast
m_Vel_(vel/3.f),
m_Col_(col),
m_Mass_(mass),
m_Radius_(std::cbrt(m_Mass_)),
m_Momentum_(m_Vel_*m_Mass_),
m_Acceleration_(0,0),
m_Res_force_(0,0)
{
}

void Particle::updatePos(float dt) {
    //we multiple by 0.5 to avoid slingshot scenarios
    m_Vel_ += dt*m_Acceleration_;
    m_Momentum_ = m_Vel_*m_Mass_;

    //we divide by 10 because the animation would be too fast
    m_Pos_ += m_Vel_/10.f;
}

const glm::vec2& Particle::getPos() const {
    return m_Pos_;
}

const glm::vec3& Particle::getColor() const {
    return m_Col_;
}

float Particle::getRadius() const {
    return m_Radius_;
}

const glm::vec2& Particle::getMomentum() const {
    return m_Momentum_;
}

void Particle::interact(Particle& particle, float gravity) {
    //calculates Newton's gravitational law with some linear algebra
    auto attractive_force = (gravity*m_Mass_*particle.m_Mass_)/glm::distance2(m_Pos_, particle.m_Pos_);
    auto dir_vec = particle.m_Pos_ - m_Pos_;
    auto norm_dir_vec = glm::normalize(dir_vec);
    auto force = attractive_force*norm_dir_vec;
    addForce(force);
    particle.addForce(-force);
}

void Particle::check_for_absorption(Particle& particle) {
    if (!intersect(particle))
        return;

    //marks the particle as absorbed
    particle.m_Absorbed = true;

    //"steals" it's properties
    m_Mass_ += particle.m_Mass_;
    m_Radius_ = std::cbrt(m_Mass_);
    m_Momentum_ += particle.m_Momentum_;
    m_Vel_ = m_Momentum_/m_Mass_;
}

void Particle::resetForce() {
    m_Res_force_ = glm::vec2(0, 0);
    m_Acceleration_ = glm::vec2(0, 0);
}

void Particle::addForce(const glm::vec2& force) {
    m_Res_force_ += force;
    m_Acceleration_ = m_Res_force_/m_Mass_;
}

bool Particle::intersect(const Particle& particle) const {
    //checks if the particles are intersecting
    auto r = m_Radius_ + particle.m_Radius_;
    return glm::distance2(m_Pos_, particle.m_Pos_) <= r * r;
}

bool Particle::was_absorbed() const {
    return m_Absorbed;
}
