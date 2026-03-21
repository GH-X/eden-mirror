// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include "tracy_jit_symbols.h"

#if defined(TRACY_ENABLE) && defined(TRACY_HAS_USER_SYMBOLS)
#include <client/TracyStringHelpers.hpp>
#include <tracy/Tracy.hpp>

namespace tracy_jit {
    std::string JitCodeMap::get_module_name () {
        return "[JIT_Code]";
    }
}

namespace tracy {
    extern bool UserDecodeCallstackPtrFast( uint64_t ptr, char* name_buf, size_t buf_size ) {
        if (buf_size <= 0) return false;

        auto& map = tracy_jit::JitCodeMap::instance();
        return map.lookup_code_address( ptr, [name_buf, buf_size] (tracy_jit::JitCodeMap::JittedCodeBlock& block, char const* module_name) -> void {
            // copy name truncated
            size_t safe_chars = std::min(block.name.size(), buf_size-1);
            memcpy(name_buf, block.name.c_str(), safe_chars);
            name_buf[safe_chars] = '\0';
        } );
    }
    extern bool UserDecodeSymbolAddress( uint64_t ptr, CallstackSymbolData* result ) {
        auto& map = tracy_jit::JitCodeMap::instance();
        return map.lookup_code_address( ptr, [result] (tracy_jit::JitCodeMap::JittedCodeBlock& block, char const* module_name) -> void {
            result->symAddr = block.entrypoint;
            // we have no source file
            result->file = "[unknown]";
            result->line = 0;
            result->needFree = false;
        } );
    }
    extern bool UserDecodeCallstackPtr( uint64_t ptr, CallstackEntryData* result, CallstackEntry* cb_data ) {
        auto& map = tracy_jit::JitCodeMap::instance();
        return map.lookup_code_address( ptr, [result, cb_data] (tracy_jit::JitCodeMap::JittedCodeBlock& block, char const* module_name) -> void {
            cb_data[0].symAddr = block.entrypoint;
            cb_data[0].symLen = block.size;
            cb_data[0].name = CopyStringFast(block.name.c_str(), block.name.size());
            // we have no source file
            cb_data[0].file = CopyStringFast("[unknown]");
            cb_data[0].line = 0;

            *result = { cb_data, 1, module_name };
        } );
    }
}
#endif
