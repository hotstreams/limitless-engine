#pragma once

#include <limitless/fx/modules/module.hpp>

#include <limitless/core/context.hpp>
#include <limitless/camera.hpp>

#include <algorithm>

namespace Limitless::fx {
    template<typename Particle>
    class BeamBuilder : public Module<Particle> {
    private:
        std::vector<BeamParticleMapping> beam_particles;

        void generate(const BeamParticle& particle, Context& ctx, const Camera& camera) {
            constexpr auto DOT_PRODUCT_RANGE = glm::vec2(0.2f, 0.8f);
            const auto resolution = glm::vec2(ctx.getSize().x, ctx.getSize().y);
            const auto& line = particle.derivative_line;

            for (uint32_t i = 0; i < 6 * (line.size() - 2 - 1); ++i) {
                int line_i = i / 6;
                int tri_i = i % 6;

                glm::vec4 va[4];
                const auto VP = camera.getProjection() * camera.getView();
                for (uint32_t k = 0; k < 4; ++k) {
                    va[k] = VP * glm::vec4(line.at(line_i + k), 1.0f);
                    va[k].x /= va[k].w;
                    va[k].y /= va[k].w;
                    va[k].z /= va[k].w;

                    va[k].x = ((glm::vec2(va[k]) + 1.0f) * 0.5f * resolution).x;
                    va[k].y = ((glm::vec2(va[k]) + 1.0f) * 0.5f * resolution).y;
                }

                glm::vec2 v_line = glm::normalize(glm::vec2(va[2]) - glm::vec2(va[1]));
                glm::vec2 nv_line = glm::vec2(-v_line.y, v_line.x);

                glm::vec4 pos;
                if (tri_i == 0 || tri_i == 1 || tri_i == 3) {
                    glm::vec2 v_pred = glm::normalize(glm::vec2(va[1]) - glm::vec2(va[0]));
                    glm::vec2 v_miter = glm::normalize(nv_line + glm::vec2(-v_pred.y, v_pred.x));

                    pos = va[1];
                    auto dot = glm::dot(v_miter, nv_line);
                    dot = glm::min(dot, DOT_PRODUCT_RANGE.y);
                    dot = glm::max(dot, DOT_PRODUCT_RANGE.x);
                    pos.x += (v_miter * particle.size / pos.w * (tri_i == 1 ? -0.5f : 0.5f) / dot).x;
                    pos.y += (v_miter * particle.size / pos.w * (tri_i == 1 ? -0.5f : 0.5f) / dot).y;
                } else {
                    glm::vec2 v_succ = normalize(glm::vec2(va[3]) - glm::vec2(va[2]));
                    glm::vec2 v_miter = normalize(nv_line + glm::vec2(-v_succ.y, v_succ.x));

                    pos = va[2];
                    auto dot = glm::dot(v_miter, nv_line);
                    dot = glm::min(dot, DOT_PRODUCT_RANGE.y);
                    dot = glm::max(dot, DOT_PRODUCT_RANGE.x);
                    pos.x += (v_miter * particle.size / pos.w * (tri_i == 5 ? 0.5f : -0.5f) / dot).x;
                    pos.y += (v_miter * particle.size / pos.w * (tri_i == 5 ? 0.5f : -0.5f) / dot).y;
                }

                pos.x = (glm::vec2(pos) / resolution * 2.0f - 1.0f).x;
                pos.y = (glm::vec2(pos) / resolution * 2.0f - 1.0f).y;

                pos.x *= pos.w;
                pos.y *= pos.w;
                pos.z *= pos.w;

                glm::vec2 uv;
                switch (tri_i) {
                    case 0:
                        uv = {0.0f, 1.0f};
                        break;
                    case 1:
                        uv = {0.0f, 0.0f};
                        break;
                    case 2:
                        uv = {1.0f, 0.0f};
                        break;
                    case 3:
                        uv = {0.0f, 1.0f};
                        break;
                    case 4:
                        uv = {1.0f, 0.0f};
                        break;
                    case 5:
                        uv = {0.0f, 1.0f};
                        break;
                }

                beam_particles.emplace_back(
                        BeamParticleMapping {pos,
                                             uv,
                                             particle.color,
                                             particle.subUV,
                                             particle.properties,
                                             particle.acceleration,
                                             particle.rotation,
                                             particle.velocity,
                                             particle.lifetime,
                                             particle.size
                                    });
            }
        }

        void generate(std::vector<glm::vec3>& line, BeamParticle& particle, glm::vec3 source, glm::vec3 dest, float distance) {
            std::random_device rd;
            std::mt19937 generator(rd());
            auto uni = std::uniform_real_distribution<float>(-distance, distance);

            if (distance < particle.min_offset) {
                line.emplace_back(source);
                line.emplace_back(dest);
            } else {
                glm::vec3 center = (source + dest) * 0.5f;
                glm::vec3 random = {uni(generator), uni(generator), uni(generator)};

                center += random;

                generate(line, particle, source, center, distance * 0.5f);
                generate(line, particle, dest, center, distance * 0.5f);
            }
        }
    public:
        BeamBuilder() noexcept
            : Module<Particle>(ModuleType::BeamBuilder) {}

        ~BeamBuilder() override = default;

        BeamBuilder(const BeamBuilder& module)
            : Module<Particle>(module.type) {}

        const auto& getParticles() const noexcept {
            return beam_particles;
        }

        void update([[maybe_unused]] AbstractEmitter& emitter, std::vector<Particle>& particles, [[maybe_unused]] float dt, Context& ctx, const Camera& camera) noexcept override {
            beam_particles.clear();

            for (auto& particle : particles) {
                const auto current = std::chrono::steady_clock::now();
                const auto delta_time = std::chrono::duration_cast<std::chrono::duration<float>>(current - particle.last_rebuild);

                if (delta_time > particle.rebuild_delta) {
                    auto& line = particle.derivative_line;
                    line.clear();
                    generate(particle.derivative_line, particle, particle.position, particle.target, particle.displacement);

                    // shitty algorithm requirements
                    {
                        std::sort(line.begin(), line.end(), [&](const auto& a, const auto& b) {
                            return glm::distance(particle.position, a) < glm::distance(particle.position, b);
                        });
                        line.erase(std::unique(line.begin(), line.end()), line.end());

                        line.emplace_back(line[0]);
                        line.insert(line.begin(), line[line.size() - 2]);
                    }

                    particle.last_rebuild = current;
                }

                generate(particle, ctx, camera);
            }
        }

        [[nodiscard]] BeamBuilder* clone() const override {
            return new BeamBuilder(*this);
        }
    };
}