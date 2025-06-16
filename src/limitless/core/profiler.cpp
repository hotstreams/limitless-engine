#include <limitless/core/profiler.hpp>

#include <limitless/assets.hpp>

using namespace Limitless;

void Profiler::draw(Context& ctx, const Assets& assets) {
    const auto text_format = TextFormat(
        /* color = */glm::vec4(1.f),
        {assets.fonts.at("nunito")},
        /* wrap_width =*/ std::nullopt,
        /* cjk_variant = */std::nullopt,
        /* line_spacing_modifier = */1.f,
        /* pixel_size = */std::nullopt,
        /* link_id = */std::nullopt
    );
    auto text = TextInstance({{"text", text_format}}, glm::vec2(0.f));
    text.setSize(glm::vec2{0.5f});
    glm::vec2 position = {400, 400};
    for (const auto& [name, query] : queries) {
        text.setText(name + " " + std::to_string(query.getDuration()), text_format);
        text.setPosition(position);
        text.draw(ctx, assets);

        position -= glm::vec2{0.0f, 0.1f};
    }
}

TimeQuery& Profiler::operator[] (const std::string& name) {
    return queries[name];
}

ProfilerScope::ProfilerScope(const std::string& name)
    : query {profiler[name]} {
    query.start();
}

ProfilerScope::~ProfilerScope() {
    query.stop();
    query.calculate();
}
