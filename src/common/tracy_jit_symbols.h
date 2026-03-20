#pragma once
#include <cstdint>
#include <cassert>
#include <string>
#include <string_view>

/*
    Warning: This allows tracy to show jitted code blocks in its Statistics (Sampling) view and source view (including disassembly)
    Will only show up in Statistics (Sampling) view with "Hide unknown" unchecked, as the source code is not known

    But the jitted code does not support stack traces, at least not with windows event tracing
    This means that while some jitted blocks show up, I believe it is only the ones that happen to be at the top of the stack at the time of the stample (as seen in the ghost flamegraph)
    It appears that dynarmic supports stacktraces via an API, but windows ETW does not know about this, and merging ETW results after the fact might not be possible...
*/

#if defined(TRACY_ENABLE) && defined(TRACY_HAS_USER_SYMBOLS)
#include <map>
#include <mutex>

namespace tracy_jit {

class JitCodeMap {
public:
    static_assert(sizeof(const void*) == sizeof(uint64_t));
    static_assert(sizeof(size_t) == sizeof(uint64_t));
    struct JittedCodeBlock {
        // friendly name already provided by dynarmic
        std::string name;
        uint64_t entrypoint;
        uint32_t size;
    };

    //// registration functions called by dynarmic EmitX64
    // which appears to execute on CPU threads, so this needs a mutex to be threadsafe

    // TODO: copy block code RegisterBlock and never unregister?
    // because tracy copies code way after emulation has already stopped and code may already have been freed

    void _RegisterBlock (const void* entrypoint, size_t size, std::string&& name) {
        std::unique_lock lock{mutex};
        sorted_blocks.emplace((uint64_t)entrypoint, JittedCodeBlock{ std::move(name), (uint64_t)entrypoint, (uint32_t)size });
    }
    void _UnregisterBlock (const void* entrypoint) {
        std::unique_lock lock{mutex};
        sorted_blocks.erase((uint64_t)entrypoint);
    }
    void _ClearAllBlocks () {
        std::unique_lock lock{mutex};
        sorted_blocks.clear();
    }
    static void RegisterBlock (const void* entrypoint, size_t size, std::string&& name) {
        instance()._RegisterBlock(entrypoint, size, std::move(name));
    }
    static void UnregisterBlock (const void* entrypoint) {
        instance()._UnregisterBlock(entrypoint);
    }
    static void ClearAllBlocks () {
        instance()._ClearAllBlocks();
    }

    // Currently we do a binary search to lookup any tracy::Decode* even if the queried addres is not part of any jitted code
    // it is likely that all jitted code lives in a single or a few ranges of addresses managed by dynarmic
    // if so we really should do a quick check against those ranges first to speed up these cases
    // TODO: investigate where jitted code blocks are allocated from

    // called by tracy symbol worker thread -> TracyCallstack.cpp
    template <typename FUNC>
    bool lookup_code_address (uint64_t ptr, FUNC set_result) {
        std::unique_lock lock{mutex};

        // Use upper bound: upper bound = find first entry from sorted list/tree where lookup_ptr < sorted_entry_key
        // normally [lower_bound, upper_bound) would be the range of entries with equal key, but we need to find the entry with lower or equal address
        auto it = sorted_blocks.upper_bound(ptr);
        if (it == sorted_blocks.begin()) { // ptr lower than first block, no match
            return false;
        }
        it--;

        assert(ptr >= it->second.entrypoint);
        if (ptr - it->second.entrypoint < it->second.size) {
            set_result(it->second, exe_name.c_str());
            return true;
        }
        return false;
    }

    // Make registration API done via singleton since jitting code is too complex to pass this around cleanly
    // use function static since other way of doing singletons may not be safe with how static libaries are used in this project (?)
    static JitCodeMap& instance () {
        // Leak intentionally, as apparently c++ destructs classes when main exits, despite other threads still running?
        static JitCodeMap* inst = new JitCodeMap();
        return *inst;
    }

private:
    // sorted map of currently active jitted code blocks
    std::map<uint64_t, JittedCodeBlock> sorted_blocks;
    std::mutex mutex;
    std::string exe_name;

    static std::string get_exe_name ();

    JitCodeMap () {
        exe_name = get_exe_name();
    }
};

}
#else
namespace tracy_jit {
class JitCodeMap {
public:
    static void RegisterBlock (const void* entrypoint, size_t size, std::string&& name) {}
    static void UnregisterBlock (const void* entrypoint) {}
    static void ClearAllBlocks () {}
};
}
#endif
