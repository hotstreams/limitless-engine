#pragma once

#include <distribution.hpp>
#include <mesh.hpp>
#include <chrono>

namespace GraphicsEngine {
    class Emitter;
    struct Particle;

    enum class EmitterModuleType {
        InitialLocation,
        InitialRotation,
        InitialVelocity,
        InitialColor,
        InitialSize,
        InitialAcceleration,

        MeshLocation,
        SubUV,
        //VelocityByLife,
        ColorByLife,
        RotationRate,
        SizeByLife,
        //SphereLocation,
        //CustomMaterial

        //lifetime should be updated at last
        Lifetime
    };

    class EmitterModule {
    public:
        EmitterModule() noexcept = default;
        virtual ~EmitterModule() = default;

        virtual void initialize(Emitter& emitter, Particle& particle) noexcept;
        virtual void update(Emitter& emitter, std::vector<Particle>& particles, float dt) noexcept;

        [[nodiscard]] virtual EmitterModule* clone() const noexcept = 0;
    };

    class InitialLocation : public EmitterModule {
    private:
        std::unique_ptr<Distribution<glm::vec3>> distribution;
    public:
        explicit InitialLocation(Distribution<glm::vec3>* distribution) noexcept;
        ~InitialLocation() override = default;

        InitialLocation(const InitialLocation&) noexcept;

        void initialize(Emitter& emitter, Particle& particle) noexcept override;

        [[nodiscard]] InitialLocation* clone() const noexcept override;

        [[nodiscard]] auto& getDistribution() noexcept { return distribution; }
    };

    class InitialRotation : public EmitterModule {
    private:
        std::unique_ptr<Distribution<glm::vec3>> distribution;
    public:
        explicit InitialRotation(Distribution<glm::vec3>* distribution) noexcept;
        ~InitialRotation() override = default;

        InitialRotation(const InitialRotation&) noexcept;

        void initialize(Emitter& emitter, Particle& particle) noexcept override;

        [[nodiscard]] InitialRotation* clone() const noexcept override;

        [[nodiscard]] auto& getDistribution() noexcept { return distribution; }
    };

    class InitialVelocity : public EmitterModule {
    private:
        std::unique_ptr<Distribution<glm::vec3>> distribution;
    public:
        explicit InitialVelocity(Distribution<glm::vec3>* distribution) noexcept;
        ~InitialVelocity() override = default;

        InitialVelocity(const InitialVelocity&) noexcept;

        void initialize(Emitter& emitter, Particle& particle) noexcept override;

        [[nodiscard]] InitialVelocity* clone() const noexcept override;

        [[nodiscard]] auto& getDistribution() noexcept { return distribution; }
    };

    class InitialColor : public EmitterModule {
    private:
        std::unique_ptr<Distribution<glm::vec4>> distribution;
    public:
        explicit InitialColor(Distribution<glm::vec4>* distribution) noexcept;
        ~InitialColor() override = default;

        InitialColor(const InitialColor&) noexcept;

        void initialize(Emitter& emitter, Particle& particle) noexcept override;

        [[nodiscard]] InitialColor* clone() const noexcept override;

        [[nodiscard]] auto& getDistribution() noexcept { return distribution; }
    };

    class InitialSize : public EmitterModule {
    private:
        std::unique_ptr<Distribution<float>> distribution;
    public:
        explicit InitialSize(Distribution<float>* distribution) noexcept;
        ~InitialSize() override = default;

        InitialSize(const InitialSize&) noexcept;

        void initialize(Emitter& emitter, Particle& particle) noexcept override;

        [[nodiscard]] InitialSize* clone() const noexcept override;

        [[nodiscard]] auto& getDistribution() noexcept { return distribution; }
    };

    class InitialAcceleration : public EmitterModule {
    private:
        std::unique_ptr<Distribution<glm::vec3>> distribution;
    public:
        explicit InitialAcceleration(Distribution<glm::vec3>* distribution) noexcept;
        ~InitialAcceleration() override = default;

        InitialAcceleration(const InitialAcceleration&) noexcept;

        void initialize(Emitter& emitter, Particle& particle) noexcept override;

        [[nodiscard]] InitialAcceleration* clone() const noexcept override;

        [[nodiscard]] auto& getDistribution() noexcept { return distribution; }
    };

    class SubUV : public EmitterModule {
    private:
        // uv of all frames
        std::vector<glm::vec2> frames;
        // rendering frames per second
        float fps;
        // scaling factor to frame-sprite space
        glm::vec2 subUV_factor;
        // last time updated
        std::chrono::time_point<std::chrono::steady_clock> last_time;
        bool first_update {false};
        // texture size
        glm::vec2 texture_size;
        // frame count
        glm::vec2 frame_count;

