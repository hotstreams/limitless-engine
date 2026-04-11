#include <limitless/core/buffer/indexed_buffer.hpp>
#include <limitless/core/context_initializer.hpp>
#include <limitless/core/cpu_profiler.hpp>

#include <iostream>

using namespace Limitless;

std::optional<std::shared_ptr<Buffer>> IndexedBuffer::get(Type type, const std::string& name) noexcept {
    CpuProfileScope scope(global_profiler, "IndexedBuffer::get");
    auto it = buffers.find(Identifier{type, name});
    if (it != buffers.end()) {
        return it->second;
    }
    return std::nullopt;
}

void IndexedBuffer::add(Type type, const std::string& name, std::shared_ptr<Buffer> buffer) noexcept {
    CpuProfileScope scope(global_profiler, "IndexedBuffer::add");
    std::cerr << "Adding buffer: " << name << "\n";
    buffers.emplace(Identifier{type, name}, std::move(buffer));
}

GLuint IndexedBuffer::getBindingPoint(Type type, std::string_view name) noexcept {
    const auto identifier = Identifier{type, name};

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

void IndexedBuffer::remove(Type type, const std::string &name) {
    CpuProfileScope scope(global_profiler, "IndexedBuffer::remove");
    std::cerr << "Removing buffer: " << name << "\n";
    buffers.erase(Identifier{type, name});
}
