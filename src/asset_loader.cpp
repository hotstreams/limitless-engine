#include <asset_loader.hpp>

using namespace LimitlessEngine;

AssetManager::AssetManager(Context& shared, uint32_t pool_size) : pool{shared, pool_size} {}

void AssetManager::loadTexture(std::string asset_name, fs::path path, bool bottom_left_start) {
    auto load_texture = [name = std::move(asset_name), path = std::move(path), bottom_left_start] () {
        assets.textures.add(name, TextureLoader::load(path, bottom_left_start));
    };

    asset_futures.emplace_back(pool.add(std::move(load_texture)));
}

void AssetManager::loadModel(std::string asset_name, fs::path path, bool flip_uv) {
    auto load_model = [name = asset_name, path = std::move(path), flip_uv] () {
        ThreadedModelLoader loader;
        return loader.loadModel(path, flip_uv);
    };

    auto addition = [name = std::move(asset_name)] (future_model& future) {
        assets.models.add(name, future.get()());
    };

    model_futures.emplace_back(model_postwork{pool.add(std::move(load_model)), std::move(addition)});
}

bool AssetManager::isDone() {
    using namespace std::chrono;

    for (auto it = asset_futures.begin(); it != asset_futures.end();) {
        if (it->wait_for(0ms) == std::future_status::ready) {
            it->get();
            it = asset_futures.erase(it);
        } else {
            return false;
        }
    }

    for (auto& [future, addition] : model_futures) {
        if (future.wait_for(0ms) != std::future_status::ready) {
            return false;
        }
    }

    return true;
}

void AssetManager::wait() {
    for (auto& future : asset_futures) {
        future.wait();
        future.get();
    }

    for (auto& [future, addition] : model_futures) {
        future.wait();
        addition(future);
    }

    asset_futures.clear();
    model_futures.clear();
}

void AssetManager::delayed_work() {
    using namespace std::chrono;
    for (auto it = model_futures.begin(); it != model_futures.end();) {
        auto& [future, addition] = *it;

        if (future.wait_for(0ms) == std::future_status::ready) {
            addition(future);
            it = model_futures.erase(it);
        } else {
            ++it;
        }
    }
}

void AssetManager::loadModels(const std::vector<model_loading>& models) {
    for (auto& [name, path, flip] : models) {
        loadModel(name, path, flip);
    }
}

void AssetManager::loadTextures(const std::vector<texture_loading>& textures) {
    for (auto& [name, path, flip] : textures) {
        loadTexture(name, path, flip);
    }
}

void AssetManager::build(std::function<void()> f) {
    asset_futures.emplace_back(pool.add(std::move(f)));
}
