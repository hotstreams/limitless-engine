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

class CameraMouseHandler : public MouseMoveObserver {
public:
	CameraMouseHandler(Camera& _camera)
		: camera {_camera} {}

	void onMouseMove(glm::dvec2 mouse_pos) override {
		auto offset = glm::vec2 {
			mouse_pos.x - last_mouse_pos.x,
			last_mouse_pos.y - mouse_pos.y
		};
		last_mouse_pos   = mouse_pos;

		camera.mouseMove(offset);
	}

private:
	Camera& camera;
	glm::dvec2 last_mouse_pos {0, 0};
};

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "usage: gltf_viewer [model]" << std::endl;
		return 2;
	}

	glm::uvec2 window_size {1080, 720};
	ContextEventObserver ctx {
		"GLTF viewer", window_size, {{WindowHint::Resizable, false}}};
	ctx.clearColor(glm::vec4{0.0f, 0.0f, 0.1f, 1.f});

	Assets assets {ENGINE_ASSETS_DIR};
	assets.load(ctx);
	auto model = GltfModelLoader::loadModel(assets, fs::path(argv[1]), {});

	Camera camera {window_size};
	CameraMouseHandler mousemouse {camera};
	Renderer renderer {ctx};

	Scene scene {ctx};

    scene.add(Light::builder()
        .color({1.0, -1.0, 1.5, 1.0f})
        .direction({0.7f, -1.0f, 0.0f})
        .build()
    );

	if (auto* skeletal_model = dynamic_cast<SkeletalModel*>(model.get()); skeletal_model) {
		std::cerr << "loaded skeletal model" << std::endl;
		auto inst = Instance::builder().model(model).asSkeletal();
        scene.add(inst);

		auto anims = skeletal_model->getAnimations();
		if (!anims.empty()) {
			inst->play(anims[0].name);
		}
	} else if (auto* plain_model = dynamic_cast<Model*>(model.get()); plain_model) {
		std::cerr << "loaded plain model" << std::endl;
        scene.add(Instance::builder().model(model).build());
	} else {
		throw std::runtime_error("unknown model type");
	}

	camera.setPosition({0.0f, 0.0f, 0.0f});
	camera.setFront({1.f, 0.f, 0.f});

	if (!Limitless::ContextInitializer::checkMinimumRequirements()) {
		std::cerr << "Minimum graphics card requirements are not met!" << std::endl;
		return 3;
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
		ctx.setDepthMask(DepthMask::True);

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
	return 0;
}
