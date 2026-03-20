#include "tracy_jit_symbols.h"

#if defined(TRACY_ENABLE) && defined(TRACY_HAS_USER_SYMBOLS)
#include <client/TracyStringHelpers.hpp>
#include <tracy/Tracy.hpp>

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif
#ifndef WIN32_NOMINMAX
    #define WIN32_NOMINMAX
#endif
#ifndef NOMINMAX
    #define NOMINMAX
#endif
#include <windows.h>

namespace tracy_jit {
    std::string JitCodeMap::get_exe_name () {
        char name[1024];
        const auto nameLength = GetModuleFileNameA( NULL, name, sizeof( name ) );
        return "<JIT> ["+std::string(name, nameLength)+"]";
    }
}

namespace tracy {
    extern bool UserDecodeCallstackPtrFast( uint64_t ptr, char* name_buf, size_t buf_size ) {
        if (buf_size <= 0) return false;

        auto& map = tracy_jit::JitCodeMap::instance();
        return map.lookup_code_address( ptr, [name_buf, buf_size] (tracy_jit::JitCodeMap::JittedCodeBlock& block, char const* exe_name) -> void {
            // copy name truncated
            size_t safe_chars = std::min(block.name.size(), buf_size-1);
            memcpy(name_buf, block.name.c_str(), safe_chars);
            name_buf[safe_chars] = '\0';
        } );
    }
    extern bool UserDecodeSymbolAddress( uint64_t ptr, CallstackSymbolData* result ) {
        auto& map = tracy_jit::JitCodeMap::instance();
        return map.lookup_code_address( ptr, [result] (tracy_jit::JitCodeMap::JittedCodeBlock& block, char const* exe_name) -> void {
            result->symAddr = block.entrypoint;
            // we have no source file
            result->file = CopyStringFast("[unknown]");
            result->line = 0;
            result->needFree = true;
        } );
    }
    extern bool UserDecodeCallstackPtr( uint64_t ptr, CallstackEntryData* result, CallstackEntry* cb_data ) {
        auto& map = tracy_jit::JitCodeMap::instance();
        return map.lookup_code_address( ptr, [result, cb_data] (tracy_jit::JitCodeMap::JittedCodeBlock& block, char const* exe_name) -> void {
            cb_data[0].symAddr = block.entrypoint;
            cb_data[0].symLen = block.size;
            cb_data[0].name = CopyStringFast(block.name.c_str(), block.name.size());
            // we have no source file
            cb_data[0].file = CopyStringFast("[unknown]");
            cb_data[0].line = 0;

            *result = { cb_data, 1, exe_name };
        } );
    }
}
#endif
