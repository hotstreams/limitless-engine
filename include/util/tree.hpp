#pragma once

#include <vector>

namespace LimitlessEngine {
    template <typename T>
    class Tree {
    private:
        T data;
        std::vector<Tree<T>> children;
    public:
        explicit Tree(T data) noexcept : data{std::move(data)} {}

        ~Tree() = default;

        Tree(const Tree&) = default;
        Tree &operator=(const Tree&) = default;

        Tree(Tree&&) noexcept = default;
        Tree &operator=(Tree&&) noexcept = default;

        template<typename T1>
        Tree& add(T1&& node) noexcept {
            return children.emplace_back(std::forward<T1>(node));
        }

        [[nodiscard]] auto size() const noexcept { return children.size(); }

        const Tree& operator[](size_t i) const noexcept { return children[i]; }
        Tree& operator[](size_t i) noexcept { return children[i]; }

        const T& operator*() const { return data; }
        T& operator*() { return data; }

        const T& get() const { return data; }
        T &get() { return data; }

        auto begin() noexcept { return children.begin(); }
        auto begin() const noexcept { return children.begin(); }

        auto end() noexcept { return children.end(); }
        auto end() const noexcept { return children.end(); }
    };
}
