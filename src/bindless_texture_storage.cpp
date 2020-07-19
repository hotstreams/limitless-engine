#include <bindless_texture_storage.hpp>

using namespace GraphicsEngine;

void BindlessTextureStorage::add(GLuint64 new_handle) {
    for (auto& [handle, alive] : handles) {
        if (!alive) {
            handle = new_handle;
            alive = true;
        }
        return;
    }

    throw std::runtime_error("Bindless texture storage capacity not enough to hold handles.");
}

void BindlessTextureStorage::remove(GLuint64 old_handle) noexcept {
    for (auto& [handle, alive] : handles) {
        if (handle == old_handle) {
            alive = false;
        }
        break;
    }
}

void BindlessTextureStorage::map() {
    std::array<GLuint64, max_handles> data{};
    for (uint64_t i = 0; i < max_handles; ++i) {
        data[i] = handles[i].handle;
    }

    handle_storage->mapData(data.data(), sizeof(GLuint64) * max_handles);
}

[[nodiscard]] uint64_t BindlessTextureStorage::getIndex(GLuint64 handle) {
    for (uint64_t i = 0; i < max_handles; ++i) {
        if (handles[i].handle == handle) {
            return i;
        }
    }

    throw std::runtime_error("No such handle.");
}

void BindlessTextureStorage::init() {
    if (!inited) {
        std::vector<GLuint64> data(max_handles);
        handle_storage = BufferBuilder::buildIndexed("handle_storage", Buffer::Type::ShaderStorage, data, Buffer::Usage::DynamicDraw, Buffer::MutableAccess::WriteOrphaning);
        inited = true;
    }
}