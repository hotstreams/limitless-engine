#pragma once

#include <limitless/util/resource_container.hpp>
#include <limitless/shader_storage.hpp>
#include <limitless/util/filesystem.hpp>

namespace Limitless::ms {
    class Material;
}

namespace Limitless {
    class AbstractMesh;
    class AbstractModel;
    class EffectInstance;
    class Texture;
    class Skybox;
    class FontAtlas;
    class Context;
    class RenderSettings;

    class Assets {
    private:
        fs::path base_dir;
    public:
        ShaderStorage shaders;
        ResourceContainer<AbstractModel> models;
        ResourceContainer<AbstractMesh> meshes;
        ResourceContainer<Texture> textures;
        ResourceContainer<ms::Material> materials;
        ResourceContainer<Skybox> skyboxes;
        ResourceContainer<EffectInstance> effects;
        ResourceContainer<FontAtlas> fonts;

        Assets(fs::path _base_dir = ENGINE_ASSETS_DIR);
        virtual ~Assets() = default;

        virtual void load(Context& context, const RenderSettings& settings);
        [[nodiscard]] const auto& getBaseDir() const noexcept { return base_dir; }
        [[nodiscard]] auto getShaderDir() const noexcept {
            return base_dir / "../shaders";
        }

        void merge(const Assets& other);
    };
}
