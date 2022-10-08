#pragma once

#include "Resource.h"

#include <map>
#include <memory>

class ResourceHandler
{
public:
    static auto instance() -> ResourceHandler &;

    template <typename T, typename... Param>
    auto registerResource(Param const &...param) -> ResourceId;

    [[nodiscard]] auto resource(ResourceId resourceId) const -> std::shared_ptr<Resource>;
    [[nodiscard]] auto resource(const std::string &name) const -> std::shared_ptr<Resource>;

private:
    ResourceHandler() = default;

    static ResourceHandler s_instance;

    std::map<ResourceId, std::shared_ptr<Resource>> m_resources;
};
