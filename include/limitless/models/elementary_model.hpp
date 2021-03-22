#pragma once

#include <limitless/models/model.hpp>

namespace LimitlessEngine {
    class ElementaryModel : public AbstractModel {
    protected:
        ElementaryModel() = default;
    public:
        ~ElementaryModel() override = default;

        ElementaryModel(const ElementaryModel&) noexcept = delete;
        ElementaryModel& operator=(const ElementaryModel&) noexcept = delete;

        ElementaryModel(ElementaryModel&&) noexcept = default;
        ElementaryModel& operator=(ElementaryModel&&) noexcept = default;

        [[nodiscard]] const auto& getMesh() const { return meshes.at(0); }
    };

    class Line : public ElementaryModel {
    public:
        Line(const glm::vec3& a, const glm::vec3& b);
        ~Line() override = default;

        Line(const Line&) noexcept = delete;
        Line& operator=(const Line&) noexcept = delete;

        Line(Line&&) noexcept = default;
        Line& operator=(Line&&) noexcept = default;
    };

    // uses NDC space
    class Quad : public ElementaryModel {
    public:
        Quad();
        ~Quad() override = default;

        Quad(const Quad&) noexcept = delete;
        Quad& operator=(const Quad&) noexcept = delete;

        Quad(Quad&&) noexcept = default;
        Quad& operator=(Quad&&) noexcept = default;
    };

    // uses local space
	class Plane : public ElementaryModel {
	public:
		Plane();
		~Plane() override = default;

		Plane(const Plane&) noexcept = delete;
		Plane& operator=(const Plane&) noexcept = delete;

		Plane(Plane&&) noexcept = default;
		Plane& operator=(Plane&&) noexcept = default;
	};

    class Cube : public ElementaryModel {
    public:
        Cube();
        ~Cube() override = default;

        Cube(const Cube&) noexcept = delete;
        Cube& operator=(const Cube&) noexcept = delete;

        Cube(Cube&&) noexcept = default;
        Cube& operator=(Cube&&) noexcept = default;
    };

    class Sphere : public ElementaryModel {
    public:
        Sphere(uint32_t x_segments, uint32_t y_segments);
        ~Sphere() override = default;

        Sphere(const Sphere &) = delete;
        Sphere &operator=(const Sphere &) = delete;

        Sphere(Sphere&&) noexcept = default;
        Sphere& operator=(Sphere&&) noexcept = default;
    };
}