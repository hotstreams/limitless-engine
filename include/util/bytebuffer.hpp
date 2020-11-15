#pragma once

#include <cstddef>
#include <algorithm>

namespace GraphicsEngine {
    class ByteBuffer {
    private:
    	union Storage
	    {
    		struct HeapStorage
		    {
			    char* data;
			    size_t size;
		    } heap;
    		struct EmbeddedStorage {
    			uint8_t size;
    			char data[sizeof(HeapStorage) - sizeof(size)];
    		} embed;
	    } storage;

	    [[nodiscard]] char* getEmbeddedData() noexcept { return storage.embed.data; }
	    [[nodiscard]] const char* getEmbeddedData() const noexcept { return storage.embed.data; }

	    [[nodiscard]] size_t getEmbeddedSize() const noexcept {
		    return storage.embed.size & MAX_EMBED_SIZE;
	    }

    public:
        explicit ByteBuffer(size_t s = 0);

        ByteBuffer(const ByteBuffer& buffer);

        ByteBuffer(ByteBuffer&& buffer) noexcept : ByteBuffer{0} {
            swap(*this, buffer);
        }

        friend void swap(ByteBuffer& a, ByteBuffer& b) noexcept {
            using std::swap;
            swap(a.storage, b.storage);
        }

        ByteBuffer& operator=(ByteBuffer buffer) {
            swap(*this, buffer);
            return *this;
        }

	    // cant do sizeof(ByteBuffer) since its incomplete here
	    static constexpr size_t MAX_EMBED_SIZE = sizeof(storage.heap.data) + sizeof(storage.heap.size) - sizeof(storage.embed.size);
	    [[nodiscard]] bool isEmbedded() const noexcept { return getEmbeddedSize() > 0; }
	    [[nodiscard]] bool isNotEmbedded() const noexcept { return !isEmbedded(); }

        [[nodiscard]] char* getData() noexcept {
        	return isNotEmbedded() ? storage.heap.data : getEmbeddedData();
        }
        [[nodiscard]] const char* getData() const noexcept {
        	return isNotEmbedded() ? storage.heap.data : getEmbeddedData();
        }

        [[nodiscard]] size_t getSize() const noexcept {
        	return isNotEmbedded() ? storage.heap.size : getEmbeddedSize();
        }
	    [[nodiscard]] char* begin() noexcept { return getData(); }
	    [[nodiscard]] char* end() noexcept { return getData() + getSize(); }
        [[nodiscard]] const char* begin() const noexcept { return getData(); }
        [[nodiscard]] const char* end() const noexcept { return getData() + getSize(); }

        [[nodiscard]] bool contains(const char* const ptr) const noexcept { return ptr >= getData() && ptr < getData() + getSize(); }
        [[nodiscard]] bool contains(size_t index) const noexcept { return index < getSize(); }

        void resize(size_t new_size);

        ~ByteBuffer();
    };
}
