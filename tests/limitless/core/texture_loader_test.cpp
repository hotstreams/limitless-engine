#include "../catch_amalgamated.hpp"
#include "../opengl_state.hpp"

#include <limitless/assets.hpp>
#include <limitless/core/context.hpp>
#include <limitless/loaders/texture_loader.hpp>

using namespace Limitless;
using namespace LimitlessTest;

TEST_CASE("TextureLoader extractChannel copies G and B from an RGBA texture") {
	Context context = {"Title", {1, 1}, nullptr, {{WindowHint::Hint::Visible, false}}};
	Assets assets {"../assets"};

	// 4x1 keeps GL pack/unpack alignment happy for both RGBA and R8.
	const uint8_t rgba[] = {
		10, 20, 30, 40,
		50, 60, 70, 80,
		90, 100, 110, 120,
		130, 140, 150, 160
	};
	const auto flags = TextureLoaderFlags {}
		.withLinearSpace()
		.withNoCompression()
		.withNoMipmaps();

	auto source = TextureLoader::loadRaw(assets, "extract_src", rgba, {4, 1}, 4, flags);
	auto green = TextureLoader::extractChannel(assets, *source, "extract_g", 1, flags);
	auto blue = TextureLoader::extractChannel(assets, *source, "extract_b", 2, flags);

	REQUIRE(green->getSize().x == 4);
	REQUIRE(green->getSize().y == 1);
	REQUIRE(green->getFormat() == Texture::Format::Red);

	const auto g_pixels = green->getPixels();
	const auto b_pixels = blue->getPixels();
	REQUIRE(g_pixels.size() == 4);
	REQUIRE(b_pixels.size() == 4);
	REQUIRE(static_cast<unsigned>(g_pixels[0]) == 20);
	REQUIRE(static_cast<unsigned>(g_pixels[1]) == 60);
	REQUIRE(static_cast<unsigned>(g_pixels[2]) == 100);
	REQUIRE(static_cast<unsigned>(g_pixels[3]) == 140);
	REQUIRE(static_cast<unsigned>(b_pixels[0]) == 30);
	REQUIRE(static_cast<unsigned>(b_pixels[1]) == 70);
	REQUIRE(static_cast<unsigned>(b_pixels[2]) == 110);
	REQUIRE(static_cast<unsigned>(b_pixels[3]) == 150);

	REQUIRE_THROWS_AS(
		TextureLoader::extractChannel(assets, *source, "extract_bad", 4, flags),
		texture_loader_exception
	);

	check_opengl_state();
}
