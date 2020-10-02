#include <model_instance.hpp>
#include <skeletal_model.hpp>

namespace GraphicsEngine {
    class SkeletalInstance : public ModelInstance {
    private:
        std::vector<glm::mat4> bone_transform;
        std::shared_ptr<Buffer> bone_buffer;

        const Animation* animation {nullptr};
        bool paused {false};

        const AnimationNode* findAnimationNode(const Bone& bone) const noexcept;
    public:
        SkeletalInstance(std::shared_ptr<AbstractModel> m, const glm::vec3& position);

        void update();

        void play(const std::string& name);
        void pause() noexcept;
        void resume() noexcept;

        void draw(MaterialShader shader_type, Blending blending, const UniformSetter& uniform_setter = UniformSetter{}) override;
    };
}

