#include <limitless/lighting/light_container.hpp>

#include <limitless/core/buffer_builder.hpp>
#include <limitless/lighting/lights.hpp>

using namespace LimitlessEngine;

template<typename T>
LightContainer<T>::LightContainer() : LightContainer{1} {

}

template<typename T>
LightContainer<T>::LightContainer(uint64_t reserve_count) {
    reserve(reserve_count);
}

template<typename T>
void LightContainer<T>::reserve(size_t n) {
    lights.reserve(n);
    lights_map.reserve(n);
    IndexedBuffer::remove(T::shader_storage_name, buffer);
    buffer = BufferBuilder::buildIndexed(T::shader_storage_name, Buffer::Type::ShaderStorage, sizeof(T) * n, Buffer::Usage::DynamicDraw, Buffer::MutableAccess::WriteOrphaning);
    modified = true;
}

template<typename T>
void LightContainer<T>::erase(uint64_t id) {
    modified = true;

    auto del_index = lights_map.at(id);

    lights.erase(lights.begin() + del_index);

    for (auto& [id, index] : lights_map)
        if (index > del_index) --index;
}

template<typename T>
void LightContainer<T>::update() {
    if (modified) {
        buffer->mapData(lights.data(), sizeof(T) * size());
        modified = false;
    }
}

namespace LimitlessEngine {
    template class LightContainer<PointLight>;
    template class LightContainer<DirectionalLight>;
    template class LightContainer<SpotLight>;
}
