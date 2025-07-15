#include "limitless/core/cpu_profiler.hpp"

using namespace Limitless;

CpuProfiler Limitless::global_profiler;

void CpuProfiler::Frame::record(Duration period) {
	if (period < min) {
		min = period;
	}
	if (period > max) {
		max = period;
	}

	avg = decltype(avg)((avg.count() * n + period.count()) / (n + 1));
	last = period;
	++n;
	total_per_frame += period;
	++n_per_frame;
}

CpuProfiler::Duration CpuProfiler::Frame::getMinDuration() const noexcept {
	return min;
}

CpuProfiler::Duration CpuProfiler::Frame::getMaxDuration() const noexcept {
	return max;
}

CpuProfiler::Duration CpuProfiler::Frame::getAverageDuration() const noexcept {
	return avg;
}

CpuProfiler::Duration CpuProfiler::Frame::getLastDuration() const noexcept {
	return last;
}

size_t CpuProfiler::Frame::getCount() const noexcept {
	return n;
}

CpuProfiler::Duration CpuProfiler::Frame::getTotalPerFrame() const noexcept {
	return total_per_frame;
}

size_t CpuProfiler::Frame::getCountPerFrame() const noexcept {
	return n_per_frame;
}

void CpuProfiler::Frame::startFrame() {
	total_per_frame = Duration::zero();
	n_per_frame = 0;
}

void CpuProfiler::startFrame() {
	for (auto& [name, frame] : frames) {
		frame.startFrame();
	}
}

CpuProfileScope::CpuProfileScope(CpuProfiler& profiler, const char* id) noexcept
	: frame {profiler.frames[id]}
	, identifier {id}
	, start {now()} {
}

CpuProfileScope::~CpuProfileScope() {
	frame.record(now() - start);
}

CpuProfiler::MonotonicTime CpuProfileScope::now() noexcept {
	return std::chrono::steady_clock::now();
}
