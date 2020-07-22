#pragma once

namespace GraphicsEngine {
    template <typename T>
    class Tree {
    private:
        T data;
        Tree<T> **children;
        size_t child_size;
    public:
        explicit Tree(T data) noexcept : data{std::move(data)}, children{nullptr}, child_size{0} {}

        Tree(Tree<T> &&other) noexcept
                : data(std::move(other.data)), children(other.children), child_size(other.child_size) {
            other.children = nullptr;
            other.child_size = 0;
        }

        Tree &operator=(Tree<T> &&other) noexcept {
            data = std::move(other.data);
            children = std::move(other.children);
            child_size = other.child_size;

            other.children = nullptr;
            other.child_size = 0;

            return *this;
        }

        Tree(const Tree &) noexcept = delete;
        Tree &operator=(const Tree &) noexcept = delete;

        ~Tree() {
            for (size_t i = 0; i < child_size; ++i)
                delete children[i];
            delete[] children;
        }

        Tree &add(T d) noexcept {
            return add(new Tree(d));
        }

        Tree &add(Tree *node) noexcept {
            auto **new_children = new Tree *[child_size + 1];

            for (size_t i = 0; i < child_size; ++i)
                new_children[i] = children[i];

            new_children[child_size] = node;

            delete[] children;
            children = new_children;
            return *children[child_size++];
        }

        [[nodiscard]] size_t size() const noexcept { return child_size; }
        Tree &operator[](size_t i) const noexcept { return *children[i]; }
        const T &operator*() const { return data; }
        const T &get() const { return data; }
        T &operator*() { return data; }
        T &get() { return data; }
    };
}
