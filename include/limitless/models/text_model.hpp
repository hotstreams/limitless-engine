#include <limitless/core/vertex_stream/vertex_array_builder.hpp>
#include <limitless/core/vertex.hpp>
#include <memory>
#include <vector>

namespace Limitless {
    class Buffer;

    class TextModel {
    private:
        std::vector<TextVertex> vertices;
        std::shared_ptr<VertexArray> vertex_array;
        std::shared_ptr<Buffer> buffer;

        void initialize(size_t count);
    public:
        explicit TextModel(std::vector<TextVertex>&& vertices);
//        explicit TextModel(size_t count);

        void update(std::vector<TextVertex>&& vertices);
        void draw() const;
    };
}