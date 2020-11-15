#include <util/bytewriter.hpp>

#include <cstring>
#include <stdexcept>

using namespace GraphicsEngine;

ByteWriter::ByteWriter()
	: buffer{}
	, write_pos {0}
	, end_pos {0} {}

void ByteWriter::resize(const size_t size) {
	buffer.resize(size);
}

void ByteWriter::revert(const size_t index) {
	if (index > getTotalWrittenSize())
		throw std::out_of_range("ByteWriter::revert past end");

	write_pos = index;
	end_pos = index;

	resize(getTotalWrittenSize());
}

void ByteWriter::seek(const size_t index) {
	if (index > getTotalWrittenSize())
		throw std::out_of_range("ByteWriter::seek past end");

	write_pos = index;
}

ByteWriter& ByteWriter::write(const char* ptr, const size_t data_size) {
	const size_t size_after_write = getCurrentWrittenSize() + data_size;

	resizeIfDoesntFit(size_after_write);
	std::memcpy(buffer.getData() + write_pos, ptr, data_size);
	advanceWriteLoc(data_size);

	return *this;
}

ByteWriter& ByteWriter::writeCString(const std::string& str) {
	return write(str.c_str(), str.size() + 1);
}

void ByteWriter::advanceWriteLoc(const size_t by) {
	write_pos += by;
	if (end_pos < write_pos)
		end_pos = write_pos;
}

void ByteWriter::resizeIfDoesntFit(const size_t desired_size) {
	if (!buffer.contains(desired_size)) {
		resize(selectNewCapacity(desired_size));
	}
}

void ByteWriter::shrinkToFit() {
	buffer.resize(getTotalWrittenSize());
}

size_t ByteWriter::selectNewCapacity(size_t desired) {
	if (desired < ByteBuffer::MAX_EMBED_SIZE) // to skip extra resizing until we absolutely have to
		return ByteBuffer::MAX_EMBED_SIZE;
	else if (desired < 64) { // should be about L1 cache line size
		return 64;
	} else {
		return desired * 3 / 2; // then increase capacity by approx x1.5 times
	}
}
