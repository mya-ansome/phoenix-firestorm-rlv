/**
 * @file llprofiler.h
 * @brief Wrapper for Tracy and/or other profilers
 *
 * $LicenseInfo:firstyear=2021&license=viewerlgpl$
 * Second Life Viewer Source Code
 * Copyright (C) 2021, Linden Research, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * version 2.1 of the License only.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Linden Research, Inc., 945 Battery Street, San Francisco, CA  94111  USA
 * $/LicenseInfo$
 */

#ifndef LL_PROFILER_H
#define LL_PROFILER_H

#define LL_PROFILER_CONFIG_NONE             0  // No profiling
#define LL_PROFILER_CONFIG_FAST_TIMER       1  // Profiling on: Only Fast Timers
#define LL_PROFILER_CONFIG_TRACY            2  // Profiling on: Only Tracy
#define LL_PROFILER_CONFIG_TRACY_FAST_TIMER 3  // Profiling on: Fast Timers + Tracy

#ifndef LL_PROFILER_CONFIGURATION
#define LL_PROFILER_CONFIGURATION           LL_PROFILER_CONFIG_FAST_TIMER
#endif

// <FS:Beq> active switch for deferred profiling
namespace LLProfiler
{
    extern bool active;
}
// </FS:Beq>
#if defined(LL_PROFILER_CONFIGURATION) && (LL_PROFILER_CONFIGURATION > LL_PROFILER_CONFIG_NONE)
    #if LL_PROFILER_CONFIGURATION == LL_PROFILER_CONFIG_TRACY || LL_PROFILER_CONFIGURATION == LL_PROFILER_CONFIG_TRACY_FAST_TIMER
        #define TRACY_ENABLE         1
// Normally these would be enabled but we want to be able to build any viewer with Tracy enabled and run the Tracy server on another machine
// They must be undefined in order to work across multiple machines
//      #define TRACY_NO_BROADCAST   1
//      #define TRACY_ONLY_LOCALHOST 1
        #define TRACY_ONLY_IPV4      1
        #include "Tracy.hpp"
