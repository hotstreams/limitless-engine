#include <limitless/util/stack_trace.hpp>

#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define NOKERNEL
#define NOUSER
#define NOSERVICE
#define NOSOUND
#define NOMCX
#define NOGDI
#define NOMSG
#define NOMB
#define NOCLIPBOARD

#include <iomanip>

#include <windows.h>
#include <dbghelp.h>
#include <iostream>

#ifdef __MINGW32__
#include <cxxabi.h>
#include <memory>

class FreeDeleter {
public:
    void operator()(void* ptr) {
        free(ptr);
    }
};

static std::string demangle(const std::string& mangled_func_name) {
    int status {-1};
    std::unique_ptr<char, FreeDeleter> demangled_cstr {abi::__cxa_demangle(mangled_func_name.c_str(), nullptr, nullptr, &status)};

    if (status == 0) {
        return std::string(demangled_cstr.get());
    }

    return mangled_func_name;
}

#endif

#if __clang__
static std::string demangle(const std::string& mangled_func_name) {
	// TODO: not supported yet
	return mangled_func_name;
}
#endif

using namespace Limitless;

class SymbolHelper {
public:
    static SymbolHelper& getInstance() {
        static SymbolHelper instance;
        return instance;
    }

    bool isInited() const noexcept { return inited; }
    HANDLE getProcess() const noexcept { return process; }
private:
    bool inited {false};
    HANDLE process;

    SymbolHelper() {
        process = GetCurrentProcess();
        DWORD flags = SymGetOptions();
        SymSetOptions(flags | SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME);
        inited = SymInitialize(process, nullptr, TRUE);
    }

    ~SymbolHelper() {
        if (inited) {
            SymCleanup(process);
        }
    }
};

static std::string getModuleBaseName(void* addr) {
    HMODULE h_module;
    const int max_name_len = 256;
    char module[max_name_len];
    strcpy(module, "");

    GetModuleHandleEx(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (LPCTSTR)addr,
            &h_module
    );

    if (h_module != nullptr) {
        GetModuleFileNameA(h_module, module, max_name_len);
    }

    char* last_slash_pos = strrchr(module, '\\');
    if (last_slash_pos) {
        return std::string{last_slash_pos + 1};
    } else {
        return std::string{module};
    }
}

StackTrace Limitless::getStackTrace(size_t frames_to_skip, size_t max_frames) {
    StackTrace result;
    ++frames_to_skip;

    char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
    auto* p_symbol = reinterpret_cast<PSYMBOL_INFO>(buffer);

    std::unique_ptr<void*[]> back_trace(new void*[max_frames]);

    const USHORT frame_num = CaptureStackBackTrace(
            static_cast<DWORD>(frames_to_skip),
            static_cast<DWORD>(max_frames),
            back_trace.get(),
            nullptr
    );

    auto& sym = SymbolHelper::getInstance();

    for (USHORT i = 0; i < frame_num; ++i) {
        DWORD64 displacement;
        //DWORD disp;
        bool has_symbol {false};
        // bool has_line {false};

        if (sym.isInited()) {
            p_symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
            p_symbol->MaxNameLen = MAX_SYM_NAME;
            has_symbol = SymFromAddr(sym.getProcess(), (ULONG64)back_trace[i], &displacement, p_symbol);

            // auto line = std::make_unique<IMAGEHLP_LINE64>();
            // line->SizeOfStruct = sizeof(IMAGEHLP_LINE64);
            // has_line = SymGetLineFromAddr64(sym.getProcess(), (ULONG64)back_trace[i], &disp, line.get());
        }

        auto module_name = getModuleBaseName(back_trace[i]);

        result.emplace_back(StackFrame{module_name, demangle(std::string{has_symbol ? p_symbol->Name : "<unknown function>"}), std::to_string(p_symbol->Address)});
    }

    return result;
}