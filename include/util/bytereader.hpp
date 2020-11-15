#pragma once

#include "bytebuffer.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <utility>
#include <map>
#include <unordered_map>
#include <memory>

namespace GraphicsEngine {
	/// Reads data from internal byte buffer
	class ByteReader
	{
	private:
		// Internal byte buffer
		ByteBuffer buffer;

		// Position of data to be read inside internal buffer
		size_t read_pos {0};

		/// Position of the end of available data inside internal buffer
		size_t end_read_pos {0};

		ByteReader() noexcept : buffer{} {}
	public:
		/// Creates ByteReader from a ByteBuffer
		explicit ByteReader(ByteBuffer b) noexcept
			: buffer {std::move(b)}
			, read_pos {0}
			, end_read_pos {buffer.getSize()} {}

		ByteReader(const ByteReader& reader) = default;

		ByteReader(ByteReader&& reader) noexcept : ByteReader{} {
			swap(*this, reader);
		}

		friend void swap(ByteReader& a, ByteReader& b) noexcept {
			using std::swap;
			swap(a.buffer, b.buffer);
			swap(a.read_pos, b.read_pos);
			swap(a.end_read_pos, b.end_read_pos);
		}

		ByteReader& operator=(ByteReader reader) {
			swap(*this, reader);
			return *this;
		}

		/// Reads a bool from buffer
		ByteReader& operator>>(bool& b) {
			return read(reinterpret_cast<char*>(&b), sizeof(bool));
		}

		/// Reads a std::string from buffer (as a C-string)
		ByteReader& operator>>(std::string& str) {
			return readCString(str);
		}

		/// Reads a shallow copy of arbitrary object from buffer
		template <typename T>
		ByteReader& operator>>(T& obj) {
			static_assert(std::is_trivially_copyable_v<T> && std::is_copy_constructible_v<T>);
	        return read(reinterpret_cast<char*>(&obj), sizeof(T));
	    }

	    template <typename K, typename V>
	    ByteReader& operator>>(std::unordered_map<K, V>& map) {
			size_t count;
			*this >> count;

			map.reserve(count);

			for (size_t i = 0; i < count; ++i) {
				map.emplace(*this, *this);
			}

			return *this;
		}

		template <typename K, typename V>
		ByteReader& operator>>(std::map<K, V>& map) {
			size_t count;
			*this >> count;

			for (size_t i = 0; i < count; ++i) {
				map.emplace(*this, *this);
			}

			return *this;
		}

		template <typename T>
		operator T() {
			T obj;
			*this >> obj;
			return obj;
		}

		template <typename T>
		operator std::unique_ptr<T>() {
			return std::make_unique<T>(*this);
		}

		void setAvailable(const size_t size) noexcept {
			end_read_pos = size;
		}

		/// Peeks at arbitrary object in internal buffer
	    template <typename T> ByteReader& peek(T& obj) {
			static_assert(std::is_trivially_copyable_v<T> && std::is_copy_constructible_v<T>);
	    	return peek(reinterpret_cast<char*>(&obj), sizeof(T));
	    }

		/// Peeks into next @size bytes of data and stores them at specified memory location.
		/// It is assumed that it can hold this many bytes.
		ByteReader& peek(char* ptr, size_t size);
		ByteReader& peek(ByteBuffer& buffer);

		/// Skips next @size bytes
		void skip(size_t size);

		/// Returns size of available data (that can be read)
	    [[nodiscard]] size_t getAvailableSize() const noexcept { return end_read_pos - read_pos; }

	    /// Returns size of available data in total
	    [[nodiscard]] size_t getTotalAvailable() const noexcept { return end_read_pos; }

	    // Returns the number of bytes already read
	    [[nodiscard]] size_t getReadSize() const noexcept { return read_pos; }

		/// Returns a pointer to current data
	    [[nodiscard]] const char* getCurrentData() const noexcept { return buffer.getData() + read_pos; };

		/// Reads @size bytes into memory region specified by @ptr
		/// It is assumed that it can hold this many bytes.
	    ByteReader& read(char* ptr, size_t size);

		/// Reads a C-string into a std::string
	    ByteReader& readCString(std::string& str);
	};
}
