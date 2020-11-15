#pragma once

#include "bytebuffer.hpp"

#include <utility>
#include <queue>
#include <string>
#include <memory>
#include <map>
#include <unordered_map>

namespace GraphicsEngine {
    /// Writes arbitrary data to internal ByteBuffer
    class ByteWriter {
    private:
        ByteBuffer buffer;

        /// Current writing position inside internal buffer
        size_t write_pos {0};

        /// End of written content inside internal buffer
        size_t end_pos {0};

        void resize(size_t size);
        static size_t selectNewCapacity(size_t desired);
        void resizeIfDoesntFit(size_t desired_size);
        void advanceWriteLoc(size_t by);
    public:
        ByteWriter();

	    explicit ByteWriter(ByteBuffer b)
            : buffer{std::move(b)}
            , write_pos{buffer.getSize()}
            , end_pos{buffer.getSize()} {}

        ByteWriter(const ByteWriter& other) = default;

        ByteWriter(ByteWriter&& writer) noexcept : ByteWriter{} {
            swap(*this, writer);
        }

        friend void swap(ByteWriter& a, ByteWriter& b) noexcept {
            using std::swap;
            swap(a.buffer, b.buffer);
            swap(a.write_pos, b.write_pos);
            swap(a.end_pos, b.end_pos);
        }

        ByteWriter& operator=(ByteWriter writer) noexcept {
            swap(*this, writer);
            return *this;
        }

        /// Concatenates entire ByteWriter to this one
        ByteWriter& operator<<(const ByteWriter& writer) {
            return write(writer.buffer.getData(), writer.getTotalWrittenSize());
        }

        // Concatenates entire ByteBuffer
        ByteWriter& operator<<(const ByteBuffer& other_buffer) {
            return write(other_buffer.getData(), other_buffer.getSize());
        }

        /// Writes std::string as a C-string to internal buffer
        ByteWriter& operator<<(const std::string& str) {
            return writeCString(str);
        }

        template <typename T>
        ByteWriter& operator<<(const std::vector<T>& vector) {
            for (const auto& obj : vector) {
                *this << obj;
            }
            return *this;
        }

        template <typename T>
        ByteWriter& operator<<(const std::vector<T*>& vector) {
	        for (auto* obj : vector) {
		        *this << *obj;
	        }
	        return *this;
        }

        ByteWriter& operator<<(const char* msg) {
	    	return writeCString(msg);
	    }

	    template <typename K, typename V>
	    ByteWriter& operator<<(const std::unordered_map<K, V>& map) {
			*this << map.size();
			for (const auto& [key, value] : map) {
				*this << key << value;
			}
			return *this;
        }

	    template <typename K, typename V>
	    ByteWriter& operator<<(const std::map<K, V>& map) {
		    *this << map.size();
		    for (const auto& [key, value] : map) {
			    *this << key << value;
		    }
		    return *this;
	    }

	    template <typename T>
	    ByteWriter& operator<<(const std::unique_ptr<T>& uptr) {
        	return *this << *uptr;
        }

        /// Writes a shallow copy of object to buffer
        template <typename T>
        ByteWriter& operator<<(const T& obj) {
	        static_assert(std::is_trivially_copyable_v<T> && std::is_copy_constructible_v<T>);
            return write((const char*)&obj, sizeof(T));
        }

        /// Returns size of written data (the buffer can have bigger capacity)
        [[nodiscard]] size_t getTotalWrittenSize() const noexcept {
            return end_pos;
        }

        [[nodiscard]] size_t getCurrentWrittenSize() const noexcept {
            return write_pos;
        }

        /// Writes arbitrary data to the buffer
        ByteWriter& write(const char* ptr, size_t size);

        /// Writes a C-string to the buffer
        ByteWriter& writeCString(const std::string& str);

        /// Sets current write pointer to index
        void seek(size_t index);

        /// Reverts all written data after specified index
        void revert(size_t index);

        /// Shrinks internal buffer size to match total written data size
        void shrinkToFit();

        /// Yields written data as a rvalue ByteBuffer (to be moved)
        /// and resets internal state
        ByteBuffer&& yield() {
        	shrinkToFit();
        	write_pos = 0;
        	end_pos = 0;
            return static_cast<ByteBuffer&&>(buffer);
        }
    };
}
