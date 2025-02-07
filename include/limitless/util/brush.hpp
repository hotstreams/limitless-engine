#pragma once

namespace Limitless {
    class Brush {
    private:
        std::string name;
        std::vector<std::byte> data;
        glm::uvec2 size;
    public:
        Brush(std::string name, glm::uvec2 size, std::vector<std::byte> data)
            : name {std::move(name)}
            , data {std::move(data)}
            , size {std::move(size)} {
        }

        [[nodiscard]] const auto& getSize() const noexcept { return size; }

        float getAt(glm::uvec2 pos) const noexcept {
            return static_cast<uint32_t>(data[pos.y * size.x + pos.x]) / 255.0f;
        }

        float operator[](glm::uvec2 pos) const noexcept {
            return getAt(pos);
        }
    };
}