#include <limitless/instances/terrain_instance.hpp>

using namespace Limitless;
using namespace Limitless::ms;

void TerrainInstance::update(const Camera &camera) {
    ModelInstance::update(camera);

    snap(camera);


    _data.cross->update(camera);

    for (auto &item: _data.seams) {
        item->update(camera);
    }

    for (auto &item: _data.trims) {
        item->update(camera);
    }

    for (auto &item: _data.fillers) {
        item->update(camera);
    }

    for (auto &item: _data.tiles) {
        item->update(camera);
    }
}
