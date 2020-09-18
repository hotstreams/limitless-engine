#include <texture_binder.hpp>
#include <context_state.hpp>
#include <context_initializer.hpp>
#include <algorithm>

using namespace GraphicsEngine;

GLint TextureBinder::bind(Texture& texture) noexcept {
    // contains [unit_index, texture_id]
    const auto& texture_bound = ContextState::getState(glfwGetCurrentContext())->texture_bound;

    // checks if there is already bound texture
    const auto already_bound = std::find_if(texture_bound.begin(), texture_bound.end(), [&] (const auto& bind) { return bind.second == texture.getId(); });
    if (already_bound != texture_bound.end()) {
        return already_bound->first;
    }

    // checks for free slots
    const auto empty_slot = std::find_if(texture_bound.begin(), texture_bound.end(), [] (const auto& bind) { return bind.second == 0; });
    if (empty_slot != texture_bound.end()) {
        texture.bind(empty_slot->first);
        return empty_slot->first;
    }

    const auto unit = current_bind;
    texture.bind(unit);

    current_bind = ++current_bind >= ContextInitializer::limits.max_texture_units ? 0 : current_bind;

    return unit;
}

std::vector<GLint> TextureBinder::bind(const std::vector<const Texture*>& textures) {
    if (textures.size() > static_cast<size_t>(ContextInitializer::limits.max_texture_units)) {
        throw std::runtime_error("Failed to bind textures which more than texture units");
    }

    using IndexMap = std::map<uint32_t, const Texture*>;
    auto& texture_bound = ContextState::getState(glfwGetCurrentContext())->texture_bound;
    std::vector<GLint> indices(textures.size(), -1);

    IndexMap bind_map;
    uint32_t i = 0;
    for (const auto texture : textures) {
        bind_map.emplace(i++, texture);
    }

    // checks if there is already bound to context textures and creates map with it
    IndexMap already_bound;
    for (const auto& [index, texture] : bind_map) {
        const auto& tex_ptr = texture;
        const auto found = std::find_if(texture_bound.begin(), texture_bound.end(), [&] (const auto& bind) { return bind.second == tex_ptr->getId(); });
        if (found != texture_bound.end()) {
            indices[index] = found->first;
            already_bound.emplace(index, texture);
        }
    }

    // creates map with unbound textures
    IndexMap unbound_map;
    std::set_difference(bind_map.begin(), bind_map.end(), already_bound.begin(), already_bound.end(), std::inserter(unbound_map, unbound_map.begin()));

    // checks for free texture unit slots in context
    IndexMap empty_bound;
    for (const auto& [index, texture] : unbound_map) {
        auto found = std::find_if(texture_bound.begin(), texture_bound.end(), [] (const auto& bind) { return bind.second == 0; });
        if (found != texture_bound.end()) {
            indices[index] = found->first;
            empty_bound.emplace(index, texture);
            texture->bind(found->first);
        }
    }

    IndexMap replace_map;
    std::set_difference(unbound_map.begin(), unbound_map.end(), empty_bound.begin(), empty_bound.end(), std::inserter(replace_map, replace_map.begin()));

    for (const auto& [index, texture] : replace_map) {
        std::vector<int>::iterator found;
        while (found != indices.end()) {
            found = std::find_if(indices.begin(), indices.end(), [] (const auto& index) { return index == current_bind; });
            if (found == indices.end()) {
                indices[index] = current_bind;
                texture->bind(current_bind);
                current_bind = ++current_bind >= ContextInitializer::limits.max_texture_units ? 0 : current_bind;
            }
        }
    }

    return indices;
}