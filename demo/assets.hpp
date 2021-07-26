#pragma once

#include <limitless/assets.hpp>
#include <limitless/pipeline/renderer.hpp>

class DemoAssets : public Limitless::Assets {
private:
    void loadLightingScene();
    void loadMaterialsScene();
    void loadEffectsScene();
    void loadModelsScene();
    void loadSponzaScene();
    void loadAssets();
public:
    DemoAssets(Limitless::Context& ctx, Limitless::Renderer& renderer, const fs::path& path);
    ~DemoAssets() override = default;
};
