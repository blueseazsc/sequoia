#pragma once

#ifdef UTIL_TRACE_ENABLE

#include "Tracy.hpp"

#define T_STARTUP tracy::StartupProfiler()
#define T_SHUTDOWN tracy::ShutdownProfiler()

#define T_SCOPED ZoneScoped
#define T_SCOPED_NAME(__NAME__) ZoneScopedN(__NAME__)

#define T_FRAME_MARK FrameMark
#define T_FRAME_MARK_NAME(__NAME__) FrameMarkNamed(__NAME__)

#else

#define T_STARTUP
#define T_SHUTDOWN

#define T_SCOPED
#define T_SCOPED_NAME(__NAME__)

#define T_FRAME_MARK
#define T_FRAME_MARK_NAME(__NAME__)

#endif
