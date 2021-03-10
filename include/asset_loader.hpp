#pragma once

#include <core/context_thread_pool.hpp>
#include <util/filesystem.hpp>

#include <threaded_model_loader.hpp>
#include <assets.hpp>
#include <texture_loader.hpp>

namespace LimitlessEngine {
    class AssetManager {
    private:
        using future_model = std::future<std::function<std::shared_ptr<AbstractModel>()>>;
        using future_asset = std::future<void>;

        struct model_postwork { future_model future; std::function<void(future_model&)> addition; };
        std::vector<model_postwork> model_futures;
        std::vector<future_asset> asset_futures;
        ContextThreadPool pool;
    public:
        explicit AssetManager(Context& shared, uint32_t pool_size = std::thread::hardware_concurrency());
        ~AssetManager() = default;

        void loadModel(std::string asset_name, fs::path path, bool flip_uv = false);
        void loadTexture(std::string asset_name, fs::path path, bool bottom_left_start = true);
        //TODO:

//        void loadMaterial(std::string name, fs::path path);
//        void loadEffect(std::string name, fs::path path);



        struct model_loading { std::string asset_name; fs::path path; bool flip_uv{}; };
        void loadModels(const std::vector<model_loading>& models);

        struct texture_loading { std::string asset_name; fs::path path; bool bottom_left_origin{true}; };
        void loadTextures(const std::vector<texture_loading>& textures);

        void build(std::function<void()> f);

        // makes work on ready futures
        void delayed_work();
        // checks if there is work; if true you should call delayed_work() then;
        bool isDone();
        // waits until all is completed
        void wait();
    };
}
