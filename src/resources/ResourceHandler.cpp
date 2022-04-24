#include "ResourceHandler.h"
#include "../util/Log.h"
#include "CubeMap.h"
#include "Model.h"
#include "Texture.h"

#include <algorithm>

ResourceHandler ResourceHandler::s_instance;

ResourceHandler &ResourceHandler::instance()
{
    return s_instance;
}

template <typename T, typename... Param>
ResourceId ResourceHandler::registerResource(Param const &...param)
{
    auto resource = std::make_shared<T>(param...);
    m_resources.emplace(resource->id(), resource);
    return resource->id();
}

template ResourceId ResourceHandler::registerResource<Texture>(TextureDescriptor const &);
template ResourceId ResourceHandler::registerResource<TextureCubeMap>(TextureCubeMapDescriptor const &);
template ResourceId ResourceHandler::registerResource<InternalCubeMap>(int const &);
template ResourceId ResourceHandler::registerResource<Model>(ModelDescriptor const &);

const std::shared_ptr<Resource> ResourceHandler::resource(const ResourceId id) const
{
    auto it = m_resources.find(id);

    if (it != m_resources.end()) {
        auto resource = it->second;

        if (!resource->isInitialized())
            resource->initialize();

        return resource;
    }

    Log::logger().warn("Could not find resource with id {}", id);
    return std::shared_ptr<Resource>();
}

const std::shared_ptr<Resource> ResourceHandler::resource(const std::string &name) const
{
    auto it = std::find_if(m_resources.begin(), m_resources.end(), [&name](const auto &resource) {
        if (auto namedResource = std::dynamic_pointer_cast<NamedResource>(resource.second)) {
            return namedResource->name() == name;
        }

        return false;
    });

    if (it != m_resources.end()) {
        auto resource = it->second;

        if (!resource->isInitialized())
            resource->initialize();

        return resource;
    }

    Log::logger().warn("Could not find resource with unique name \"{}\"", name);
    return std::shared_ptr<Resource>();
}
