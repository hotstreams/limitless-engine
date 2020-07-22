#include <assets.hpp>

using namespace GraphicsEngine;

void Assets::load() {
    models.add("test", ModelLoader::loadModel(ASSETS_DIR "models/sponza/sponza.obj"));
}
