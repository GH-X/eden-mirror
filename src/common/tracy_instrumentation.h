// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

// Add our own tracy instrumentation macros which compile to nothing without TRACY_ENABLE
// tracys macros alread do that but do require you to always include the full tracy code even without TRACY_ENABLE
// Doing this means any instrumentation in the project keeps compiling even with ENABLE_TRACY=0 in cmake, meaning most devs can compile without even pulling tracy at all
// Additionally These macros may be more readable than the badly named "ZoneScoped", and we could easily add levels or categories of instrumentation that can be made selectable
// Unless there is already a tracing framework that is configurable...?
#if TRACY_ENABLE
#include <tracy/Tracy.hpp>

#define TRACY_ZONE_SCOPED ZoneScoped;
#define TRACY_ZONE_SCOPEDN(name) ZoneScopedN(name);
#define TRACY_ZONE_SCOPEDVN(var, name) ZoneScopedVN(var, name);
#define TRACY_ZONE_NAMEDN(var, name) ZoneNamedN(var, name, true);
#define TRACY_MSG_C(text, col) TracyMessageC(text, strlen(text), col);
#else
#define TRACY_ZONE_SCOPED
#define TRACY_ZONE_SCOPEDN(name)
#define TRACY_ZONE_SCOPEDVN(var, name)
#define TRACY_ZONE_NAMEDN(var, name)
#define TRACY_MSG_C(text, col)
#endif
