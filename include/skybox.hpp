#pragma once

#include <filesystem.hpp>

#include <memory>

namespace GraphicsEngine {
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