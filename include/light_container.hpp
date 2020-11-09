#pragma once

#include <unordered_map>
#include <vector>
#include <memory>

namespace GraphicsEngine {
    class Buffer;

    template<typename T>
    class LightContainer {
        /**
         *      requirement for shader buffer object name in type T
         **/
        static_assert(sizeof(T::shader_storage_name), "value_type must implement static shader_storage_name variable");
    private:
        std::unordered_map<uint64_t, uint64_t> lights_map;
        std::shared_ptr<Buffer> buffer;
        std::vector<T> lights;
        uint64_t next_id {0};
        bool modified {false};
    public:
        explicit LightContainer(uint64_t reserve_count);
        LightContainer();
        ~LightContainer() = default;

        LightContainer(const LightContainer&) = delete;
        LightContainer& operator=(const LightContainer&) = delete;

        LightContainer(LightContainer&&) noexcept = default;
        LightContainer& operator=(LightContainer&&) noexcept = default;

        [[nodiscard]] auto begin() noexcept { return lights.begin(); }
        [[nodiscard]] auto begin() const noexcept { return lights.begin(); }

        [[nodiscard]] auto end() noexcept { return lights.end(); }
        [[nodiscard]] auto end() const noexcept { return lights.end(); }

        [[nodiscard]] auto size() const noexcept { return lights.size(); }

        void reserve(size_t n);

        [[nodiscard]] auto capacity() const noexcept { return lights.capacity(); }
        [[nodiscard]] auto empty() const noexcept { return lights.begin() == lights.end(); }

        T& operator[](size_t id) noexcept { modified = true; return lights[lights_map[id]]; }
        [[nodiscard]] const T& operator[](size_t id) const noexcept { return lights[lights_map.at(id)]; }

        T& at(size_t id) {  modified = true; return lights.at(lights_map.at(id)); }
        [[nodiscard]] const T& at(size_t id) const { return lights.at(lights_map.at(id)); }

        T& back() noexcept { return lights.back(); }
        const T& back() const noexcept { return lights.back(); }

        void erase(uint64_t id);

        void update();

        template<typename... Args>
        auto emplace_back(Args&&... args) {
            if (capacity() == size()) {
                reserve(size() + 1);
            }
            lights.emplace_back(std::forward<Args>(args)...);
            lights_map.emplace(next_id, lights.size() - 1);
            modified = true;
            return next_id++;
        }
    };
}
