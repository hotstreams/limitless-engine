#pragma once

#include <glm/glm.hpp>
#include <random>

namespace Limitless {
    // Custom uniform distribution for GLM vector types
    template<typename T>
    class uniform_vector_distribution {
    private:
        T min, max;
        std::uniform_real_distribution<float> float_dist;
    public:
        uniform_vector_distribution(const T& min, const T& max) noexcept
                : min(min), max(max), float_dist(0.0f, 1.0f) {}

        template<typename Generator>
        T operator()(Generator& gen) {
            if constexpr (std::is_same_v<T, glm::vec2>) {
                return { float_dist(gen) * (max.x - min.x) + min.x,
                         float_dist(gen) * (max.y - min.y) + min.y };
            } else if constexpr (std::is_same_v<T, glm::vec3>) {
                return { float_dist(gen) * (max.x - min.x) + min.x,
                         float_dist(gen) * (max.y - min.y) + min.y,
                         float_dist(gen) * (max.z - min.z) + min.z };
            } else if constexpr (std::is_same_v<T, glm::vec4>) {
                return { float_dist(gen) * (max.x - min.x) + min.x,
                         float_dist(gen) * (max.y - min.y) + min.y,
                         float_dist(gen) * (max.z - min.z) + min.z,
                         float_dist(gen) * (max.w - min.w) + min.w };
            }
        }

        void set(const T& new_min, const T& new_max) noexcept {
            min = new_min;
            max = new_max;
        }
    };
}

namespace Limitless {
    enum class DistributionType { Const, Range, Curve };

    template<typename T>
    class Distribution {
    protected:
        DistributionType type;
        explicit Distribution(DistributionType type) noexcept : type{type} { }
    public:
        virtual ~Distribution() = default;

        [[nodiscard]] virtual T get() = 0;
        [[nodiscard]] virtual T get() const = 0;
        [[nodiscard]] virtual Distribution<T>* clone() = 0;
        [[nodiscard]] const auto& getType() const noexcept { return type; }
    };

    template<typename T>
    class ConstDistribution : public Distribution<T> {
    private:
        T value;
    public:
        explicit ConstDistribution(const T& value) noexcept : Distribution<T>(DistributionType::Const), value{value} {}
        ~ConstDistribution() override = default;

        T get() override { return value; }
        T get() const override { return value; }

        T& getValue() const noexcept { return value; }
        T& getValue() noexcept { return value; }

        [[nodiscard]] Distribution<T>* clone() override {
            return new ConstDistribution<T>(*this);
        }
    };

    template<typename T, typename E = void>
    class uniform_distribution { static_assert("kek i shrek bratya na vek"); };

    template<typename T>
    class uniform_distribution<T, typename std::enable_if_t<std::is_integral_v<T>>> {
        std::uniform_int_distribution<T> distribution;
    public:
        uniform_distribution(T min, T max) : distribution{min, max} {}
        void set(const T& min, const T& max) { distribution = std::uniform_int_distribution{min, max}; }
        template<typename Gen> auto operator()(Gen&& gen) { return distribution(std::forward<Gen>(gen)); }
    };

    template<typename T>
    class uniform_distribution<T, typename std::enable_if_t<std::is_floating_point_v<T>>> {
        std::uniform_real_distribution<T> distribution;
    public:
        uniform_distribution(T min, T max) : distribution{min, max} {}
        void set(const T& min, const T& max) { distribution = std::uniform_real_distribution{min, max}; }
        template<typename Gen> auto operator()(Gen&& gen) { return distribution(std::forward<Gen>(gen)); }
    };

    // Specialization for GLM vector types
    template<typename T>
    class uniform_distribution<T, typename std::enable_if_t<
        std::is_same_v<T, glm::vec2> || std::is_same_v<T, glm::vec3> || std::is_same_v<T, glm::vec4>
    >> {
        uniform_vector_distribution<T> distribution;
    public:
        uniform_distribution(const T& min, const T& max) : distribution{min, max} {}
        void set(const T& min, const T& max) { distribution.set(min, max); }
        template<typename Gen> auto operator()(Gen&& gen) { return distribution(std::forward<Gen>(gen)); }
    };

    template<typename T>
    class RangeDistribution : public Distribution<T> {
    private:
        T min, max;

        mutable std::default_random_engine generator;
        mutable uniform_distribution<T> distribution;
    public:
        RangeDistribution(const T& min, const T& max) noexcept
            : Distribution<T>(DistributionType::Range)
            , min(min)
            , max(max)
            , distribution(min, max) {}
        ~RangeDistribution() override = default;

        [[nodiscard]] const T& getMin() const noexcept { return min; }
        [[nodiscard]] T& getMin() noexcept { return min; }
        [[nodiscard]] const T& getMax() const noexcept { return max; }
        [[nodiscard]] T& getMax() noexcept { return max; }

        void setMin(const T& _min) noexcept {
            min = _min;
            distribution.set(min, max);
        }

        void setMax(const T& _max) noexcept {
            max = _max;
            distribution.set(min, max);
        }

        T get() override { return distribution(generator); }
        T get() const override { return distribution(generator); }

        [[nodiscard]] Distribution<T>* clone() override {
            return new RangeDistribution<T>(*this);
        }
    };

    //TODO:
    template<typename T>
    class CurveDistribution : public Distribution<T> {
    private:

    public:

    };
}