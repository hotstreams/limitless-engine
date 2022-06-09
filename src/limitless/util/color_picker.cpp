#include <limitless/util/color_picker.hpp>

using namespace Limitless;

void ColorPicker::onPick(Context& ctx, Assets& assets, Scene& scene, glm::uvec2 coords, std::function<void(uint32_t id)> callback) {
    auto& pick = data.emplace_back(PickData{std::move(callback), coords});

    framebuffer.bind();
    ctx.clear(Clear::Color);
    ctx.enable(Capabilities::DepthTest);
    ctx.setDepthFunc(DepthFunc::Equal);
    ctx.setDepthMask(DepthMask::False);

    for (auto& [id, instance] : scene) {
        const auto setter = UniformSetter([color_id = convert(id)](ShaderProgram& shader) {
            shader << UniformValue<glm::vec3>("color_id", color_id);
        });
        instance.get()->draw(ctx, assets, ShaderPass::ColorPicker, ms::Blending::Opaque, setter);
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

        info.callback(convert(color));

        framebuffer.unbind();

        data.pop_front();
    }
}
