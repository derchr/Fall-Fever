#pragma once

#include <filesystem>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <string_view>
#include <unordered_map>

struct Shader
{
    Shader(std::string_view name, std::filesystem::path const &directory);

    Shader(Shader const &) = delete;
    auto operator=(Shader const &) -> Shader & = delete;

    Shader(Shader &&other) noexcept : program(other.program) { other.program = 0; }
    auto operator=(Shader &&other) noexcept -> Shader &
    {
        program = other.program;
        other.program = 0;
        return *this;
    };

    ~Shader();

    void bind() const;
    static void unbind();

    template <typename T>
    void set_uniform(std::string_view name, T value) const;

private:
    auto retrieveUniformLocation(std::string_view uniform_name) const -> GLint;
    static auto parse(const std::filesystem::path &path) -> std::string;
    static auto compile(std::string_view source, GLenum type) -> GLuint;

    GLuint program;
    mutable std::unordered_map<std::string, GLint> uniform_location_cache;
};

struct ShaderLoader
{
    using result_type = std::shared_ptr<Shader>;
    static constexpr std::string_view shader_directory{"data/shaders"};

    auto operator()(std::string_view name) -> result_type
    {
        return std::make_shared<Shader>(name, shader_directory);
    }
};
