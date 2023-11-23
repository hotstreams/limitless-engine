#include <chrono>
#include <cstdio>
#include <limitless/assets.hpp>
#include <limitless/camera.hpp>
#include <limitless/core/context.hpp>
#include <limitless/core/context_observer.hpp>
#include <limitless/core/indexed_stream.hpp>
#include <limitless/core/skeletal_stream.hpp>
#include <limitless/core/vertex.hpp>
#include <limitless/instances/model_instance.hpp>
#include <limitless/instances/skeletal_instance.hpp>
#include <limitless/models/abstract_mesh.hpp>
#include <limitless/models/abstract_model.hpp>
#include <limitless/models/bones.hpp>
#include <limitless/models/line.hpp>
#include <limitless/models/mesh.hpp>
#include <limitless/models/model.hpp>
#include <limitless/models/skeletal_model.hpp>
#include <limitless/loaders/gltf_model_loader.hpp>
#include <limitless/ms/material_builder.hpp>
#include <limitless/ms/property.hpp>
#include <limitless/pipeline/shader_type.hpp>
#include <limitless/renderer/renderer.hpp>
#include <limitless/scene.hpp>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

using namespace Limitless;
using namespace std::chrono;

class Pro100ProMouseHandler : public MouseMoveObserver {
public:
	Pro100ProMouseHandler(Camera& _camrip)
		: camrip {_camrip} {}

	void onMouseMove(glm::dvec2 pos) override {
		auto offset = glm::vec2 {pos.x - last_move.x, last_move.y - pos.y};
		last_move   = pos;

		camrip.mouseMove(offset);
	}

private:
	Camera& camrip;
	glm::dvec2 last_move {0, 0};
};

int main(int argc, char* argv[]) {
	try {
		if (argc != 3) {
			std::cerr << "usage: cgltf [model] [animation]" << std::endl;
			return 2;
		}

		glm::uvec2 window_size {1080, 720};
		ContextEventObserver ctx {
			"CGLTF demo", window_size, {{WindowHint::Resizable, false}}};
		ctx.clearColor(glm::vec4{0.9f, 0.9f, 1.f, 1.f});

		Assets assets {ENGINE_ASSETS_DIR};
		assets.load(ctx);
		auto model = GltfModelLoader::loadModel(assets, fs::path(argv[1]), {});

		Camera camera {window_size};
		Pro100ProMouseHandler mousemouse {camera};
		Renderer renderer {ctx};

		Scene scene {ctx};
		scene.lighting.ambient_color.a   = 1.0f;
		scene.lighting.directional_light = {
			glm::vec4(1.0, -1.0, 1.5, 1.0f),
			glm::vec4 {1.0f, 1.0f, 1.0f, 1.0f}
		};
		scene.add<ModelInstance>(model, glm::vec3(0.f));

		camera.setPosition({0.0f, 0.0f, 0.0f});
		camera.setFront({1.f, 0.f, 0.f});

		if (!Limitless::ContextInitializer::checkMinimumRequirements()) {
			throw("Minimum requirements are not met!");
		}

		ctx.setCursorMode(CursorMode::Normal);
		ctx.setSwapInterval(1);
		ctx.setStickyKeys(true);
		ctx.registerObserver(&mousemouse);

		assets.recompileAssets(ctx, renderer.getSettings());

		while (!ctx.shouldClose()) {
			auto current_time     = steady_clock::now();
			static auto last_time = steady_clock::now();
			auto delta =
				duration_cast<duration<float>>(current_time - last_time)
					.count();
			last_time = current_time;

			renderer.draw(ctx, assets, scene, camera);

			ctx.setDepthFunc(DepthFunc::Less);
			ctx.setDepthMask(DepthMask::False);
			

			ctx.setLineWidth(2.5f);

			static const auto x = std::make_shared<Line>(
				glm::vec3 {0.0f, 0.0f, 0.0f}, glm::vec3 {1.0f, 0.0f, 0.0f}
			);
			static const auto y = std::make_shared<Line>(
				glm::vec3 {0.0f, 0.0f, 0.0f}, glm::vec3 {0.0f, 1.0f, 0.0f}
			);
			static const auto z = std::make_shared<Line>(
				glm::vec3 {0.0f, 0.0f, 0.0f}, glm::vec3 {0.0f, 0.0f, 1.0f}
			);

			static ModelInstance x_i {
				x, assets.materials.at("green"), {5.0f, 1.0f, 0.0f}
            };
			static ModelInstance y_i {
				y, assets.materials.at("blue"), {5.0f, 1.0f, 0.0f}
            };
			static ModelInstance z_i {
				z, assets.materials.at("red"), {5.0f, 1.0f, 0.0f}
            };

            ctx.disable(Capabilities::DepthTest);
			x_i.draw(ctx, assets, ShaderType::Forward, ms::Blending::Opaque);
			y_i.draw(ctx, assets, ShaderType::Forward, ms::Blending::Opaque);
			z_i.draw(ctx, assets, ShaderType::Forward, ms::Blending::Opaque);
			ctx.enable(Capabilities::DepthTest);

			ctx.swapBuffers();
			ctx.pollEvents();

			if (ctx.isPressed(GLFW_KEY_W)) {
				camera.movement(CameraMovement::Forward, delta);
			}

			if (ctx.isPressed(GLFW_KEY_S)) {
				camera.movement(CameraMovement::Backward, delta);
			}

			if (ctx.isPressed(GLFW_KEY_A)) {
				camera.movement(CameraMovement::Left, delta);
			}

			if (ctx.isPressed(GLFW_KEY_D)) {
				camera.movement(CameraMovement::Right, delta);
			}

			if (ctx.isPressed(GLFW_KEY_SPACE)) {
				camera.movement(CameraMovement::Up, delta);
			}

			if (ctx.isPressed(GLFW_KEY_Z)) {
				camera.movement(CameraMovement::Down, delta);
			}

			if (ctx.isPressed(GLFW_KEY_Q)) {
				break;
			}
		}

	} catch (const std::string& err) {
		std::cerr << "usage: cgltf [model] [animation]" << std::endl;
		fprintf(stderr, "%s\n", err.c_str());
		return 1;
	} catch (const char* err) {
		fprintf(stderr, "%s\n", err);
		return 1;
	}
	return 0;
}
