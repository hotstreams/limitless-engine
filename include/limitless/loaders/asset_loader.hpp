#pragma once

#include <limitless/core/context_thread_pool.hpp>
#include <limitless/models/abstract_model.hpp>
#include <limitless/util/filesystem.hpp>

#include <limitless/loaders/model_loader.hpp>
#include <limitless/loaders/texture_loader.hpp>

static constexpr const char* ASSETS_DIR = ENGINE_ASSETS_DIR;

static constexpr const char* MATERIAL_DIR = ENGINE_ASSETS_DIR "materials/";
static constexpr const char* TEXTURE_DIR = ENGINE_ASSETS_DIR "textures/";
static constexpr const char* EFFECT_DIR = ENGINE_ASSETS_DIR "effects/";
static constexpr const char* MODEL_DIR = ENGINE_ASSETS_DIR "models/";

namespace Limitless {
    class Assets;

    class AssetManager final {
    private:
        using future_model = std::future<std::function<std::shared_ptr<AbstractModel>()>>;
        using future_asset = std::future<void>;

        struct model_postproc {
            future_model future;
            std::function<void(future_model&)> addition;
        };

        std::vector<model_postproc> model_futures;
        std::vector<future_asset> asset_futures;
        ContextThreadPool pool;

        Context& context;
        Assets& assets;
        const RenderSettings& settings;
    public:
        AssetManager(Context& context, Assets& assets, const RenderSettings& settings, uint32_t pool_size = std::thread::hardware_concurrency());
        ~AssetManager();

        void loadModel(std::string asset_name, fs::path path, ModelLoaderFlags flags = {});
        void loadTexture(std::string asset_name, fs::path path, TextureLoaderFlags flags = {});

        void loadMaterial(std::string asset_name, fs::path path);
        void loadEffect(std::string asset_name, fs::path path);

        void build(std::function<void()> f);
        // makes job on ready futures
        void delayed_job();
        // checks if there is job; if true you should call delayed_job() then;
        bool isDone();
        // waits until all is completed
        void wait();

        operator bool() { return isDone(); }
    };
}
