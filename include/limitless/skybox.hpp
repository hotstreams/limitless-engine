#pragma once

#include <limitless/util/filesystem.hpp>

#include <memory>

namespace LimitlessEngine {
    class Texture;
    class Context;
    class Assets;

    class Skybox {
    private:
        std::shared_ptr<Texture> cubemap;
    public:
        Skybox(Assets& assets, const fs::path& path);
        virtual ~Skybox() = default;

        virtual void draw(Context& context, const Assets& assets);
    };
}