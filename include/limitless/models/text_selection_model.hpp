#pragma once

#include <limitless/core/vertex_array.hpp>
#include <limitless/core/vertex.hpp>
#include <memory>
#include <vector>

namespace Limitless {
    class TextSelectionModel {
   private:
        VertexArray vertex_array;
        std::shared_ptr<Buffer> buffer;
        std::vector<TextSelectionVertex> vertices;

        void initialize(size_t count);
    public:
        explicit TextSelectionModel(std::vector<TextSelectionVertex>&& vertices);
        explicit TextSelectionModel(size_t count);

        void update(std::vector<TextSelectionVertex>&& vertices);
        void draw() const;
    };
}