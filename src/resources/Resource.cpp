#include "Resource.h"
#include "../util/Log.h"

ResourceId Resource::s_idCounter = 0;

Resource::Resource(const std::filesystem::path &path) : m_id(s_idCounter++), m_path(path)
{
    Log::logger().info("Resource \"{}\" with id {} created", m_path.string(), m_id);
}

ResourceId Resource::id() const
{
    return m_id;
}

bool Resource::isInitialized() const
{
    return m_initialized;
}

const std::filesystem::path &Resource::resourcePath() const
{
    return m_path;
}

GLuint GlResource::glId() const
{
    return m_glId;
}

NamedResource::NamedResource(const std::string &name) : m_name(name)
{}

const std::string &NamedResource::name() const
{
    return m_name;
}

NamedResource::~NamedResource()
{}
