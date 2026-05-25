#pragma once

#include <chrono>
#include <memory>
#include <unordered_map>
#include <vector>

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

		// A node in the per-thread profile trace tree. Each Span is uniquely
		// identified within its parent by its name; entering a CpuProfileScope
		// whose parent already has a child with the same name updates the
		// existing Span instead of creating a new one. This keeps the trace
		// compact when scopes execute in tight loops.
		struct Span {
			const char* name {nullptr};
			Duration total_time {Duration::zero()};
			size_t hit_count {0};
			std::vector<std::unique_ptr<Span>> children {};
			Span* parent {nullptr};

			Span() = default;
			Span(const Span&)            = delete;
			Span& operator=(const Span&) = delete;
			Span(Span&&)                 = default;
			Span& operator=(Span&&)      = default;

			Span* findChild(const char* id) noexcept;
		};

		struct Trace {
			std::vector<std::unique_ptr<Span>> roots {};
		};

		std::unordered_map<const char*, Frame> frames;

		// Most recently completed trace snapshot from the recording thread.
		// Updated by maybeRotateTrace() (typically called by startFrame).
		Trace previous_trace {};
		MonotonicTime trace_started_at {};
		Duration trace_rotation_interval {std::chrono::seconds(5)};

		void startFrame();

		// Rotates the recording thread's accumulated trace into `previous_trace`
		// when the rotation interval has elapsed and no scope is currently active.
		// Must be called on the thread that records CpuProfileScopes.
		void maybeRotateTrace();

		// Moves any accumulated trace on the recording thread into `previous_trace`.
		// Must be called on the thread that records CpuProfileScopes.
		void finalizeTrace();

		[[nodiscard]] const Trace& getPreviousTrace() const noexcept { return previous_trace; }
	};

	extern CpuProfiler global_profiler;

	struct CpuProfileScope {
		CpuProfiler::Frame& frame;
		CpuProfiler::Span* span;
		CpuProfiler::Span* parent_span;
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
