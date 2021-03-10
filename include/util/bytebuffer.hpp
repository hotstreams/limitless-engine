#pragma once

#include <algorithm>
#include <string>

namespace LimitlessEngine {
    class ByteBuffer final {
    private:
        std::vector<std::byte> buffer;

        void write(const std::byte& bytes, size_t size) {
            buffer.insert(buffer.end(), &bytes, &bytes + size);
        }

        void read(std::byte& bytes, size_t size) {
            std::copy(buffer.begin(), buffer.begin() + size, &bytes);
            buffer.erase(buffer.begin(), buffer.begin() + size);
        }
    public:
        ByteBuffer() = default;

        explicit ByteBuffer(size_t size) {
            buffer.reserve(size);
        }

        ~ByteBuffer() = default;

        ByteBuffer(const ByteBuffer&) = default;
        ByteBuffer& operator=(const ByteBuffer&) = default;

        ByteBuffer(ByteBuffer&&) = default;
        ByteBuffer& operator=(ByteBuffer&&) = default;

        void write(const std::string& str) {
            write(str.size());
            write(reinterpret_cast<const std::byte&>(*str.c_str()), str.size());
        }

        void read(std::string& str) {
            size_t size{};
            read(size);
            str.resize(size);
            read(reinterpret_cast<std::byte&>(*str.data()), size);
        }

        template<typename T, std::enable_if_t<std::is_trivially_copyable_v<std::remove_reference_t<T>>, bool> = true>
        void write(T&& value) {
            write(reinterpret_cast<const std::byte&>(value), sizeof(T));
        }

        template<typename T, std::enable_if_t<std::is_trivially_copyable_v<T>, bool> = true>
        void read(T& value) {
            read(reinterpret_cast<std::byte&>(value), sizeof(T));
        }

        void flip() {
            std::reverse(buffer.begin(), buffer.end());
        }

        template<typename T, std::enable_if_t<std::is_copy_constructible_v<T>, bool> = true>
        T erase() {
            std::array<std::byte, sizeof(T)> value{};
            std::copy(buffer.begin(), buffer.begin() + sizeof(T), value.begin());
            buffer.erase(buffer.begin(), buffer.begin() + sizeof(T));

            return reinterpret_cast<T&>(value);
        }

        template<typename T, std::enable_if_t<std::is_trivially_copyable_v<std::remove_reference_t<T>>, bool> = true>
        ByteBuffer& operator<<(T&& value) {
            write(std::forward<T>(value));
            return *this;
        }

        template<typename T>
        ByteBuffer& operator>>(T& value) {
            read(value);
            return *this;
        }

        ByteBuffer& operator<<(const std::string& str) {
            write(str);
            return *this;
        }

        ByteBuffer& operator>>(std::string& str) {
            read(str);
            return *this;
        }

        template<typename T>
        ByteBuffer& operator<<(const std::vector<T>& v) {
            *this << v.size();
            std::for_each(v.begin(), v.end(), [this] (const auto& el) { *this << el; });
            return *this;
        }

        template<typename T>
        ByteBuffer& operator>>(std::vector<T>& v) {
            size_t size{};
            *this >> size;
            v.reserve(size);
            for (size_t i = 0; i < size; ++i) {
                T value{};
                *this >> value;
                v.emplace_back(std::move(value));
            }
            return *this;
        }

        template<typename T>
        ByteBuffer& operator<<(const std::unique_ptr<T>& ptr) {
            *this << *ptr;
            return *this;
        }

        // TODO: constraints
        template<typename K, typename V, template<typename...> class M>
        ByteBuffer& operator<<(const M<K, V>& m) {
            *this << m.size();
            std::for_each(m.begin(), m.end(), [this](const auto& p) { *this << p.first << p.second; });
            return *this;
        }

        template<typename K, typename V, template<typename...> class M>
        ByteBuffer& operator>>(M<K, V>& m) {
            size_t size{};
            *this >> size;
            for (size_t i = 0; i < size; ++i) {
                K key{};
                V value{};
                *this >> key >> value;
                m.emplace(std::move(key), std::move(value));
            }
            return *this;
        }

        [[nodiscard]] auto begin() const { return buffer.cbegin(); }
        [[nodiscard]] auto end() const { return buffer.cend(); }

        ByteBuffer& operator<<(const ByteBuffer& b) {
            buffer.insert(buffer.end(), b.begin(), b.end());
            return *this;
        }
    };
}