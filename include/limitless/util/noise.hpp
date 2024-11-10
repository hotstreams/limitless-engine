#pragma once

#include <vector>
#include <random>
#include <glm/glm.hpp>

namespace Limitless {
    class Noise {
        using NoiseMap = std::vector<std::vector<float>>;
    public:
        template <typename Rng>
        static NoiseMap makeSimpleNoiseMap(glm::uvec2 dim, Rng& rng) {
            NoiseMap result;
            std::uniform_real_distribution<float> distr(0.0f, 1.0f);

            result.resize(dim.y);

            for (auto& row : result) {
                row.resize(dim.x);

                for (float& cell : row) {
                    cell = distr(rng);
                }
            }

            return result;
        }

        static NoiseMap makeSmoothNoiseMap(glm::uvec2 dim, size_t octave, const NoiseMap& base_map) {
            const size_t sample_period = 1 << octave;
            const float sample_freq    = 1.0f / sample_period;

            NoiseMap smooth_map;
            smooth_map.resize(dim.y);
            for (auto& row : smooth_map) {
                row.resize(dim.x, 0.f);
            }

            for (auto y = 0; y < dim.y; ++y) {
                // calculate the horizontal sampling indices
                size_t sy0    = (y / sample_period) * sample_period;
                size_t sy1    = (sy0 + sample_period) % dim.y; // wrap around
                float v_blend = (y - sy0) * sample_freq;

                for (auto x = 0; x < dim.x; ++x) {
                    // calculate the vertical sampling indices
                    int sx0       = (x / sample_period) * sample_period;
                    int sx1       = (sx0 + sample_period) % dim.x; // wrap around
                    float h_blend = (x - sx0) * sample_freq;

                    // blend the top two corners
                    float top = glm::mix(base_map[sy0][sx0], base_map[sy0][sx1], h_blend);

                    // blend the bottom two corners
                    float bottom = glm::mix(base_map[sy1][sx0], base_map[sy1][sx1], h_blend);

                    // final blend
                    smooth_map[y][x] = glm::mix(top, bottom, v_blend);
                }
            }
            return smooth_map;
        }

        template <typename Rng>
        static NoiseMap makePerlinNoiseMap(glm::uvec2 dims, Rng& rng, const size_t octaves) {
            auto base_map    = makeSimpleNoiseMap(dims, rng);
            auto smooth_maps = std::vector<NoiseMap>(octaves);

            for (size_t octave = 0; octave < octaves; ++octave) {
                smooth_maps[octave] = makeSmoothNoiseMap(dims, octave, base_map);
            }

            NoiseMap perlin_map;
            perlin_map.resize(dims.y);
            for (auto& row : perlin_map) {
                row.resize(dims.x, 0.f);
            }

            const float persistance = 0.5f;
            float amplitude         = 1.0f;
            float total_amplitude   = 0.0f;

            // Blend smooth maps together.
            for (auto it = smooth_maps.rbegin(); it != smooth_maps.rend(); ++it) {
                const auto& smooth_map = *it;

                amplitude *= persistance;
                total_amplitude += amplitude;

                for (auto y = 0; y < dims.y; ++y) {
                    for (auto x = 0; x < dims.x; ++x) {
                        perlin_map[y][x] += amplitude * smooth_map[y][x];
                    }
                }
            }

            for (auto& row : perlin_map) {
                for (float& cell : row) {
                    cell /= total_amplitude;
                }
            }

            return perlin_map;
        }
    };
}