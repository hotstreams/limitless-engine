#pragma once

#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

#ifdef WIN32
    #define PATH_SEPARATOR "\\"

        inline fs::path convertPathSeparators(const fs::path& path) {
            auto new_path = path.string();
            std::replace(new_path.begin(), new_path.end(), '/', '\\');
            return new_path;
        }
#else
    #define PATH_SEPARATOR "/"

	inline fs::path convertPathSeparators(const fs::path& path) {
		auto new_path = path.string();
	    std::replace(new_path.begin(), new_path.end(), '\\', '/');
	    return new_path;
	}
#endif
