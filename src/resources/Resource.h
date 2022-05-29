#pragma once

#include <cstdint>
#include <filesystem>
#include <glad/glad.h>
#include <string>

using ResourceId = uint64_t;

class Resource
{
public:
    Resource(const std::filesystem::path &path);
    Resource(const Resource &other) = delete;
    Resource(Resource &&other) = delete;
    Resource &operator=(const Resource &other) = delete;
    Resource &operator=(Resource &&other) = delete;

    ResourceId id() const;
    const std::filesystem::path &resourcePath() const;

protected:
    bool isInitialized() const;
    virtual void initialize() = 0;

    bool m_initialized = false;

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

    GLuint glId() const;

protected:
    GLuint m_glId;
};

class NamedResource
{
public:
    NamedResource(const std::string &name);
    virtual ~NamedResource() = 0;

    const std::string &name() const;

protected:
    std::string m_name;
};
