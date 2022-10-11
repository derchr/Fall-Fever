#include "Resource.h"
#include "../util/Log.h"

ResourceId Resource::s_idCounter = 0;

Resource::Resource(std::filesystem::path path) : m_id(s_idCounter++), m_path(std::move(path))
{
    Log::logger().info("Resource \"{}\" with id {} created", m_path.string(), m_id);
}

auto Resource::id() const -> ResourceId
{
    return m_id;
}

auto Resource::resourcePath() const -> const std::filesystem::path &
{
    return m_path;
}

auto GlResource::glId() const -> GLuint
{
    return m_glId;
}

NamedResource::NamedResource(std::string name) : m_name(std::move(name))
{
}

auto NamedResource::name() const -> const std::string &
{
    return m_name;
}

NamedResource::~NamedResource() = default;
