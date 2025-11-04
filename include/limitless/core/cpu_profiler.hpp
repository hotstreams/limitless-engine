#pragma once

#include <chrono>
#include <unordered_map>

namespace Limitless {
	struct CpuProfiler {
		using MonotonicTime = std::chrono::steady_clock::time_point;
		using Duration      = std::chrono::steady_clock::duration;

		struct Frame {
			void record(Duration period);

			Duration getMinDuration() const noexcept;
			Duration getMaxDuration() const noexcept;
			Duration getAverageDuration() const noexcept;
			Duration getLastDuration() const noexcept;
			size_t getCount() const noexcept;

			Duration getTotalPerFrame() const noexcept;
			size_t getCountPerFrame() const noexcept;

			void startFrame();

		private:
			Duration min {Duration::max()};
			Duration max {Duration::min()};
			Duration avg {Duration::zero()};
			Duration last {Duration::zero()};
			Duration total_per_frame {Duration::zero()};
			size_t n_per_frame {0};
			size_t n {0};
		};

		std::unordered_map<const char*, Frame> frames;

		void startFrame();
	};

	extern CpuProfiler global_profiler;

	struct CpuProfileScope {
		CpuProfiler::Frame& frame;
		// Is a const char* to avoid dynamically allocating a std::sting for
		// every profile frame.
		const char* identifier;
		CpuProfiler::MonotonicTime start;

		CpuProfileScope(CpuProfiler& profiler, const char* id) noexcept;

		~CpuProfileScope();

	private:
		// TODO: consider taking a clock instance.
		static CpuProfiler::MonotonicTime now() noexcept;
	};
}
