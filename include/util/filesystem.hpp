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
#else
    #define PATH_SEPARATOR "/"
#endif
