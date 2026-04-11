#include <limitless/lighting/lighting.hpp>

#include <limitless/core/context.hpp>
#include <limitless/core/buffer/buffer_builder.hpp>
#include <limitless/util/frustum.hpp>
#include <limitless/camera.hpp>
#include <algorithm>
#include <cmath>

using namespace Limitless;

constexpr auto SCENE_LIGHTING_BUFFER_NAME = "scene_lighting";
static constexpr auto TILE_LIGHT_GRID_BUFFER_NAME = "TILE_LIGHT_GRID";
static constexpr auto TILE_LIGHT_INDICES_BUFFER_NAME = "TILE_LIGHT_INDICES";

void Lighting::createLightBuffer() {
    buffer = Buffer::builder()
            .target(Buffer::Type::ShaderStorage)
            .usage(Buffer::Usage::DynamicDraw)
            .access(Buffer::MutableAccess::WriteOrphaning)
            .size(sizeof(SceneLighting))
            .build(SCENE_LIGHTING_BUFFER_NAME, context);
}

Lighting::Lighting(Context& ctx)
    : directional_light {Light::builder().buildDefaultDirectional()}
    , context {ctx}
    , changed {true} {
    createLightBuffer();

    // Allocate empty tile buffers; they will be resized on first update(camera).
    tile_grid_buffer = Buffer::builder()
        .target(Buffer::Type::ShaderStorage)
        .usage(Buffer::Usage::DynamicDraw)
        .access(Buffer::MutableAccess::WriteOrphaning)
        .size(sizeof(glm::uvec2)) // at least 1 element
        .build(TILE_LIGHT_GRID_BUFFER_NAME, context);

    tile_indices_buffer = Buffer::builder()
        .target(Buffer::Type::ShaderStorage)
        .usage(Buffer::Usage::DynamicDraw)
        .access(Buffer::MutableAccess::WriteOrphaning)
        .size(sizeof(uint32_t)) // at least 1 element
        .build(TILE_LIGHT_INDICES_BUFFER_NAME, context);
}

Lighting::~Lighting() {
    context.getIndexedBuffers().remove(IndexedBuffer::Type::ShaderStorage, SCENE_LIGHTING_BUFFER_NAME);
}

const glm::vec4& Lighting::getAmbientColor() const noexcept {
    return ambient_color;
}

glm::vec4& Lighting::getAmbientColor() noexcept {
    change();
    return ambient_color;
}

void Lighting::setAmbientColor(const glm::vec4& _ambient_color) noexcept {
    ambient_color = _ambient_color;
    change();
}

const Light& Lighting::getDirectionalLight() const noexcept {
    return directional_light;
}

Light& Lighting::getDirectionalLight() noexcept {
    return directional_light;
}

void Lighting::setDirectionalLight(const Light& _directional_light) {
    directional_light = _directional_light;
    change();
}

bool Lighting::isChanged() const noexcept {
    return changed;
}

void Lighting::resetChanged() noexcept {
    changed = false;
}

void Lighting::change() noexcept {
    changed = true;
}

const std::map<uint64_t, Light>& Lighting::getLights() const noexcept {
    return punctual_lights.getLights();;
}

std::map<uint64_t, Light>& Lighting::getLights() noexcept {
    return punctual_lights.getLights();
}

LightContainer& Lighting::getLightContainer() noexcept {
    return punctual_lights;
}

Light& Lighting::add(Light&& light) {
    if (light.isDirectional()) {
        directional_light = std::move(light);
        return directional_light;
    }

    return punctual_lights.add(std::move(light));
}

Light& Lighting::add(const Light& light) {
    if (light.isDirectional()) {
        directional_light = light;
        return directional_light;
    }

    return punctual_lights.add(light);
}


void Lighting::updateSceneLightBuffer() {
    SceneLighting light_info {
        { directional_light.getDirection(), 0.0f },
        directional_light.getColor(),
        ambient_color,
        static_cast<uint32_t>(punctual_lights.visibleSize())
    };

    buffer->mapData(&light_info, sizeof(SceneLighting));
}

void Lighting::update() {
    punctual_lights.update();

    if (isChanged() || directional_light.isChanged()) {
        updateSceneLightBuffer();
        resetChanged();
        directional_light.resetChanged();
    }

    // binds light buffer to the context
    // in case if there are many scenes or lighting classes
    buffer->bindBase(context.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::ShaderStorage, SCENE_LIGHTING_BUFFER_NAME));
}

