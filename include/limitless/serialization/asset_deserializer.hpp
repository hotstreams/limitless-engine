#pragma once

#include <limitless/util/bytebuffer.hpp>

#include <set>

namespace Limitless {
    class Assets;
    class Context;
    class RenderSettings;

    template<typename T>
    struct AssetDeserializer {
        Context& context;
        Assets& assets;
        const RenderSettings& settings;

        T& asset;
    };

    template<typename K, typename C>
    ByteBuffer& operator>>(ByteBuffer& buffer, const AssetDeserializer<std::set<K, C>>& asset_map) {
        auto& [context, assets, settings, asset] = asset_map;
        size_t size{};
        buffer >> size;
        for (size_t i = 0; i < size; ++i) {
            K key{};
            buffer >> AssetDeserializer<K>{context, assets, settings, key};
            asset.emplace(std::move(key));
        }
        return buffer;
    }

    template<typename K, typename V, template<typename...> class M>
    ByteBuffer& operator>>(ByteBuffer& buffer, const AssetDeserializer<M<K, V>>& asset_map) {
        auto& [context, assets, settings, asset] = asset_map;
        size_t size{};
        buffer >> size;
        for (size_t i = 0; i < size; ++i) {
            K key{};
            V value{};
            buffer >> key >> AssetDeserializer<V>{context, assets, settings, value};
            asset.emplace(std::move(key), std::move(value));
        }
        return buffer;
    }
}