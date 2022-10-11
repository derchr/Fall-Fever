#include "ResourceHandler.h"
#include "../util/Log.h"
#include "CubeMap.h"

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

template ResourceId ResourceHandler::registerResource<TextureCubeMap>(TextureCubeMapDescriptor const &);
template ResourceId ResourceHandler::registerResource<InternalCubeMap>(int const &);

auto ResourceHandler::resource(const ResourceId resourceId) const -> std::shared_ptr<Resource>
{
    auto resourceIt = m_resources.find(resourceId);

    if (resourceIt != m_resources.end()) {
        auto resource = resourceIt->second;
        return resource;
    }

    Log::logger().warn("Could not find resource with id {}", resourceId);
    return {};
}
