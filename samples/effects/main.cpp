#include "scene.hpp"
#include "assets.hpp"

#include <limitless/core/context_observer.hpp>
#include <limitless/text/text_instance.hpp>
#include <limitless/util/color_picker.hpp>
#include <limitless/core/state_query.hpp>
#include <limitless/core/texture/state_texture.hpp>
#include <limitless/renderer/renderer.hpp>

namespace LimitlessMaterials {
    class MaterialsScene
        : public Limitless::MouseMoveObserver,
          public Limitless::KeyObserver,
          public Limitless::FramebufferObserver {
    private:
        glm::uvec2 window_size {1080, 720};

        Limitless::ContextEventObserver context;
        Limitless::Camera camera;
        Limitless::Renderer render;
        Assets assets;
        Scene scene;

        bool done{};
        bool hidden_text{};
    public:
        MaterialsScene()
            : context {"Limitless-demo", window_size, {{Limitless::WindowHint::Resizable, false}}}
            , camera {window_size}
            , render {context}
            , assets {context, ENGINE_ASSETS_DIR}
            , scene {context, assets} {
            camera.setPosition({-3.0f, 2.0f, 3.0f});

            if (!Limitless::ContextInitializer::checkMinimumRequirements()) {
                throw std::runtime_error("Minimum requirements are not met!");
            }

            context.setCursorMode(Limitless::CursorMode::Normal);
            context.setSwapInterval(1);
            context.setStickyKeys(true);

            context.registerObserver(static_cast<KeyObserver *>(this));
            context.registerObserver(static_cast<MouseMoveObserver *>(this));
            context.registerObserver(static_cast<FramebufferObserver *>(this));

            assets.recompileAssets(context, render.getSettings());
        }

        ~MaterialsScene() override {
            context.unregisterObserver(static_cast<KeyObserver *>(this));
            context.unregisterObserver(static_cast<MouseMoveObserver *>(this));
            context.unregisterObserver(static_cast<FramebufferObserver *>(this));
        }

        void onMouseMove(glm::dvec2 pos) override {
            static glm::dvec2 last_move = {0, 0};

            auto offset = glm::vec2{pos.x - last_move.x, last_move.y - pos.y};
            last_move = pos;

            camera.mouseMove(offset);
        }

        void onKey(int key, [[maybe_unused]] int scancode, Limitless::InputState state, [[maybe_unused]] Limitless::Modifier modifier) override {
            using namespace Limitless;
            if (key == GLFW_KEY_ESCAPE && state == Limitless::InputState::Pressed) {
                done = true;
            }

            if (key == GLFW_KEY_SPACE && state == Limitless::InputState::Pressed) {
                camera.getMoveSpeed() *= 5.0f;
            }

            if (key == GLFW_KEY_SPACE && state == Limitless::InputState::Released) {
                camera.getMoveSpeed() /= 5.0f;
            }

            if (key == GLFW_KEY_GRAVE_ACCENT && state == Limitless::InputState::Released) {
                hidden_text = !hidden_text;
            }
        }

        void onFramebufferChange(glm::uvec2 size) override {
            camera.updateProjection(size);
            window_size = size;

        }

        void handleInput(float delta) noexcept {
            using namespace Limitless;

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

        void gameLoop() {
            using namespace std::chrono;
            while (!context.shouldClose() && !done) {
                auto current_time = steady_clock::now();
                static auto last_time = steady_clock::now();
                auto delta_time = duration_cast<duration<float>>(current_time - last_time).count();
                last_time = current_time;

                scene.update(context, camera);
                render.draw(context, assets, scene.getScene(), camera);

                context.swapBuffers();
                context.pollEvents();

                handleInput(delta_time);
            }
        }
    };
}

int main() {
    LimitlessMaterials::MaterialsScene scene;
    scene.gameLoop();
    return 0;
}
