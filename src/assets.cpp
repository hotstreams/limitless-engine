#include <assets.hpp>
#include <elementary_model.hpp>

using namespace GraphicsEngine;

void Assets::load() {
    models.add("sponza", ModelLoader::loadModel(ASSETS_DIR "models/sponza/sponza.obj"));
    models.add("bob", ModelLoader::loadModel(ASSETS_DIR "models/boblamp/boblampclean.md5mesh"));

    models.add("sphere", std::make_shared<Sphere>(10, 10));
    models.add("quad", std::make_shared<Quad>());
}
