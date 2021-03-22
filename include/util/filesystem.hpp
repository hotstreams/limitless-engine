#pragma once

//#if __GNUC__ < 8
//    #include <experimental/filesystem>
//    namespace fs = std::experimental::filesystem;
//#else
    #include <filesystem>
    namespace fs = std::filesystem;
//#endif

#ifdef WIN32
    #define PATH_SEPARATOR "\\"

	inline void convertPathSeparators(char* c_str) {
		for (char* p = c_str; *p; ++p) {
			if (*p == '/') {
				*p = '\\';
			}
		}
	}
#else
    #define PATH_SEPARATOR "/"

	inline void convertPathSeparators(char* c_str) {
		for (char* p = c_str; *p; ++p) {
			if (*p == '\\') {
				*p = '/';
			}
		}
	}
#endif