static glm::uvec2 getTileCounts(glm::uvec2 resolution, uint32_t tile_size) {
    resolution.x = std::max(1u, resolution.x);
    resolution.y = std::max(1u, resolution.y);
    tile_size = std::max(1u, tile_size);
    return {
        (resolution.x + tile_size - 1u) / tile_size,
        (resolution.y + tile_size - 1u) / tile_size
    };
}

static bool computeLightTileBounds(
    const Camera& camera,
    const glm::uvec2 resolution,
    const uint32_t tile_size,
    const Light& light,
    glm::uvec2& out_min_tile,
    glm::uvec2& out_max_tile
) {
    const auto tiles = getTileCounts(resolution, tile_size);
    if (tiles.x == 0u || tiles.y == 0u) {
        return false;
    }

    // View-space position (OpenGL conventional: forward is -Z)
    const glm::vec3 view_pos = glm::vec3(camera.getView() * glm::vec4(light.getPosition(), 1.0f));
    const float z = -view_pos.z;
    if (z <= 1e-4f) {
        return false;
    }

    const float r = light.getRadius();
    if (r <= 0.0f) {
        return false;
    }

    // Project center to NDC
    const glm::vec4 clip = camera.getProjection() * glm::vec4(view_pos, 1.0f);
    if (clip.w == 0.0f) {
        return false;
    }
    const glm::vec2 ndc_center = glm::vec2(clip) / clip.w;

    // Approximate NDC radius for sphere using projection diagonal terms.
    // For typical perspective matrix: P[0][0] = f/aspect, P[1][1] = f.
    const glm::mat4& P = camera.getProjection();
    const float ndc_rx = (r * P[0][0]) / z;
    const float ndc_ry = (r * P[1][1]) / z;

    if (!std::isfinite(ndc_rx) || !std::isfinite(ndc_ry)) {
        return false;
    }

    glm::vec2 ndc_min = ndc_center - glm::vec2(ndc_rx, ndc_ry);
    glm::vec2 ndc_max = ndc_center + glm::vec2(ndc_rx, ndc_ry);

    // Clamp to screen NDC
    ndc_min = glm::clamp(ndc_min, glm::vec2(-1.0f), glm::vec2(1.0f));
    ndc_max = glm::clamp(ndc_max, glm::vec2(-1.0f), glm::vec2(1.0f));

    // Convert to pixel coords (origin bottom-left), then to tiles.
    const glm::vec2 resf = glm::vec2(resolution);
    const glm::vec2 px_min_f = (ndc_min * 0.5f + 0.5f) * resf;
    const glm::vec2 px_max_f = (ndc_max * 0.5f + 0.5f) * resf;

    glm::ivec2 px_min = glm::ivec2(glm::floor(px_min_f));
    glm::ivec2 px_max = glm::ivec2(glm::ceil(px_max_f));

    px_min = glm::clamp(px_min, glm::ivec2(0), glm::ivec2(glm::max(glm::uvec2(1u), resolution) - 1u));
    px_max = glm::clamp(px_max, glm::ivec2(0), glm::ivec2(glm::max(glm::uvec2(1u), resolution) - 1u));

    glm::ivec2 tile_min_i = px_min / glm::ivec2(tile_size);
    glm::ivec2 tile_max_i = px_max / glm::ivec2(tile_size);

    tile_min_i = glm::clamp(tile_min_i, glm::ivec2(0), glm::ivec2(tiles) - 1);
    tile_max_i = glm::clamp(tile_max_i, glm::ivec2(0), glm::ivec2(tiles) - 1);

    out_min_tile = glm::uvec2(tile_min_i);
    out_max_tile = glm::uvec2(tile_max_i);
    return true;
}

