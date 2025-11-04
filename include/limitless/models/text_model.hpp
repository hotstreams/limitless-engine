#include <limitless/core/vertex_stream/vertex_array_builder.hpp>
#include <limitless/core/vertex.hpp>
#include <memory>
#include <vector>
#include <utility>

#include "limitless/core/vertex_stream/vertex_stream.hpp"

namespace Limitless {
    class Buffer;

    class TextModel {
    public:
        explicit TextModel(std::vector<TextVertex>&& vertices);
        explicit TextModel(size_t count);

        void update(std::vector<TextVertex>&& vertices);
        void draw() const;

    private:
        std::vector<TextVertex> vertices;
        std::shared_ptr<VertexStream> vertex_stream;

        void initialize(size_t count);
    };
}
