#include <util/bytebuffer.hpp>

#include <cstring>

using namespace GraphicsEngine;

ByteBuffer::ByteBuffer(size_t s) {
	static_assert(sizeof(ByteBuffer::storage.heap.size) == sizeof(ByteBuffer::storage.heap.data));
	static_assert(__STDCPP_DEFAULT_NEW_ALIGNMENT__ >= sizeof(ByteBuffer));

	if (s > MAX_EMBED_SIZE || s == 0) {
		storage.heap.size = s;
		storage.heap.data = s ? static_cast<char*>(std::malloc(s)) : nullptr;
	} else {
		storage.embed.size = s;
	}
}

ByteBuffer::ByteBuffer(const ByteBuffer& buffer)
	: ByteBuffer{buffer.getSize()}
{
	std::memcpy(getData(), buffer.getData(), getSize());
}

void ByteBuffer::resize(const size_t new_size) {
	const char* old_data = getData();
	const size_t old_size = getSize();

    if (new_size == old_size)
        return;

    if (new_size == 0) {
	    if (old_size > MAX_EMBED_SIZE)
		    std::free(storage.heap.data);

    	storage.heap.size = 0;
    	storage.heap.data = nullptr;
    } else if (new_size > MAX_EMBED_SIZE) {
	    // tfw allowed to use realloc
	    char* old_alloc_block = old_size > MAX_EMBED_SIZE ? storage.heap.data : nullptr;
	    char* new_data = static_cast<char*>(realloc(old_alloc_block, new_size));

	    if (new_data == nullptr) {
	    	throw std::bad_alloc();
	    }

	    // if old buffer was embedded, then we have to copy its contents ourselves
	    if (old_alloc_block == nullptr && old_data) {
	    	std::memcpy(new_data, old_data, std::min(new_size, old_size));
	    }

	    storage.heap.data = new_data;
	    storage.heap.size = new_size;
    } else {
	    if (old_size > MAX_EMBED_SIZE) {
		    std::memcpy(getEmbeddedData(), old_data, std::min(old_size, new_size));
		    std::free((void*)old_data);
	    }
	    storage.embed.size = new_size;
    }
}

ByteBuffer::~ByteBuffer() {
	if (isNotEmbedded() && storage.heap.size != 0)
		std::free(storage.heap.data);
}
