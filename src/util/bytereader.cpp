#include <util/bytereader.hpp>

#include <stdexcept>
#include <iostream>
#include <cstring>
#include <algorithm>

using namespace GraphicsEngine;

ByteReader& ByteReader::read(char* ptr, const size_t size) {
	if (getAvailableSize() < size) {
		throw std::out_of_range("ByteReader exhausted");
	}

	std::memcpy(ptr, buffer.getData() + read_pos, size);

	read_pos += size;
	return *this;
}

ByteReader& ByteReader::peek(char* ptr, const size_t size) {
	read(ptr, size);
	read_pos -= size;
	return *this;
}

ByteReader& ByteReader::readCString(std::string& str) {
	size_t i = 0;
	const char* rp = buffer.getData() + read_pos;

	do {
		if (getAvailableSize() == i)
			throw std::out_of_range("ByteReader exhausted");
	} while (rp[i++] != '\0');

	str.resize(i - 1);
	std::memcpy(str.data(), rp, i - 1);
	read_pos += i;
	return *this;
}

void ByteReader::skip(const size_t size)
{
	if (getAvailableSize() < size) {
		throw std::out_of_range("ByteReader exhausted");
	}
	read_pos += size;
}

ByteReader& ByteReader::peek(ByteBuffer& into) {
	return peek(into.getData(), into.getSize());
}
