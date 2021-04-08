#pragma once

#include <limitless/core/context_thread_pool.hpp>
#include <limitless/util/filesystem.hpp>

#include <limitless/loaders/threaded_model_loader.hpp>
#include <limitless/assets.hpp>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <iterator>
#include <mutex>

#define ASSETS_DIR "../assets/"

#define MATERIAL_DIR ASSETS_DIR"materials/"
#define TEXTURE_DIR ASSETS_DIR"textures/"
#define EFFECT_DIR ASSETS_DIR"effects/"
#define MODEL_DIR ASSETS_DIR"models/"

namespace LimitlessEngine {
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
    public:
        AssetManager(Context& context, Assets& assets, Context& shared, uint32_t pool_size = std::thread::hardware_concurrency());
        //TODO: wait on loading
        ~AssetManager() = default;

        void loadModel(std::string asset_name, fs::path path, bool flip_uv = false);
        void loadTexture(std::string asset_name, fs::path path, bool bottom_left_start = true);

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
