#include "assets.hpp"
#include "scene.hpp"

#include <limitless/core/context_observer.hpp>
#include <limitless/text/text_instance.hpp>
#include <limitless/loaders/dds_loader.hpp>
#include <limitless/util/color_picker.hpp>
#include <limitless/pipeline/deferred/deferred_framebuffer_pass.hpp>
#include <iostream>
#include <limitless/core/state_query.hpp>
#include <limitless/core/texture/state_texture.hpp>
#include <limitless/util/stack_trace.hpp>

using namespace Limitless;

std::ostream& operator<<(std::ostream& stream, const StackFrame& stackframe) {
    return stream << stackframe.module << ": " << stackframe.func << std::endl;
}

std::ostream& operator<<(std::ostream& stream, const StackTrace& stacktrace) {
    for (const auto& frame : stacktrace) {
        stream << frame;
    }
    return stream;
}

static void terminationHandler() {
    std::cerr << "The application has to be terminated due to a fatal error";
    bool printed_stack_trace = false;

    auto exception = std::current_exception();
    if (exception) {
        try {
            std::rethrow_exception(exception);
        }
        catch (const std::runtime_error& ex) {
            std::cerr << ": " << ex.what() << std::endl;
//            std::cerr << ex.stack_trace;
            printed_stack_trace = true;
        }
        catch (const std::exception& ex) {
            std::cerr << ": " << ex.what() << std::endl;
        }
        catch (...) {
            std::cerr << "." << std::endl;
        }
    } else {
        std::cerr << "." << std::endl;
    }

    if (!printed_stack_trace) {
        std::cerr << getStackTrace(1) << std::endl;
    }
    std::abort();
}

class Game : public MouseMoveObserver, public KeyObserver, public FramebufferObserver {
private:
    glm::uvec2 window_size {1080, 720};

    ContextEventObserver context;
    Camera camera;
    Renderer render;
    DemoAssets assets;
    LimitlessDemo::DemoScene scene;

    bool done {};
    bool hidden_text {};
public:
    Game()
        : context {"Limitless-demo", window_size, {{ WindowHint::Resizable, false }}}
        , camera {window_size}
        , render {context}
        , assets {context, render, ENGINE_ASSETS_DIR}
        , scene {context, assets}
    {
        camera.setPosition({-3.0f, 2.0f, 3.0f});

        if (!Limitless::ContextInitializer::checkMinimumRequirements()) {
            throw std::runtime_error("Minimum requirements are not met!");
        }

        context.setCursorMode(CursorMode::Normal);
        context.setSwapInterval(1);
        context.setStickyKeys(true);

        context.registerObserver(static_cast<KeyObserver*>(this));
        context.registerObserver(static_cast<MouseMoveObserver*>(this));
        context.registerObserver(static_cast<FramebufferObserver*>(this));

        assets.recompileAssets(context, render.getSettings());
    }

    ~Game() override {
        context.unregisterObserver(static_cast<KeyObserver*>(this));
        context.unregisterObserver(static_cast<MouseMoveObserver*>(this));
        context.unregisterObserver(static_cast<FramebufferObserver*>(this));
    }

    void onMouseMove(glm::dvec2 pos) override {
        static glm::dvec2 last_move = { 0, 0 };

        auto offset = glm::vec2{ pos.x - last_move.x, last_move.y - pos.y };
        last_move = pos;

        camera.mouseMove(offset);
    }

    void onKey(int key, [[maybe_unused]] int scancode, InputState state, [[maybe_unused]] Modifier modifier) override {
        if (key == GLFW_KEY_ESCAPE && state == InputState::Pressed) {
            done = true;
        }

        if (key == GLFW_KEY_TAB && state == InputState::Pressed) {
            scene.next(camera);
        }

        if (key == GLFW_KEY_SPACE && state == InputState::Pressed) {
            camera.getMoveSpeed() *= 5.0f;
        }

        if (key == GLFW_KEY_SPACE && state == InputState::Released) {
            camera.getMoveSpeed() /= 5.0f;
        }

        if (key == GLFW_KEY_GRAVE_ACCENT && state == InputState::Released) {
            hidden_text = !hidden_text;
        }
    }

    void onFramebufferChange(glm::uvec2 size) override {
        camera.updateProjection(size);
        window_size = size;

    }

    void handleInput(float delta) noexcept {
        if (context.isPressed(GLFW_KEY_W)) {
            camera.movement(CameraMovement::Forward, delta);
        }

        if (context.isPressed(GLFW_KEY_S)) {
            camera.movement(CameraMovement::Backward, delta);
        }

        if (context.isPressed(GLFW_KEY_A)) {
            camera.movement(CameraMovement::Left, delta);
        }

        if (context.isPressed(GLFW_KEY_D)) {
            camera.movement(CameraMovement::Right, delta);
        }
    }

    void drawText() {
        if (hidden_text) {
            return;
        }

        auto start = glm::vec2 {0.0f, window_size.y - 20};
        for (auto it = scene.getScenes().begin(); it != scene.getScenes().end(); ++it) {
            TextInstance list {it->first, start, assets.fonts.at("nunito")};
            list.setColor((it == scene.getCurrent()) ? glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) : glm::vec4(1.0f));
            list.setSize(glm::vec2{0.3f});
            list.draw(context, assets);
            start.y -= 20.0f;
        }

        TextInstance helper {"Use 'WASD' & 'mouse' to look around.\n"
                             "Press 'TAB' to switch scene.\n"
                             "Hold 'SPACE' to boost camera speed.\n"
                             "Press '~' to hide this text.\n"
                             "Press 'ESC' to quit.", start, assets.fonts.at("nunito")};
        helper.setSize(glm::vec2{0.3f});
        helper.draw(context, assets);
    }

    void gameLoop() {
        using namespace std::chrono;
        while (!context.shouldClose() && !done) {
            auto current_time = steady_clock::now();
            static auto last_time = steady_clock::now();
            auto delta_time = duration_cast<duration<float>>(current_time - last_time).count();
            last_time = current_time;

            render.draw(context, assets, scene.getCurrentScene(), camera);
            drawText();

            context.swapBuffers();
            context.pollEvents();

            handleInput(delta_time);
        }
    }
};

int main() {
//    std::set_terminate(terminationHandler);
    Game game;
    game.gameLoop();
    return 0;
}
