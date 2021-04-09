#pragma once

#include <limitless/util/resource_container.hpp>
#include <limitless/shader_storage.hpp>
#include <filesystem>

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
	    std::filesystem::path base_dir;
    public:
        ShaderStorage shaders;
        ResourceContainer<AbstractModel> models;
        ResourceContainer<AbstractMesh> meshes;
        ResourceContainer<Texture> textures;
        ResourceContainer<ms::Material> materials;
        ResourceContainer<Skybox> skyboxes;
        ResourceContainer<EffectInstance> effects;
        ResourceContainer<FontAtlas> fonts;

        Assets(decltype(base_dir) _base_dir);
        virtual ~Assets() = default;

        virtual void load(Context& context, const RenderSettings& settings);
        [[nodiscard]] const auto& getBaseDir() const noexcept { return base_dir; }
        [[nodiscard]] std::filesystem::path getShaderDir() const noexcept;

        virtual void load(Context& context);
    };
}
