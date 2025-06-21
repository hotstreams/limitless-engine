#pragma once

#include <memory>
#include <cstdint>

namespace Limitless {
    template<typename T> class Distribution;
    class ByteBuffer;

    class DistributionSerializer {
    private:
        static constexpr uint8_t VERSION = 0x1;
    public:
        template<typename T>
        ByteBuffer serialize(const Distribution<T>& distr);

        template<typename T>
        std::unique_ptr<Distribution<T>> deserialize(ByteBuffer& buffer);
    };

    template<typename T>
    ByteBuffer& operator<<(ByteBuffer& buffer, const Distribution<T>& distr);

    template<typename T>
    ByteBuffer& operator>>(ByteBuffer& buffer, std::unique_ptr<Distribution<T>>& distr);
}