void Lighting::update(const Camera& camera) {
    const auto frustum = Frustum::fromCamera(camera);
    punctual_lights.update(frustum);

    // Update scene lighting buffer if global lighting changed, directional changed,
    // or visible punctual light count changed due to culling/camera movement.
    const auto current_visible = static_cast<uint32_t>(punctual_lights.visibleSize());

    if (isChanged() || directional_light.isChanged() || last_visible_count != current_visible) {
        updateSceneLightBuffer();
        resetChanged();
        directional_light.resetChanged();
        last_visible_count = current_visible;
    }

    // CPU tiled light culling buffers (for deferred shading)
    const glm::uvec2 resolution = context.getSize();
    const uint32_t tile_size = getTiledLightTileSize();
    const glm::uvec2 tile_counts = getTileCounts(resolution, tile_size);
    const uint32_t tile_count = tile_counts.x * tile_counts.y;

    std::vector<uint32_t> per_tile_counts(tile_count, 0u);

    // Build in the same order as uploaded to LIGHTS_BUFFER: LightContainer::getVisibleIds()
    const auto& lights = punctual_lights.getLights();
    const auto& visible_ids = punctual_lights.getVisibleIds();

    glm::uvec2 min_tile, max_tile;
    for (uint32_t visible_index = 0; visible_index < visible_ids.size(); ++visible_index) {
        const uint64_t id = visible_ids[visible_index];
        auto it = lights.find(id);
        if (it == lights.end()) {
            continue;
        }
        const Light& light = it->second;

        if (!computeLightTileBounds(camera, resolution, tile_size, light, min_tile, max_tile)) {
            continue;
        }

        for (uint32_t ty = min_tile.y; ty <= max_tile.y; ++ty) {
            for (uint32_t tx = min_tile.x; tx <= max_tile.x; ++tx) {
                const uint32_t tile_index = tx + ty * tile_counts.x;
                ++per_tile_counts[tile_index];
            }
        }
    }

    // Prefix sum -> offsets
    std::vector<glm::uvec2> tile_grid(tile_count, glm::uvec2(0u));
    uint32_t running = 0;
    for (uint32_t i = 0; i < tile_count; ++i) {
        tile_grid[i].x = running;
        tile_grid[i].y = per_tile_counts[i];
        running += per_tile_counts[i];
    }

    std::vector<uint32_t> tile_indices;
    tile_indices.resize(std::max(1u, running));

    // Second pass fill
    std::vector<uint32_t> write_cursor(tile_count, 0u);
    for (uint32_t i = 0; i < tile_count; ++i) {
        write_cursor[i] = tile_grid[i].x;
    }

    for (uint32_t visible_index = 0; visible_index < visible_ids.size(); ++visible_index) {
        const uint64_t id = visible_ids[visible_index];
        auto it = lights.find(id);
        if (it == lights.end()) {
            continue;
        }
        const Light& light = it->second;

        if (!computeLightTileBounds(camera, resolution, tile_size, light, min_tile, max_tile)) {
            continue;
        }

        for (uint32_t ty = min_tile.y; ty <= max_tile.y; ++ty) {
            for (uint32_t tx = min_tile.x; tx <= max_tile.x; ++tx) {
                const uint32_t tile_index = tx + ty * tile_counts.x;
                const uint32_t dst = write_cursor[tile_index]++;
                tile_indices[dst] = visible_index;
            }
        }
    }

    // Resize and upload SSBOs
    if (last_tile_resolution != resolution || last_tile_size != tile_size || tile_grid_buffer->getSize() < tile_grid.size() * sizeof(glm::uvec2)) {
        tile_grid_buffer->resize(std::max(sizeof(glm::uvec2), tile_grid.size() * sizeof(glm::uvec2)));
        last_tile_resolution = resolution;
        last_tile_size = tile_size;
    }
    tile_grid_buffer->mapData(tile_grid.data(), tile_grid.size() * sizeof(glm::uvec2));

    if (tile_indices_buffer->getSize() < tile_indices.size() * sizeof(uint32_t)) {
        tile_indices_buffer->resize(tile_indices.size() * sizeof(uint32_t));
    }
    tile_indices_buffer->mapData(tile_indices.data(), tile_indices.size() * sizeof(uint32_t));

    // Bind tiled buffers explicitly (also bound by ShaderProgram::bindIndexedBuffers when used).
    tile_grid_buffer->bindBase(context.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::ShaderStorage, TILE_LIGHT_GRID_BUFFER_NAME));
    tile_indices_buffer->bindBase(context.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::ShaderStorage, TILE_LIGHT_INDICES_BUFFER_NAME));

    buffer->bindBase(context.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::ShaderStorage, SCENE_LIGHTING_BUFFER_NAME));
}
