#include "ResourceHandler.h"
#include "../util/Log.h"
#include "CubeMap.h"
#include "Texture.h"

#include <algorithm>

ResourceHandler ResourceHandler::s_instance;

auto ResourceHandler::instance() -> ResourceHandler &
{
    return s_instance;
}

template <typename T, typename... Param>
auto ResourceHandler::registerResource(Param const &...param) -> ResourceId
{
    auto resource = std::make_shared<T>(param...);
    m_resources.emplace(resource->id(), resource);
    return resource->id();
}

template ResourceId ResourceHandler::registerResource<Texture>(TextureDescriptor const &);
template ResourceId ResourceHandler::registerResource<TextureCubeMap>(TextureCubeMapDescriptor const &);
template ResourceId ResourceHandler::registerResource<InternalCubeMap>(int const &);

auto ResourceHandler::resource(const ResourceId resourceId) const -> std::shared_ptr<Resource>
{
    auto resourceIt = m_resources.find(resourceId);

    if (resourceIt != m_resources.end()) {
        auto resource = resourceIt->second;

        if (!resource->isInitialized()) {
            resource->initialize();
        }

        return resource;
    }

    Log::logger().warn("Could not find resource with id {}", resourceId);
    return {};
}

auto ResourceHandler::resource(const std::string &name) const -> std::shared_ptr<Resource>
{
    auto resourceIt = std::find_if(m_resources.begin(), m_resources.end(), [&name](const auto &resource) {
        if (auto namedResource = std::dynamic_pointer_cast<NamedResource>(resource.second)) {
            return namedResource->name() == name;
        }

        return false;
    });

    if (resourceIt != m_resources.end()) {
        auto resource = resourceIt->second;

        if (!resource->isInitialized()) {
            resource->initialize();
        }

        return resource;
    }

    Log::logger().warn("Could not find resource with unique name \"{}\"", name);
    return {};
}
