#pragma once

#include "Resource.h"

class ShaderProgram : public GlResource, public NamedResource
{
public:
    ShaderProgram();

    void bind() const;
    void unbind() const override;
}