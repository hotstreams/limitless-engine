#pragma once

#include <limitless/instances/instance.hpp>
#include <limitless/instances/lod_group_instance.hpp>

namespace Limitless {
    class no_such_mesh : public std::runtime_error {
    private:
        using std::runtime_error::runtime_error;
    };
}

namespace Limitless {
    /**
     * ModelInstance is an instance that contains static model
     */
    class ModelInstance : public Instance {
    protected:
        std::shared_ptr<Model> model;
        LodGroupInstance lod_group;

        void updateBoundingBox() noexcept override;
        ModelInstance(InstanceType shader, decltype(model) model, const glm::vec3& position);
    public:
        /**
         * Creates static model with specified loaded Model
         */
        explicit ModelInstance(decltype(model) model, const glm::vec3& position);

        ~ModelInstance() override = default;

        ModelInstance(const ModelInstance&) = default;
        ModelInstance(ModelInstance&&) noexcept = default;

        /**
         * Updates instance and meshes
         */
        void update(const Camera &camera) override;

        /**
         * Makes copy
         */
        std::unique_ptr<Instance> clone() noexcept override;

        /**
         * Gets used model
         */
        [[nodiscard]] const auto& getAbstractModel() const noexcept { return *model; }


        /**
         * Gets mesh instance with specified name
         *
         * throws no_such_mesh if not found
         */
        MeshInstance& getMeshInstance(const std::string& mesh);

        /**
         * Gets mesh instance with specified index
         *
         * throws no_such_mesh if not found
         */
        MeshInstance& getMeshInstance(uint32_t index);

        /**
         * Gets mesh instance with specified name
         *
         * throws no_such_mesh if not found
         */
        MeshInstance& operator[](const std::string& mesh);

        /**
         * Gets mesh instance with specified index
         *
         * throws no_such_mesh if not found
         */
        MeshInstance& operator[](uint32_t index);

        /**
         *  Gets material of specified mesh
         */
        const std::shared_ptr<ms::Material>& getMaterial(uint32_t mesh_index);

        /**
         *  Gets material of specified mesh
         */
        const std::shared_ptr<ms::Material>& getMaterial(const std::string& mesh_name);

        /**
         *  Changes current material of mesh
         */
        void changeMaterial(uint32_t mesh_index, const std::shared_ptr<ms::Material>& material);

        /**
        *  Changes current material of mesh
        */
        void changeMaterial(const std::string& mesh_name, const std::shared_ptr<ms::Material>& material);

        /**
         *  Changes current materials of all meshes to specified
         */
        void changeMaterials(const std::shared_ptr<ms::Material>& material);

        /**
        *  Changes base material of mesh
        */
        void changeBaseMaterial(uint32_t mesh_index, const std::shared_ptr<ms::Material>& material);

        /**
        *  Changes base material of mesh
        */
        void changeBaseMaterial(const std::string& mesh_name, const std::shared_ptr<ms::Material>& material);

        /**
        *  Changes base materials of all meshes to specified
        */
        void changeBaseMaterials(const std::shared_ptr<ms::Material>& material);

        /**
        *  Resets material to base of mesh
        */
        void resetMaterial(uint32_t mesh_index);

        /**
        *  Resets material to base of mesh
        */
        void resetMaterial(const std::string& mesh_name);

        /**
        *  Resets materials to base of all meshes
        */
        void resetMaterials();

        [[nodiscard]] const auto& getMeshes() const noexcept { return lod_group.getCurrentMeshes(); }
        auto& getMeshes() noexcept { return lod_group.getCurrentMeshes(); }
    };
}