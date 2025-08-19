#pragma once

#include <limitless/core/vertex_array.hpp>
#include <limitless/core/vertex.hpp>
#include <memory>
#include <vector>

namespace Limitless {
    class TextSelectionModel {     
    public:
        explicit TextSelectionModel(std::vector<TextSelectionVertex>&& vertices);
        explicit TextSelectionModel(size_t count);

        TextSelectionModel(const TextSelectionModel&) = delete;
        TextSelectionModel& operator=(const TextSelectionModel&) = delete;

        TextSelectionModel(TextSelectionModel&&) noexcept;
        TextSelectionModel& operator=(TextSelectionModel&&) noexcept;

        void update(std::vector<TextSelectionVertex>&& vertices);
        void draw() const;

        [[nodiscard]] bool empty() const noexcept { return vertices.empty(); }

         ~TextSelectionModel();

     private:
        VertexArray vertex_array;
        std::shared_ptr<Buffer> buffer;
        std::vector<TextSelectionVertex> vertices;

        void initialize(size_t count);

        friend void swap(TextSelectionModel& lhs, TextSelectionModel& rhs) noexcept;
    };

    void swap(TextSelectionModel& lhs, TextSelectionModel& rhs) noexcept;
}