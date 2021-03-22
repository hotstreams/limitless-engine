#include <limitless/particle_system/distribution_serializer.hpp>

#include <limitless/particle_system/distribution.hpp>
#include <limitless/util/bytebuffer.hpp>

using namespace LimitlessEngine;

template<typename T>
ByteBuffer DistributionSerializer::serialize(Distribution<T>& distr) {
    ByteBuffer buffer;

    buffer << distr.getType();

    switch (distr.getType()) {
        case DistributionType::Const:
            buffer << distr.get();
            break;
        case DistributionType::Range:
            buffer << static_cast<RangeDistribution<T>&>(distr).getMin()
                   << static_cast<RangeDistribution<T>&>(distr).getMax();
            break;
        case DistributionType::Curve:
            assert("TODO");
            break;
    }

    return buffer;
}

template<typename T>
std::unique_ptr<Distribution<T>> DistributionSerializer::deserialize(ByteBuffer& buffer) {
    DistributionType type{};
    buffer >> type;

    std::unique_ptr<Distribution<T>> distribution;
    switch (type) {
        case DistributionType::Const: {
            T value{};
            buffer >> value;
            distribution = std::unique_ptr<Distribution<T>>(new ConstDistribution<T>(value));
            break;
        }
        case DistributionType::Range: {
            T min{};
            T max{};
            buffer >> min >> max;
            distribution = std::unique_ptr<Distribution<T>>(new RangeDistribution<T>(min, max));
            break;
        }
        case DistributionType::Curve:
            assert("TODO");
            break;
    }

    return distribution;
}

template<typename T>
ByteBuffer& LimitlessEngine::operator<<(ByteBuffer& buffer, Distribution<T>& distr) {
    DistributionSerializer serializer;
    buffer << serializer.serialize<T>(distr);
    return buffer;
}

template<typename T>
ByteBuffer& LimitlessEngine::operator>>(ByteBuffer& buffer, std::unique_ptr<Distribution<T>>& distr) {
    DistributionSerializer serializer;
    distr = serializer.deserialize<T>(buffer);
    return buffer;
}

namespace LimitlessEngine {
    template ByteBuffer& operator<<(ByteBuffer& buffer, Distribution<float>& distr);
    template ByteBuffer& operator<<(ByteBuffer& buffer, Distribution<uint32_t>& distr);
    template ByteBuffer& operator<<(ByteBuffer& buffer, Distribution<glm::vec3>& distr);
    template ByteBuffer& operator<<(ByteBuffer& buffer, Distribution<glm::vec4>& distr);

    template ByteBuffer& operator>>(ByteBuffer& buffer, std::unique_ptr<Distribution<float>>& distr);
    template ByteBuffer& operator>>(ByteBuffer& buffer, std::unique_ptr<Distribution<uint32_t>>& distr);
    template ByteBuffer& operator>>(ByteBuffer& buffer, std::unique_ptr<Distribution<glm::vec3>>& distr);
    template ByteBuffer& operator>>(ByteBuffer& buffer, std::unique_ptr<Distribution<glm::vec4>>& distr);
}
