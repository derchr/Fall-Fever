#pragma once

#include "Resource.h"

#include <map>
#include <memory>

class ResourceHandler
{
public:
    static ResourceHandler &instance();

    template <typename T, typename... Param>
    ResourceId registerResource(Param const &...param);

    const std::shared_ptr<Resource> resource(ResourceId id) const;
    const std::shared_ptr<Resource> resource(const std::string &name) const;

private:
    ResourceHandler() = default;

    static ResourceHandler s_instance;

    std::map<ResourceId, std::shared_ptr<Resource>> m_resources;
};
