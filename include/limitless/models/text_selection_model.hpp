#pragma once

#include <limitless/core/vertex.hpp>
#include <memory>
#include <vector>

#include "limitless/core/vertex_stream/vertex_stream.hpp"

namespace Limitless {
    class TextSelectionModel {     
    public:
        explicit TextSelectionModel(std::vector<TextSelectionVertex>&& vertices);
        explicit TextSelectionModel(size_t count);

        TextSelectionModel(const TextSelectionModel&) = delete;
        TextSelectionModel& operator=(const TextSelectionModel&) = delete;

        void update(std::vector<TextSelectionVertex>&& vertices);
        void draw() const;

        [[nodiscard]] bool empty() const noexcept { return vertices.empty(); }

         ~TextSelectionModel();

     private:
        std::vector<TextSelectionVertex> vertices;
        std::shared_ptr<VertexStream> vertex_stream;
    };
}
