#pragma once

#include <limitless/core/profiler.hpp>
#include <limitless/logging/log.hpp>
#include <string>
#include <plog/Log.h>
#include <iomanip>
#include <sstream>

namespace Limitless {
    namespace ProfilerLog {
        // Convert nanoseconds to a readable string with appropriate unit (ns, μs, ms)
        inline std::string formatDuration(std::chrono::nanoseconds duration) {
            std::stringstream ss;
            double value = duration.count();
            
            if (value < 1000.0) {
                ss << std::fixed << std::setprecision(2) << value << " ns";
            } else if (value < 1000000.0) {
                ss << std::fixed << std::setprecision(2) << (value / 1000.0) << " μs";
            } else {
                ss << std::fixed << std::setprecision(2) << (value / 1000000.0) << " ms";
            }
            
            return ss.str();
        }
        
        // Log GPU profiler data
        inline void logGPUProfilerData(plog::Severity severity = plog::info) {
            PLOG(severity) << "--------- GPU Profiler Report ---------";
            
            if (global_gpu_profiler.frames.empty()) {
                PLOG(severity) << "No GPU profiling data available.";
            } else {
                PLOG(severity) << std::left << std::setw(40) << "Profile Name" 
                              << std::setw(15) << "Avg" 
                              << std::setw(15) << "Min" 
                              << std::setw(15) << "Max"
                              << "Samples";
                              
                for (const auto& [name, frame] : global_gpu_profiler.frames) {
                    PLOG(severity) << std::left << std::setw(40) << name 
                                  << std::setw(15) << formatDuration(frame.getAverageDuration())
                                  << std::setw(15) << formatDuration(frame.getMinDuration())
                                  << std::setw(15) << formatDuration(frame.getMaxDuration())
                                  << frame.getCount();
                }
            }
            PLOG(severity) << "--------------------------------------";
        }
        
        // Log CPU profiler data
        inline void logCPUProfilerData(plog::Severity severity = plog::info) {
            PLOG(severity) << "--------- CPU Profiler Report ---------";
            
            if (global_cpu_profiler.frames.empty()) {
                PLOG(severity) << "No CPU profiling data available.";
            } else {
                PLOG(severity) << std::left << std::setw(40) << "Profile Name" 
                              << std::setw(15) << "Avg" 
                              << std::setw(15) << "Min" 
                              << std::setw(15) << "Max"
                              << "Samples";
                              
                for (const auto& [name, frame] : global_cpu_profiler.frames) {
                    PLOG(severity) << std::left << std::setw(40) << name 
                                  << std::setw(15) << formatDuration(frame.getAverageDuration())
                                  << std::setw(15) << formatDuration(frame.getMinDuration())
                                  << std::setw(15) << formatDuration(frame.getMaxDuration())
                                  << frame.getCount();
                }
            }
            PLOG(severity) << "--------------------------------------";
        }
        
        // Log both GPU and CPU profiler data
        inline void logAllProfilerData(plog::Severity severity = plog::info) {
            PLOG(severity) << "\n========= PERFORMANCE PROFILER REPORT =========";
            logGPUProfilerData(severity);
            logCPUProfilerData(severity);
            PLOG(severity) << "==============================================\n";
        }
    }
} 