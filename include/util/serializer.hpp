#pragma once

#include <material_system/material.hpp>

namespace GraphicsEngine {
	template <typename W>
	class MaterialSerializer {
	private:
		W writer;
	public:
		template <typename Args...>
		MaterialSerializer(Args&&... args) : writer{std::forward<Args>(args)...} {}

		void serialize(const Material& mat) {
			for (const auto& [type, uniform] : mat.getProperties()) {
				writer << type;
				UniformSerializer<W>::serialize(uniform, writer);
			}
		}
	};
}
