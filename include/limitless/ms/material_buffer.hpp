#pragma once

#include <limitless/core/buffer/buffer.hpp>

#include <memory>
#include <vector>
#include <unordered_map>

namespace Limitless {
    class Uniform;
}

namespace Limitless::ms {
    class Material;

    class MaterialBuffer final {
    private:
        /**
         *  Corresponding OpenGL buffer to store properties on GPU
         */
        std::shared_ptr<Buffer> material_buffer;

        /**
         *  Property offsets in buffer
         */
        std::unordered_map<std::string, uint64_t> uniform_offsets;

        /**
        *   Adds uniform typed value at specific offset in block so it can be mapped to GPU
        */
        template<typename V>
        void map(std::vector<std::byte>& block, const Uniform& uniform) const;

        /**
        *   Adds any uniform to block
        */
        void map(std::vector<std::byte>& block, Uniform& uniform);

        /**
         *  Maps material to GPU uniform buffer
         */
        void map(const Material& material);
    };
}