        void updateFrames() noexcept;
    public:
        explicit SubUV(const glm::vec2& texture_size, float fps, const glm::vec2& frame_count) noexcept;
        ~SubUV() override = default;

        SubUV(const SubUV&) = default;

        [[nodiscard]] SubUV* clone() const noexcept override;

        void initialize(Emitter& emitter, Particle& particle) noexcept override;
        void update(Emitter& emitter, std::vector<Particle>& particles, float dt) noexcept override;

        [[nodiscard]] const auto& getTextureSize() const { return texture_size; }
        void setTextureSize(const glm::vec2& size) { texture_size = size; updateFrames(); }

        [[nodiscard]] const auto& getFrameCount() const { return frame_count; }
        void setFrameCount(const glm::vec2& count) { frame_count = count; updateFrames(); }

        [[nodiscard]] auto& getFPS() const noexcept { return fps; }
    };

    class Lifetime : public EmitterModule {
    private:
        std::unique_ptr<Distribution<float>> distribution;
    public:
        explicit Lifetime(Distribution<float>* distribution) noexcept;
        ~Lifetime() override = default;

        Lifetime(const Lifetime&) noexcept;

        void initialize(Emitter& emitter, Particle& particle) noexcept override;
        void update(Emitter& emitter, std::vector<Particle>& particles, float dt) noexcept override;

        [[nodiscard]] Lifetime* clone() const noexcept override;

        [[nodiscard]] auto& getDistribution() noexcept { return distribution; }
    };

    class ColorByLife : public EmitterModule {
    private:
        std::unique_ptr<Distribution<glm::vec4>> distribution;
    public:
        explicit ColorByLife(Distribution<glm::vec4>* distribution) noexcept;
        ~ColorByLife() override = default;

        ColorByLife(const ColorByLife&) noexcept;

        void initialize(Emitter& emitter, Particle& particle) noexcept override;
        void update(Emitter& emitter, std::vector<Particle>& particles, float dt) noexcept override;

        [[nodiscard]] ColorByLife* clone() const noexcept override;

        [[nodiscard]] auto& getDistribution() noexcept { return distribution; }
    };

    class RotationRate : public EmitterModule {
    private:
        std::unique_ptr<Distribution<glm::vec3>> distribution;
    public:
        explicit RotationRate(Distribution<glm::vec3>* distribution) noexcept;
        ~RotationRate() override = default;

        RotationRate(const RotationRate&) noexcept;

        void update(Emitter& emitter, std::vector<Particle>& particles, float dt) noexcept override;

        [[nodiscard]] RotationRate* clone() const noexcept override;

        [[nodiscard]] auto& getDistribution() noexcept { return distribution; }
    };

    class SizeByLife : public EmitterModule {
    private:
        std::unique_ptr<Distribution<float>> distribution;
        float factor;
    public:
        explicit SizeByLife(Distribution<float>* distribution, float factor) noexcept;
        ~SizeByLife() override = default;

        SizeByLife(const SizeByLife&) noexcept;

        void update(Emitter& emitter, std::vector<Particle>& particles, float dt) noexcept override;

        [[nodiscard]] SizeByLife* clone() const noexcept override;

        [[nodiscard]] auto& getDistribution() noexcept { return distribution; }
        [[nodiscard]] auto& getFactor() noexcept { return factor; }
    };

//    class VelocityByLife : public EmitterModule {
//    private:
//        std::unique_ptr<Distribution<glm::vec3>> distribution;
//        float factor;
//    public:
//        explicit VelocityByLife(Distribution<glm::vec3>* distribution, float factor) noexcept;
//        ~VelocityByLife() override = default;
//
//        VelocityByLife(const VelocityByLife&) noexcept;
//
//        void update(Emitter& emitter, std::vector<Particle>& particles, float dt) noexcept override;
//
//        [[nodiscard]] VelocityByLife* clone() const noexcept override;
//
//        [[nodiscard]] auto& getDistribution() noexcept { return distribution; }
//        [[nodiscard]] auto& getFactor() noexcept { return factor; }
//    };

    class MeshLocation : public EmitterModule {
    private:
        std::shared_ptr<AbstractMesh> mesh;

        std::default_random_engine generator;
        std::uniform_real_distribution<float> distribution;

        glm::vec3 getPositionOnTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) noexcept;
        glm::vec3 getPositionOnMesh() noexcept;
    public:
        explicit MeshLocation(std::shared_ptr<AbstractMesh> mesh) noexcept;
        ~MeshLocation() override = default;

        MeshLocation(const MeshLocation&) noexcept = default;
        MeshLocation& operator=(const MeshLocation&) noexcept = default;

        void initialize(Emitter& emitter, Particle& particle) noexcept override;
        void update(Emitter& emitter, std::vector<Particle>& particles, float dt) noexcept override;

        [[nodiscard]] MeshLocation* clone() const noexcept override;
    };
}
