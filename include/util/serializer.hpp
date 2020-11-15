#pragma once

#include <filesystem>
#include <fstream>

#include <util/bytewriter.hpp>
#include <util/bytereader.hpp>

namespace GraphicsEngine {
	class serialization_exception : public std::runtime_error {
	public:
		template <typename Arg>
		explicit serialization_exception(Arg&& str) : std::runtime_error{std::forward<Arg>(str)} {}
	};

	class deserialization_exception : public std::runtime_error {
	public:
		template <typename Arg>
		explicit deserialization_exception(Arg&& str) : std::runtime_error{std::forward<Arg>(str)} {}
	};

	class FileSerializer {
	private:
		ByteWriter writer;
		std::ofstream stream;
	public:
		explicit FileSerializer(const std::filesystem::path& path) {
			stream.open(path.c_str(), std::ios::binary);
		}

		template <typename T>
		ByteWriter& operator<<(const T& arg) {
			return writer << arg;
		}

		~FileSerializer() {
			auto buffer = writer.yield();
			if (buffer.getData() && buffer.getSize() > 0)
				stream.write(buffer.getData(), buffer.getSize());
		}
	};

	class FileDeserializer {
	private:
		std::ifstream stream;
		ByteReader reader;
	public:
		explicit FileDeserializer(const std::filesystem::path& path)
			: stream {path.c_str(), std::ios::binary | std::ios::ate}
			, reader {[&]() {
				size_t pos = stream.tellg();
				ByteBuffer buffer {pos};
				stream.seekg(0, std::ios::beg);
				stream.read(buffer.getData(), buffer.getSize());
				return buffer;
			}()} {

		}

		template <typename T>
		operator T() { return static_cast<T>(reader); }

		template <typename T>
		ByteReader& operator>>(T& arg) {
			return reader >> arg;
		}
	};
}
