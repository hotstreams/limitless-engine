#include <chrono>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>

#include <limitless/assets.hpp>
#include <limitless/camera.hpp>
#include <limitless/core/context.hpp>
#include <limitless/core/context_initializer.hpp>
#include <limitless/core/vertex.hpp>
#include <limitless/instances/instance.hpp>
#include <limitless/instances/skeletal_instance.hpp>
#include <limitless/loaders/gltf_model_loader.hpp>
#include <limitless/models/model.hpp>
#include <limitless/models/skeletal_model.hpp>
#include <limitless/renderer/renderer.hpp>
#include <limitless/scene.hpp>

using namespace Limitless;
using namespace std::chrono;

class CameraMouseHandler {
public:
	explicit CameraMouseHandler(Camera& cam)
		: camera {cam} {}

	void onMouseMove(glm::dvec2 mouse_pos) {
		auto offset = glm::vec2 {
			mouse_pos.x - last_mouse_pos.x,
			last_mouse_pos.y - mouse_pos.y
		};
		last_mouse_pos = mouse_pos;
		camera.setRotation(
			camera.getPitch() + float(offset.y),
			camera.getYaw() + float(offset.x)
		);
	}

private:
	Camera& camera;
	glm::dvec2 last_mouse_pos {0, 0};
};

static void frameCameraToBox(Camera& camera, const Box& box) {
	const glm::vec3 center = box.center;
	const float radius = std::max(0.25f, 0.5f * glm::length(box.size));
	const glm::vec3 cam_pos = center + glm::vec3(0.0f, radius * 0.25f, radius * 2.5f);
	const glm::vec3 dir = glm::normalize(center - cam_pos);
	const float pitch = glm::degrees(std::asin(std::clamp(dir.y, -1.0f, 1.0f)));
	const float yaw = glm::degrees(std::atan2(dir.z, dir.x));
	camera.setPosition(cam_pos);
	camera.setRotation(pitch, yaw);

	std::cerr << "[viewer] bbox center=(" << center.x << "," << center.y << "," << center.z << ")"
			  << " size=(" << box.size.x << "," << box.size.y << "," << box.size.z << ")"
			  << " radius=" << radius << "\n";
	std::cerr << "[viewer] camera pos=(" << cam_pos.x << "," << cam_pos.y << "," << cam_pos.z << ")"
			  << " pitch=" << pitch << " yaw=" << yaw << "\n";
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cerr << "usage: gltf_viewer <model>\n";
		return 2;
	}
	const std::string model_path = argv[1];

	glm::uvec2 window_size {1080, 720};
	Camera camera {window_size};
	CameraMouseHandler mouse_handler {camera};
	Context ctx = Context::builder()
		.title("GLTF viewer")
		.size(window_size)
		.not_resizeable()
		.cursor(CursorMode::Normal)
		.swap_interval(1)
		.sticky_keys()
		.on_mouse_move([&](glm::dvec2 pos) { mouse_handler.onMouseMove(pos); })
		.build();

	ctx.clearColor(glm::vec4{0.0f, 0.0f, 0.1f, 1.f});

	Assets assets {ENGINE_ASSETS_DIR};
	assets.load(ctx);

	const ModelLoaderFlags load_flags;
	auto model = GltfModelLoader::builder().path(fs::path(model_path)).flags(load_flags).build().load(assets);

	auto renderer = Renderer::builder()
		.resolution(window_size)
		.deferred()
		.build();

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
		const auto anims = skeletal_model->getAnimations();
		if (!anims.empty()) {
			inst->play(anims[0].name);
		}
	} else if (auto* plain_model = dynamic_cast<Model*>(model.get()); plain_model) {
		std::cerr << "loaded plain model" << std::endl;
		scene.add(Instance::builder().model(model).build());
	} else {
		throw std::runtime_error("unknown model type");
	}

	frameCameraToBox(camera, model->getBoundingBox());

	if (!ContextInitializer::checkMinimumRequirements()) {
		std::cerr << "Minimum graphics card requirements are not met!" << std::endl;
		return 3;
	}

	ctx.setCursorMode(CursorMode::Normal);
	ctx.setSwapInterval(1);
	ctx.setStickyKeys(true);

	assets.recompileAssets(ctx, renderer->getSettings());

	while (!ctx.shouldClose()) {
		auto current_time = steady_clock::now();
		static auto last_time = steady_clock::now();
		auto delta = duration_cast<duration<float>>(current_time - last_time).count();
		last_time = current_time;

		scene.update(camera);
		renderer->render(ctx, assets, scene, camera);

		ctx.setDepthFunc(DepthFunc::Less);
		ctx.setDepthMask(DepthMask::True);
		ctx.enable(Capabilities::DepthTest);

		ctx.swapBuffers();
		ctx.pollEvents();

		if (ctx.isPressed(GLFW_KEY_W)) {
			camera.setPosition(camera.getPosition() + camera.getFront() * delta);
		}
		if (ctx.isPressed(GLFW_KEY_S)) {
			camera.setPosition(camera.getPosition() - camera.getFront() * delta);
		}
		if (ctx.isPressed(GLFW_KEY_A)) {
			camera.setPosition(camera.getPosition() - camera.getRight() * delta);
		}
		if (ctx.isPressed(GLFW_KEY_D)) {
			camera.setPosition(camera.getPosition() + camera.getRight() * delta);
		}
		if (ctx.isPressed(GLFW_KEY_SPACE)) {
			camera.setPosition(camera.getPosition() + camera.getUp() * delta);
		}
		if (ctx.isPressed(GLFW_KEY_Z)) {
			camera.setPosition(camera.getPosition() - camera.getUp() * delta);
		}
		if (ctx.isPressed(GLFW_KEY_Q)) {
			break;
		}
	}
	return 0;
}
