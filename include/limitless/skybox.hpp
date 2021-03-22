#pragma once

#include <limitless/util/filesystem.hpp>

#include <memory>

namespace LimitlessEngine {
    class Texture;
    class Context;

    class Skybox {
    private:
        std::shared_ptr<Texture> cubemap;
    public:
        explicit Skybox(const fs::path& path);
        virtual ~Skybox() = default;

        virtual void draw(Context& context);
    };
}