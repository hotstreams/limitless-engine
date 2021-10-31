#pragma once

#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

#define WIN_PATH_SEPARATOR_CHAR '\\'
#define UNIX_PATH_SEPARATOR_CHAR '/'

#ifdef WIN32
    #define PATH_SEPARATOR "\\"
	#define PATH_SEPARATOR_CHAR WIN_PATH_SEPARATOR_CHAR

        inline fs::path convertPathSeparators(const fs::path& path) {
            auto new_path = path.string();
            std::replace(new_path.begin(), new_path.end(), UNIX_PATH_SEPARATOR_CHAR, WIN_PATH_SEPARATOR_CHAR);
            return new_path;
        }
#else
    #define PATH_SEPARATOR "/"
    #define PATH_SEPARATOR_CHAR UNIX_PATH_SEPARATOR_CHAR

	inline fs::path convertPathSeparators(const fs::path& path) {
		auto new_path = path.string();
	    std::replace(new_path.begin(), new_path.end(), WIN_PATH_SEPARATOR_CHAR, UNIX_PATH_SEPARATOR_CHAR);
	    return new_path;
	}
#endif
