#include <limitless/util/color_picker.hpp>

#include <limitless/pipeline/shader_type.hpp>
#include <utility>
#include <iostream>
#include <limitless/core/uniform/uniform_sampler.hpp>
using namespace Limitless;

void ColorPicker::onPick(Context& ctx, Assets& assets, Scene& scene, glm::uvec2 coords, std::function<void(uint32_t id)> callback) {
    onPick(ctx, assets, scene.getInstances(), coords, std::move(callback));
}

void ColorPicker::onPick(Context& ctx, Assets& assets, const Instances& instances, glm::uvec2 coords, std::function<void(uint32_t)> callback) {
    coords.y = ctx.getSize().y - coords.y;
    auto& pick = data.emplace_back(PickData{std::move(callback), coords});

    framebuffer.bind();

    ctx.enable(Capabilities::DepthTest);
    ctx.setDepthFunc(DepthFunc::Less);
    ctx.setDepthMask(DepthMask::True);

    ctx.clearColor(glm::vec4{0.0f, 0.0f, 0.f, 1.f});

    framebuffer.clear();

    for (auto& wrapper : instances) {
        auto& instance = wrapper.get();
        const auto id = instance.getId();
        const auto setter = UniformSetter([color_id = convert(id)](ShaderProgram& shader) {
            shader.setUniform("color_id", color_id);
        });
        instance.draw(ctx, assets, ShaderType::ColorPicker, ms::Blending::Opaque, setter);
    }

    pick.sync.place();

    framebuffer.unbind();
}

void ColorPicker::process(Context& ctx) {
    if (!data.empty() && data.front().sync.isDone()) {
        const auto& info = data.front();

        framebuffer.bind();

        ctx.setPixelStore(PixelStore::UnpackAlignment, 1);

        glm::uvec3 color {};
        glReadPixels(info.coords.x, info.coords.y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, &color[0]);

        framebuffer.unbind();

        info.callback(convert(color));

        data.pop_front();
    }
}

void ColorPicker::onFramebufferChange(glm::uvec2 size) {
    framebuffer.onFramebufferChange(size);
}
