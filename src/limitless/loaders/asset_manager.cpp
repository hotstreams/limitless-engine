#include <limitless/loaders/asset_manager.hpp>

#include <limitless/loaders/threaded_model_loader.hpp>
#include <limitless/loaders/texture_loader.hpp>
#include <limitless/loaders/material_loader.hpp>
#include <limitless/loaders/effect_loader.hpp>
#include <limitless/assets.hpp>

using namespace Limitless;

AssetManager::AssetManager(Context& _context, Assets& _assets, uint32_t pool_size)
    : pool {_context, pool_size}
    , assets {_assets} {
}

AssetManager::~AssetManager() {
    wait();
}

void AssetManager::loadTexture(fs::path path, const TextureLoaderFlags& flags) {
    auto load_texture = [&, path = std::move(path), fl = flags] () {
        TextureLoader::load(assets, path, fl);
    };

    asset_futures.emplace_back(pool.add(std::move(load_texture)));
}

void AssetManager::loadModel(std::string asset_name, fs::path path, const ModelLoaderFlags& flags) {
    auto load_model = [&, name = asset_name, path = std::move(path), fl = flags] () {
        return ThreadedModelLoader::loadModel(assets, path, fl);
    };

    auto addition = [&, name = std::move(asset_name)] (future_model& future) {
        assets.models.add(name, future.get()());
    };

    model_futures.emplace_back(model_postproc{pool.add(std::move(load_model)), std::move(addition)});
}

bool AssetManager::isDone() {
    using namespace std::chrono;

    for (auto it = asset_futures.begin(); it != asset_futures.end();) {
        if (it->wait_for(0ns) == std::future_status::ready) {
            it->get();
            it = asset_futures.erase(it);
        } else {
            return false;
        }
    }

    return std::all_of(model_futures.begin(), model_futures.end(), [] (auto& post) {
        return post.future.wait_for(0ns) == std::future_status::ready;
    });
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

void AssetManager::doDelayedJob() {
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

void AssetManager::build(std::function<void()> f) {
    asset_futures.emplace_back(pool.add(std::move(f)));
}

void AssetManager::loadMaterial(std::string asset_name, fs::path path) {
    auto load_material = [&, name = std::move(asset_name), path = std::move(path)] () {
        assets.materials.add(name, MaterialLoader::load(assets, path));
    };

    asset_futures.emplace_back(pool.add(std::move(load_material)));
}

void AssetManager::loadEffect(std::string asset_name, fs::path path) {
    auto load_effect = [&, name = std::move(asset_name), path = std::move(path)] () {
        assets.effects.add(name, EffectLoader::load(assets, path));
    };

    asset_futures.emplace_back(pool.add(std::move(load_effect)));
}

void AssetManager::compileShaders(Context& ctx, const RenderSettings& settings) {
	//assets.initialize(ctx, settings);

    for (const auto& [_, material] : assets.materials) {
        build([&, &ctx = ctx, &settings = settings, &material = material] () {
            assets.compileMaterial(ctx, settings, material);
        });
    }

    for (const auto& [_, effect] : assets.effects) {
        build([&, &ctx = ctx, &settings = settings, &effect = effect] () {
            assets.compileEffect(ctx, settings, effect);
        });
    }

    for (const auto& [_, skybox] : assets.skyboxes) {
        build([&, &ctx = ctx, &settings = settings, &skybox = skybox] () {
            assets.compileSkybox(ctx, settings, skybox);
        });
    }
}
