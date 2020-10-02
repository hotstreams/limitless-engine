#include <material.hpp>

#include <stdexcept>

namespace GraphicsEngine {
    class CustomMaterial : public Material {
    private:
        std::unordered_map<std::string, std::unique_ptr<Uniform>> uniforms;

        void update() const noexcept override;

        friend class CustomMaterialBuilder;
        friend void swap(CustomMaterial& lhs, CustomMaterial& rhs) noexcept;
        CustomMaterial() noexcept;
        CustomMaterial(decltype(properties)&& properties, decltype(uniform_offsets)&& offsets, Blending blending, Shading shading, std::string name, uint64_t shader_index, decltype(uniforms)&& uniforms) noexcept;
    public:
        ~CustomMaterial() override = default;

        CustomMaterial(CustomMaterial&&) noexcept;
        CustomMaterial& operator=(CustomMaterial&&) noexcept;

        CustomMaterial(const CustomMaterial&);
        CustomMaterial& operator=(CustomMaterial);

        [[nodiscard]] CustomMaterial* clone() const noexcept override;

        bool isCustom() const noexcept override { return true; }

        template<typename T>
        T& getUniform(const std::string& name) {
            try {
                return static_cast<T&>(uniforms.at(name));
            } catch (const std::out_of_range& e) {
                throw std::runtime_error("No such uniform in custom material.");
            }
        }
    };

    void swap(CustomMaterial& lhs, CustomMaterial& rhs) noexcept;
}
