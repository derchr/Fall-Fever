#pragma once

#include <cstdint>
#include <filesystem>
#include <glad/gl.h>
#include <string>

using ResourceId = uint64_t;

class Resource
{
public:
    Resource(std::filesystem::path path);
    Resource(const Resource &other) = delete;
    Resource(Resource &&other) = delete;
    auto operator=(const Resource &other) -> Resource & = delete;
    auto operator=(Resource &&other) -> Resource & = delete;

    [[nodiscard]] auto id() const -> ResourceId;
    [[nodiscard]] auto resourcePath() const -> const std::filesystem::path &;
    
private:
    ResourceId m_id;
    static ResourceId s_idCounter;

    std::filesystem::path m_path;

    friend class ResourceHandler;
};

class GlResource
{
public:
    // virtual ~GlResource() = 0; TODO!!
    virtual void unbind() const = 0;

    auto glId() const -> GLuint;

protected:
    GLuint m_glId;
};

class NamedResource
{
public:
    NamedResource(std::string name);
    virtual ~NamedResource() = 0;

    [[nodiscard]] auto name() const -> const std::string &;

private:
    const std::string m_name;
};
