#pragma once

#include <limitless/pipeline/shader_pass_types.hpp>
#include <memory>
#include <map>
#include <vector>

namespace Limitless::ms {
    class Material;
}

namespace Limitless {
    class Context;

    class MaterialInstance final {
    private:
        uint64_t next_id {};
        std::shared_ptr<ms::Material> base;
        std::map<uint64_t, std::shared_ptr<ms::Material>> materials;
    public:
        explicit MaterialInstance(const std::shared_ptr<ms::Material>& material);
        ~MaterialInstance() = default;

        MaterialInstance(const MaterialInstance&);
        MaterialInstance(MaterialInstance&&) = default;

        // changes base material
        void changeMaterial(const std::shared_ptr<ms::Material>& material) noexcept;

        // resets base material to first initialized
        void reset() noexcept;

        // clears all applied materials
        void clear() noexcept;

        // applies material as a layer and returns its id
        uint64_t apply(const std::shared_ptr<ms::Material>& material) noexcept;

        // removes 'id' layer
        void remove(uint64_t id);

        // sets context state for %id layer
        void setMaterialState(Context& ctx, uint64_t id, ShaderPass pass);

        // gets material layer
        ms::Material& operator[](uint64_t id) { return *materials.at(id); }
        const ms::Material& operator[](uint64_t id) const { return *materials.at(id); }

        [[nodiscard]] auto count() const noexcept { return materials.size(); }

        [[nodiscard]] auto begin() const noexcept { return materials.cbegin(); }
        [[nodiscard]] auto begin() noexcept { return materials.begin(); }

        [[nodiscard]] auto end() const noexcept { return materials.cend(); }
        [[nodiscard]] auto end() noexcept { return materials.end(); }
    };
}