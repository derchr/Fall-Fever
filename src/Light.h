#pragma once

#include <glm/glm.hpp>
#include <string>

class Shader;

class Light
{
public:
    struct Prototype
    {
        Prototype(const std::string &_name, const std::string &_parent, glm::vec3 _color, float _intensity)
            : name(_name), parent(_parent), color(_color), intensity(_intensity)
        {}
        virtual ~Prototype() = default;

        std::string name;
        std::string parent;
        glm::vec3 color;
        float intensity;
    };

    Light(const std::string &name, glm::vec3 color, float intensity, Shader *shader);
    virtual ~Light() = 0;

    virtual void update() = 0;

    void setActive(bool active);
    void setColor(glm::vec3 color);
    void setIntensity(float intensity);
    void setShader(Shader *shader);

    glm::vec3 getColor();

protected:
    Shader *m_shader;

    uint32_t m_id;
    static uint32_t s_idCounter;

    bool m_isActive = true;

    float m_intensity;

    // Color
    glm::vec3 m_lightColor;
};

class PointLight : public Light
{
public:
    struct Prototype : public Light::Prototype
    {
        Prototype(const std::string &_name, const std::string &_parent, glm::vec3 _position, glm::vec3 _color,
                  float _intensity)
            : Light::Prototype(_name, _parent, _color, _intensity), position(_position)
        {}
        glm::vec3 position;
    };

    PointLight(Prototype prototype, Shader *shader);
    ~PointLight() override = default;

    void setPosition(glm::vec3 position);

    glm::vec3 getPosition();

private:
    void update() override;
    std::string getStructMemberName();

    glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
};

class DirectionalLight : public Light
{
public:
    struct Prototype : public Light::Prototype
    {
        Prototype(const std::string &_name, glm::vec3 _direction, glm::vec3 _color, float _intensity)
            : Light::Prototype(_name, "", _color, _intensity), direction(_direction)
        {}
        glm::vec3 direction;
    };

    DirectionalLight(Prototype prototype, Shader *shader);
    ~DirectionalLight() override = default;

    void setDirection(glm::vec3 direction);

    glm::vec3 getDirection();

private:
    void update() override;

    glm::vec3 m_direction;
};
