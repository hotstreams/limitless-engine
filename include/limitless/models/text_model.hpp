#include <limitless/core/vertex_array.hpp>
#include <limitless/core/vertex.hpp>
#include <memory>
#include <vector>

namespace Limitless {
    class Buffer;

    class TextModel {
    private:
        VertexArray vertex_array;
        std::shared_ptr<Buffer> buffer;
        std::vector<TextVertex> vertices;

        void initialize(size_t count);
    public:
        explicit TextModel(std::vector<TextVertex>&& vertices);
        explicit TextModel(size_t count);

        void update(std::vector<TextVertex>&& vertices);
        void draw() const;
    };
}