#include "limitless/core/cpu_profiler.hpp"

using namespace Limitless;

CpuProfiler Limitless::global_profiler;

namespace {
	struct ThreadTraceState {
		// Top-level spans recorded since the last rotation.
		std::vector<std::unique_ptr<CpuProfiler::Span>> roots;
		// Span currently being recorded, or null when no scope is active.
		CpuProfiler::Span* current {nullptr};
	};

	thread_local ThreadTraceState tls_trace_state;
}

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

CpuProfiler::Span* CpuProfiler::Span::findChild(const char* id) noexcept {
	for (auto& child : children) {
		if (child->name == id) {
			return child.get();
		}
	}
	return nullptr;
}

void CpuProfiler::startFrame() {
	for (auto& [name, frame] : frames) {
		frame.startFrame();
	}
	maybeRotateTrace();
}

void CpuProfiler::maybeRotateTrace() {
	// Avoid swapping the trace from underneath an active scope; the scope's
	// span pointer would otherwise refer to a node now owned by previous_trace.
	if (tls_trace_state.current != nullptr) {
		return;
	}

	const auto now = std::chrono::steady_clock::now();
	if (trace_started_at == MonotonicTime {}) {
		trace_started_at = now;
		return;
	}

	if (now - trace_started_at < trace_rotation_interval) {
		return;
	}

	previous_trace.roots = std::move(tls_trace_state.roots);
	tls_trace_state.roots.clear();
	trace_started_at = now;
}

void CpuProfiler::finalizeTrace() {
	if (tls_trace_state.current != nullptr) {
		return;
	}
	if (tls_trace_state.roots.empty()) {
		return;
	}
	previous_trace.roots = std::move(tls_trace_state.roots);
	tls_trace_state.roots.clear();
}

CpuProfileScope::CpuProfileScope(CpuProfiler& profiler, const char* id) noexcept
	: frame {profiler.frames[id]}
	, parent_span {tls_trace_state.current}
	, identifier {id}
	, start {now()} {
	CpuProfiler::Span* found = nullptr;
	if (parent_span) {
		found = parent_span->findChild(id);
	} else {
		for (auto& root : tls_trace_state.roots) {
			if (root->name == id) {
				found = root.get();
				break;
			}
		}
	}

	if (!found) {
		auto owned    = std::make_unique<CpuProfiler::Span>();
		owned->name   = id;
		owned->parent = parent_span;
		found         = owned.get();
		if (parent_span) {
			parent_span->children.push_back(std::move(owned));
		} else {
			tls_trace_state.roots.push_back(std::move(owned));
		}
	}

	span                    = found;
	tls_trace_state.current = span;
}

CpuProfileScope::~CpuProfileScope() {
	const auto duration = now() - start;
	frame.record(duration);
	span->total_time += duration;
	++span->hit_count;
	tls_trace_state.current = parent_span;
}

CpuProfiler::MonotonicTime CpuProfileScope::now() noexcept {
	return std::chrono::steady_clock::now();
}
