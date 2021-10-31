#pragma once

#include <stack>

namespace Limitless {
	class MatrixStack {
	private:
		std::stack<glm::mat4> stack;
	public:
		MatrixStack() {

		}
	};
}