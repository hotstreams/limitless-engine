#include <assets.hpp>
#include <elementary_model.hpp>

using namespace GraphicsEngine;

void Assets::load() {
   // models.add("sponza", ModelLoader::loadModel(ASSETS_DIR "models/sponza/sponza.obj"));
    models.add("bob", ModelLoader::loadModel(ASSETS_DIR "models/boblamp/boblampclean.md5mesh"));
    models.add("backpack", ModelLoader::loadModel(ASSETS_DIR "models/backpack/backpack.obj", true));
    models.add("nanosuit", ModelLoader::loadModel(ASSETS_DIR "models/nanosuit/nanosuit.obj"));
    models.add("cyborg", ModelLoader::loadModel(ASSETS_DIR "models/cyborg/cyborg.obj"));

    models.add("sphere", std::make_shared<Sphere>(100, 100));
    models.add("quad", std::make_shared<Quad>());
    models.add("cube", std::make_shared<Cube>());
}
