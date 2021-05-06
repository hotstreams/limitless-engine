#pragma once

#include <unordered_map>
#include <stdexcept>
#include <memory>
#include <mutex>

namespace Limitless {
    struct resource_container_error : public std::runtime_error {
        explicit resource_container_error(const std::string& error) : runtime_error(error) {}
        explicit resource_container_error(const char* error) : runtime_error(error) {}
    };

    template<typename T>
    class ResourceContainer final {
    private:
        std::unordered_map<std::string, std::shared_ptr<T>> resource;
        // fix const ^at
        mutable std::mutex mutex {};
    public:
        ResourceContainer() = default;
        ~ResourceContainer() = default;

        auto& operator[](const std::string& name) noexcept {
            std::unique_lock lock(mutex);
            return resource[name];
        }

        const auto& at(const std::string& name) {
            std::unique_lock lock(mutex);
            try {
                return resource.at(name);
            } catch (...) {
                throw resource_container_error("No such resource called " + name);
            }
        }

        const auto& at(const std::string& name) const {
            std::unique_lock lock(mutex);
            try {
                return resource.at(name);
            } catch (...) {
                throw resource_container_error("No such resource called " + name);
            }
        }

        void add(const std::string& name, std::shared_ptr<T> res) {
            std::unique_lock lock(mutex);
            const auto result = resource.emplace(name, std::move(res));
            if (!result.second) {
                throw resource_container_error("Failed to add resource " + name + ", already exists.");
            }
        }

        void remove(const std::string& name) {
            std::unique_lock lock(mutex);
            resource.erase(name);
        }

        [[nodiscard]] bool exists(const std::string& name) {
            std::unique_lock lock(mutex);
            return resource.find(name) != resource.end();
        }

        auto begin() noexcept { return resource.begin(); }
        auto begin() const noexcept { return resource.begin(); }

        auto end() noexcept { return resource.end(); }
        auto end() const noexcept { return resource.end(); }
    };
}