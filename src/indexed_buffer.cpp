#include <indexed_buffer.hpp>
#include <context_initializer.hpp>

using namespace GraphicsEngine;

std::shared_ptr<Buffer> IndexedBuffer::get(std::string_view name) {
    const auto count = buffers.count(name.data());

    switch (count) {
        case 0:
            throw buffer_not_found("Failed to find IndexedBuffer with name " + std::string(name.data()));
        case 1:
            return buffers.find(name.data())->second;
        default:
            throw multiple_indexed_buffers("More than one buffer, should be set manually which one");
    }
}

void IndexedBuffer::add(std::string_view name, std::shared_ptr<Buffer> buffer) noexcept {
    buffers.emplace(name, std::move(buffer));
}

GLuint IndexedBuffer::getBindingPoint(Type type, std::string_view name) noexcept {
    const auto identifier = Identifier{ type, name };

    const auto point_bound = bound.find(identifier);
    if (point_bound != bound.end()) {
        return point_bound->second;
    }

    GLuint bind;

    //auto& found = current_bind[type];

    auto found = current_bind.find(type);
    if (found == current_bind.end()) {
        bind = 0;
        current_bind.emplace(type, bind);
    } else {
        auto& next_bind = ++found->second;
        switch (type) {
            case IndexedBuffer::Type::UniformBuffer:
                if (next_bind >= ContextInitializer::limits.uniform_buffer_max_count) {
                    next_bind = 0;
                }
                break;
            case IndexedBuffer::Type::ShaderStorage:
                if (next_bind >= ContextInitializer::limits.shader_storage_max_count) {
                    next_bind = 0;
                }
                break;
        }
        bind = next_bind;
    }

    bound.emplace(identifier, bind);

    return bind;
}

void IndexedBuffer::remove(const std::string &name, const std::shared_ptr<Buffer>& buffer) {
    if (buffer == nullptr) return;

    auto found = buffers.find(name);

    while (found->second != buffer) { ++found; }

    buffers.erase(found);
}