// <FS:Beq> Fixed mutual exclusion issues with RAM and GPU. NOTE: This might still break on Apple in which case we'll need to restrict that platform
        //// GPU Mutually exclusive with detailed memory tracing
        // #define LL_PROFILER_ENABLE_TRACY_OPENGL 0
    #define LL_PROFILER_ENABLE_TRACY_MEMORY 1
    #define LL_PROFILER_ENABLE_TRACY_OPENGL 1
    #endif

    #if LL_PROFILER_CONFIGURATION == LL_PROFILER_CONFIG_TRACY
        #define LL_PROFILER_FRAME_END                   FrameMark
        #define LL_PROFILER_SET_THREAD_NAME( name )     tracy::SetThreadName( name )
        #define LL_PROFILER_THREAD_BEGIN(name)          FrameMarkStart( name ) // C string
        #define LL_PROFILER_THREAD_END(name)            FrameMarkEnd( name )   // C string
        // <FS:Beq> revert change that obscures custom FTM zones. We may want to may FTM Zones unique in future.
        // #define LL_RECORD_BLOCK_TIME(name)              ZoneScoped // Want descriptive names; was: ZoneNamedN( ___tracy_scoped_zone, #name, LLProfiler::active );
        #define LL_RECORD_BLOCK_TIME(name)              ZoneNamedN( ___tracy_scoped_zone, #name, LLProfiler::active )
        // </FS:Beq>

        // <FS:Beq>
        // #define LL_PROFILE_ZONE_NAMED(name)             ZoneNamedN( ___tracy_scoped_zone, name, true )
        // #define LL_PROFILE_ZONE_NAMED_COLOR(name,color) ZoneNamedNC( ___tracy_scopped_zone, name, color, true ) // RGB
        // #define LL_PROFILE_ZONE_SCOPED                  ZoneScoped
        #define LL_PROFILE_ZONE_NAMED(name)             ZoneNamedN( ___tracy_scoped_zone, name, LLProfiler::active )
        #define LL_PROFILE_ZONE_NAMED_COLOR(name,color) ZoneNamedNC( ___tracy_scopped_zone, name, color, LLProfiler::active ) // RGB
        #define LL_PROFILE_ZONE_SCOPED                  ZoneNamed( ___tracy_scoped_zone, LLProfiler::active ) // <FS:Beq/> Enable deferred collection through filters
        // </FS:Beq>

        #define LL_PROFILE_ZONE_NUM( val )              ZoneValue( val )
        #define LL_PROFILE_ZONE_TEXT( text, size )      ZoneText( text, size )

        #define LL_PROFILE_ZONE_ERR(name)               LL_PROFILE_ZONE_NAMED_COLOR( name, 0XFF0000  )  // RGB yellow
        #define LL_PROFILE_ZONE_INFO(name)              LL_PROFILE_ZONE_NAMED_COLOR( name, 0X00FFFF  )  // RGB cyan
        #define LL_PROFILE_ZONE_WARN(name)              LL_PROFILE_ZONE_NAMED_COLOR( name, 0x0FFFF00 )  // RGB red

        // <FS:Beq> Additional FS Tracy macros
        #define LL_PROFILE_ZONE_COLOR(color)            ZoneNamedC( ___tracy_scoped_zone, color, LLProfiler::active ) // <FS:Beq/> Additional Tracy macro
        #define LL_PROFILE_PLOT( name, value )          TracyPlot( name, value)
        #define LL_PROFILE_PLOT_SQ( name, prev, value ) TracyPlot(name,prev);TracyPlot( name, value)
        #define LL_PROFILE_IS_CONNECTED                 TracyIsConnected
        // </FS:Beq>
    #endif
    #if LL_PROFILER_CONFIGURATION == LL_PROFILER_CONFIG_FAST_TIMER
        #define LL_PROFILER_FRAME_END
        #define LL_PROFILER_SET_THREAD_NAME( name )     (void)(name);
        #define LL_PROFILER_THREAD_BEGIN(name)          (void)(name); // Not supported
        #define LL_PROFILER_THREAD_END(name)            (void)(name); // Not supported

        #define LL_RECORD_BLOCK_TIME(name)                                                                  const LLTrace::BlockTimer& LL_GLUE_TOKENS(block_time_recorder, __LINE__)(LLTrace::timeThisBlock(name)); (void)LL_GLUE_TOKENS(block_time_recorder, __LINE__);
        #define LL_PROFILE_ZONE_NAMED(name)             // LL_PROFILE_ZONE_NAMED is a no-op when Tracy is disabled
        #define LL_PROFILE_ZONE_NAMED_COLOR(name,color) // LL_RECORD_BLOCK_TIME(name)
        #define LL_PROFILE_ZONE_SCOPED                  // LL_PROFILE_ZONE_SCOPED is a no-op when Tracy is disabled

        #define LL_PROFILE_ZONE_NUM( val )              (void)( val );                // Not supported
        #define LL_PROFILE_ZONE_TEXT( text, size )      (void)( text ); void( size ); // Not supported

        #define LL_PROFILE_ZONE_ERR(name)               (void)(name); // Not supported
        #define LL_PROFILE_ZONE_INFO(name)              (void)(name); // Not supported
        #define LL_PROFILE_ZONE_WARN(name)              (void)(name); // Not supported
        // <FS:Beq> Additional FS Tracy macros
        #define LL_PROFILE_ZONE_COLOR(color)
        #define LL_PROFILE_PLOT( name, value )
        #define LL_PROFILE_PLOT_SQ( name, prev, value )
        #define LL_PROFILE_IS_CONNECTED
        // </FS:Beq>
    #endif
    #if LL_PROFILER_CONFIGURATION == LL_PROFILER_CONFIG_TRACY_FAST_TIMER
        #define LL_PROFILER_FRAME_END                   FrameMark
        #define LL_PROFILER_SET_THREAD_NAME( name )     tracy::SetThreadName( name )
        #define LL_PROFILER_THREAD_BEGIN(name)          FrameMarkStart( name ) // C string
        #define LL_PROFILER_THREAD_END(name)            FrameMarkEnd( name )   // C string

        // <FS:Beq> revert change that obscures custom FTM zones.
        // #define LL_RECORD_BLOCK_TIME(name)              ZoneScoped                                          const LLTrace::BlockTimer& LL_GLUE_TOKENS(block_time_recorder, __LINE__)(LLTrace::timeThisBlock(name)); (void)LL_GLUE_TOKENS(block_time_recorder, __LINE__);
        #define LL_RECORD_BLOCK_TIME(name)              ZoneNamedN( ___tracy_scoped_zone, #name, LLProfiler::active );    const LLTrace::BlockTimer& LL_GLUE_TOKENS(block_time_recorder, __LINE__)(LLTrace::timeThisBlock(name)); (void)LL_GLUE_TOKENS(block_time_recorder, __LINE__);
        // </FS:Beq>
        // <FS:Beq>
        // #define LL_PROFILE_ZONE_NAMED(name)             ZoneNamedN( ___tracy_scoped_zone, name, true )
        // #define LL_PROFILE_ZONE_NAMED_COLOR(name,color) ZoneNamedNC( ___tracy_scopped_zone, name, color, true ) // RGB
        // #define LL_PROFILE_ZONE_SCOPED                  ZoneScoped
        #define LL_PROFILE_ZONE_NAMED(name)             ZoneNamedN( ___tracy_scoped_zone, name, LLProfiler::active );
        #define LL_PROFILE_ZONE_NAMED_COLOR(name,color) ZoneNamedNC( ___tracy_scopped_zone, name, color, LLProfiler::active ) // RGB
        #define LL_PROFILE_ZONE_SCOPED                  ZoneNamed( ___tracy_scoped_zone, LLProfiler::active ) // <FS:Beq/> Enable deferred collection through filters
        // </FS:Beq>

        #define LL_PROFILE_ZONE_NUM( val )              ZoneValue( val )
        #define LL_PROFILE_ZONE_TEXT( text, size )      ZoneText( text, size )

        #define LL_PROFILE_ZONE_ERR(name)               LL_PROFILE_ZONE_NAMED_COLOR( name, 0XFF0000  )  // RGB yellow
        #define LL_PROFILE_ZONE_INFO(name)              LL_PROFILE_ZONE_NAMED_COLOR( name, 0X00FFFF  )  // RGB cyan
        #define LL_PROFILE_ZONE_WARN(name)              LL_PROFILE_ZONE_NAMED_COLOR( name, 0x0FFFF00 )  // RGB red
        // <FS:Beq> Additional FS Tracy macros
        #define LL_PROFILE_ZONE_COLOR(color)            ZoneNamedC( ___tracy_scoped_zone, color, LLProfiler::active )
        #define LL_PROFILE_PLOT( name, value )          TracyPlot( name, value)
        #define LL_PROFILE_PLOT_SQ( name, prev, value ) TracyPlot( name, prev );TracyPlot( name, value )
        #define LL_PROFILE_IS_CONNECTED                 TracyIsConnected
        // </FS:Beq>
    #endif
#else
    #define LL_PROFILER_FRAME_END
    #define LL_PROFILER_SET_THREAD_NAME(name)
    #define LL_PROFILER_THREAD_BEGIN(name)
    #define LL_PROFILER_THREAD_END(name)

    #define LL_RECORD_BLOCK_TIME(name)
    #define LL_PROFILE_ZONE_NAMED(name)
    #define LL_PROFILE_ZONE_NAMED_COLOR(name,color)
    #define LL_PROFILE_ZONE_SCOPED

    #define LL_PROFILE_ZONE_NUM(val)
    #define LL_PROFILE_ZONE_TEXT(text, size)

    #define LL_PROFILE_ZONE_ERR(name)
    #define LL_PROFILE_ZONE_INFO(name)
    #define LL_PROFILE_ZONE_WARN(name)

    // <FS:Ansariel> Additional FS Tracy macros
    #define LL_PROFILE_ZONE_COLOR(color)
    #define LL_PROFILE_PLOT( name, value )
    #define LL_PROFILE_PLOT_SQ( name, prev, value )
    #define LL_PROFILE_IS_CONNECTED
    // </FS:Ansariel>
#endif // LL_PROFILER

#endif // LL_PROFILER_H
