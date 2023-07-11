#pragma once

#include <limitless/util/resource_container.hpp>
#include <limitless/shader_storage.hpp>
#include <limitless/util/filesystem.hpp>
#include <limitless/loaders/texture_loader.hpp>

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

    /**
     * Assets is a class that contains a set of resources
     *
     * Besides common assets as models, materials, textures, effects it is responsible for shader compilation and storage
     *
     * Using this class you can compile, recompile and hot-reload assets at run-time
     *
     *
     * Use cases:
     *      When you want to:
     *
     *      add asset
     *      compile asset
     *      compile all assets
     *      recompile asset
     *      remove?
     */
    class Assets {
    private:
        static ShaderTypes getRequiredPassShaders(const RenderSettings& settings);

    protected:
        fs::path base_dir;
        fs::path shader_dir;
    public:
        /**
         * Resource containers that hold specific type of assets
         */
        ResourceContainer<AbstractModel> models;
        ResourceContainer<AbstractMesh> meshes;
        ResourceContainer<Texture> textures;
        ResourceContainer<ms::Material> materials;
        ResourceContainer<Skybox> skyboxes;
        ResourceContainer<EffectInstance> effects;
        ResourceContainer<FontAtlas> fonts;

        /**
         * Single resource container for shaders
         */
        ShaderStorage shaders;

        explicit Assets(const fs::path& base_dir) noexcept;
        Assets(fs::path base_dir, fs::path shader_dir) noexcept;

        virtual ~Assets() = default;

        /**
         *  Loads common assets:
         *
         *  red, blue, green materials
         *
         *  simple models and meshes: sphere, quad, cube, plane, line, cylinder
         */
        virtual void load(Context& context);

        /**
         *
         */
        virtual void initialize(Context& ctx, const RenderSettings& settings);

        /**
         *  Compiles shader for specified Material with passed RenderSettings if not already compiled
         *
         *  adds created shader/shaders to shader storage
         */
        void compileMaterial(Context& ctx, const RenderSettings& settings, const std::shared_ptr<ms::Material>& material);

        /**
         *  Compiles shader for specified Effect with passed RenderSettings if not already compiled
         *
         *  adds created shader/shaders to shader storage
         */
        void compileEffect(Context& ctx, const RenderSettings& settings, const std::shared_ptr<EffectInstance>& effect);

        /**
         *  Compiles shader for specified Skybox with passed RenderSettings if not already compiled
         *
         *  adds created shader/shaders to shader storage
         */
        void compileSkybox(Context& ctx, const RenderSettings& settings, const std::shared_ptr<Skybox>& skybox);

        /**
         *  Recompiles shader for specified Material with passed RenderSettings
         *
         *  Updates shader itself without modifying material index, so already existing instances remain valid
         *
         *  The main usage is when you update your RenderSettings, NOT your material
         *
         *  If you updated your material, you should use compileMaterial
         */
        void recompileMaterial(Context& ctx, const RenderSettings& settings, const std::shared_ptr<ms::Material>& material);

        /**
         *  Compiles all assets
         */
        virtual void compileAssets(Context& ctx, const RenderSettings& settings);

        /**
         *
         */
        void recompileAssets(Context& ctx, const RenderSettings& settings);

        /**
         *
         */
        void add(const Assets& other);

        [[nodiscard]] const auto& getBaseDir() const noexcept { return base_dir; }
        [[nodiscard]] const auto& getShaderDir() const noexcept { return shader_dir; }

        void reloadTextures(const TextureLoaderFlags& settings);
    };
}
