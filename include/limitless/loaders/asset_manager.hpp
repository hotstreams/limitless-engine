#pragma once

#include <limitless/core/context_thread_pool.hpp>
#include <limitless/models/abstract_model.hpp>
#include <limitless/util/filesystem.hpp>
#include <limitless/loaders/model_loader.hpp>
#include <limitless/loaders/texture_loader.hpp>

namespace Limitless {
    class Assets;

    fs::path getAssetsDir();
    fs::path getShadersDir();

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

        Assets& assets;
    public:
        AssetManager(Context& context, Assets& assets, uint32_t pool_size = std::thread::hardware_concurrency());
        ~AssetManager();

        void loadModel(std::string asset_name, fs::path path, const ModelLoaderFlags& flags = {});
        void loadTexture(fs::path path, const TextureLoaderFlags& flags = TextureLoaderFlags{});

        void loadMaterial(std::string asset_name, fs::path path);
        void loadEffect(std::string asset_name, fs::path path);

        void build(std::function<void()> f);

        // does a delayed job
        // constructs loaded models because VertexArray is not shared between contexts
        void doDelayedJob();

        // compiles all required shaders
        void compileShaders(Context& ctx, const RenderSettings& settings);

        void wait();

        bool isDone();
        operator bool() { return isDone(); }
    };
}
