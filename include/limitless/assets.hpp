#pragma once

#include <limitless/util/resource_container.hpp>
#include <limitless/shader_storage.hpp>

namespace LimitlessEngine {
    class AbstractMesh;
    class AbstractModel;
    class EffectInstance;
    class Texture;
    class Material;
    class Skybox;
    class FontAtlas;
    class Context;

    class Assets {
    public:
        ShaderStorage shaders;
        ResourceContainer<AbstractModel> models;
        ResourceContainer<AbstractMesh> meshes;
        ResourceContainer<Texture> textures;
        ResourceContainer<Material> materials;
        ResourceContainer<Skybox> skyboxes;
        ResourceContainer<EffectInstance> effects;
        ResourceContainer<FontAtlas> fonts;

        Assets() = default;
        virtual ~Assets() = default;

        virtual void load(Context& context);
    };
}
