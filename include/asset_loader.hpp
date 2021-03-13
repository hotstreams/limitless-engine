#pragma once

#include <core/context_thread_pool.hpp>
#include <util/filesystem.hpp>

#include <threaded_model_loader.hpp>
#include <assets.hpp>
#include <texture_loader.hpp>
#include <unordered_map>
#include <memory>
#include <stdexcept>
#include <iterator>
#include <mutex>

#define ASSETS_DIR "../assets/"

#define MATERIAL_DIR ASSETS_DIR"materials"
#define TEXTURE_DIR ASSETS_DIR"textures"
#define EFFECT_DIR ASSETS_DIR"effects"
#define MODEL_DIR ASSETS_DIR"models"

namespace LimitlessEngine {
    class AssetManager final {
    private:
        using future_model = std::future<std::function<std::shared_ptr<AbstractModel>()>>;
        using future_asset = std::future<void>;

        struct model_postwork {
            future_model future;
            std::function<void(future_model&)> addition;
        };

        std::vector<model_postwork> model_futures;
        std::vector<future_asset> asset_futures;
        ContextThreadPool pool;
    public:
        explicit AssetManager(Context& shared, uint32_t pool_size = std::thread::hardware_concurrency());
        ~AssetManager() = default;

        void loadModel(std::string asset_name, fs::path path, bool flip_uv = false);
        void loadTexture(std::string asset_name, fs::path path, bool bottom_left_start = true);

//        void loadMaterial(std::string name, fs::path path);
//        void loadEffect(std::string name, fs::path path);

        void build(std::function<void()> f);
        // makes job on ready futures
        void delayed_job();
        // checks if there is job; if true you should call delayed_job() then;
        bool isDone();
        // waits until all is completed
        void wait();
    };
}
