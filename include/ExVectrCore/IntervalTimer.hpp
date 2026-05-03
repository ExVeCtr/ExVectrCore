#ifndef EXVECTRCORE_INTERVALTIMER_HPP
#define EXVECTRCORE_INTERVALTIMER_HPP

#include <cstdint>
#include <functional>

namespace VCTR::Core {

enum class EdgeType : uint8_t {
  Rising, // Transition into HIGH phase
  Falling // Transition into LOW phase
};

enum class TimerMode : uint8_t { Periodic, OneShot };

/// Policy for what happens when Update() detects that more than one full
/// period has elapsed since the last call.
enum class MissPolicy : uint8_t {
  /// Fire the callback once for each missed edge, in order.
  FireAll,
  /// Skip all missed edges; only evaluate the current local state.
  SkipMissed
};

/// A lightweight, poll-driven interval timer with duty-cycle support,
/// designed for embedded / real-time use (no allocations after construction,
/// no floating-point in the hot path).
///
/// Timing model (one period):
///
///   |<-------- Period -------->|
///   |<-- HighTime -->|         |
///   ____HIGH_________          |
///   |                |___LOW___|
///   ^                ^         ^
///   Rising edge      Falling   next Rising
///
/// The duty cycle defines the HIGH portion of each period.
/// Both edges fire the callback with the corresponding EdgeType.
class IntervalTimer {
public:
  using Callback = std::function<void(EdgeType)>;

  // ── Construction ────────────────────────────────────────────────

  IntervalTimer() = default;

  /// Construct a fully configured timer (does NOT auto-start).
  /// \param period      Cycle period in nanoseconds.
  /// \param highTime    Duration of the HIGH phase in nanoseconds.
  /// \param callback    Invoked on each edge transition.
  /// \param mode        Periodic or OneShot.
  /// \param missPolicy  How to handle missed intervals.
  IntervalTimer(int64_t period, int64_t highTime, Callback callback = nullptr,
                TimerMode mode = TimerMode::Periodic,
                MissPolicy missPolicy = MissPolicy::SkipMissed);

  // ── Lifecycle ───────────────────────────────────────────────────

  /// Start (or restart) the timer.  Sets the cycle origin to NOW().
  void start();

  /// Start the timer with an explicit origin timestamp (e.g. a packet
  /// arrival time captured earlier).
  void start(int64_t originNs);

  /// Stop the timer.  update() becomes a no-op until start() is called.
  void stop();

  /// Returns true if the timer is currently running.
  bool isRunning() const;

  // ── Core update (call from your main loop) ──────────────────────

  /// Evaluate the current time, detect edge crossings since the last
  /// update(), and fire callbacks as appropriate.
  void update();

  /// Same as update() but uses the supplied timestamp instead of NOW().
  /// Useful when you already have the time or are running in a
  /// deterministic test harness.
  void update(int64_t nowNs);

  /// Returns true if an update() call would cause the callback to fire.
  bool needUpdate() const;

  // ── Configuration (all safe to call while running) ──────────────

  /// Set the period in nanoseconds.
  void setPeriod(int64_t periodNs);
  int64_t getPeriod() const;

  /// Set the HIGH time directly in nanoseconds.
  void setHighTime(int64_t highTimeNs);
  int64_t getHighTime() const;

  /// Convenience: set duty cycle as a ratio [0.0 … 1.0].
  /// Internally converts to highTime = period * ratio.
  void setDutyCycleRatio(float ratio);

  /// Convenience: set the point within the cycle where the falling
  /// edge occurs.  E.g. for a 1 s period, setFallingEdgeOffset(800*ms)
  /// means HIGH for the first 800 ms → 80 % duty cycle.
  /// This is equivalent to setHighTime(offsetNs).
  void setFallingEdgeOffset(int64_t offsetNs);

  void setCallback(Callback callback);
  void setMode(TimerMode mode);
  void setMissPolicy(MissPolicy policy);

  // ── Synchronisation ─────────────────────────────────────────────

  /// Synchronise the timer so that the rising edge aligns with
  /// \p syncTimestampNs.  Only the internal phase offset is modified
  /// (origin is never touched).  The offset is always reduced modulo
  /// the current period, so it stays within [0, period) — never
  /// exceeding one full interval.  If the adjustment crosses an edge
  /// relative to the previous state, the callback fires on the next
  /// update().
  void sync(int64_t syncTimestampNs);

  /// @brief Calculates the phase offset from the given sync time
  int64_t calcSyncOffset(int64_t syncTimestampNs) const;

  /// Shift the phase by \p deltaNs (positive = delay, negative =
  /// advance) without modifying the origin.  The delta is accumulated
  /// in the internal phase-offset value (see getPhaseOffset()).  Same
  /// edge-crossing rules as sync().
  void adjustPhase(int64_t deltaNs);

  /// Returns the accumulated phase offset applied on top of the origin
  /// (nanoseconds).  This is the sum of all sync() / adjustPhase()
  /// calls since the last start().
  int64_t getPhaseOffset() const;

  /// Clear the accumulated phase offset back to zero.  Equivalent to
  /// calling sync(origin).
  void clearPhaseOffset();

  // ── Read-only state queries ─────────────────────────────────────

  /// True if the timer is currently in the HIGH phase of its cycle.
  bool isInHighPhase() const;

  /// True if the timer is currently in the HIGH phase at the given
  /// timestamp.
  bool isInHighPhase(int64_t nowNs) const;

  /// Nanoseconds elapsed within the current cycle [0 .. period).
  int64_t getCycleElapsed() const;
  int64_t getCycleElapsed(int64_t nowNs) const;

  /// Percent elapsed within the current cycle [0 .. 1].
  float getCycleElapsedPercent() const;

  /// Number of full cycles completed since start().
  int64_t getCompletedCycles() const;
  int64_t getCompletedCycles(int64_t nowNs) const;

  /// Returns the absolute timestamp (ns) of the start of the interval
  /// identified by \p cycleOffset relative to the current cycle.
  ///   0 → start of the current interval
  ///   1 → start of the next interval
  ///  -1 → start of the previous interval, etc.
  int64_t getIntervalStart(int32_t cycleOffset) const;
  int64_t getIntervalStart(int32_t cycleOffset, int64_t nowNs) const;

private:
  // ── Internal helpers ────────────────────────────────────────────

  /// Compute the position within a cycle for a given absolute time.
  /// Returns a value in [0, m_Period).  Handles negative offsets
  /// (time before origin) correctly.
  int64_t cyclePosition(int64_t nowNs) const;

  /// Determine the phase (HIGH / LOW) at a given cycle position.
  bool isHighAtPosition(int64_t pos) const;

  /// Core edge-detection logic shared by both update() overloads.
  void processEdges(int64_t nowNs);

  // ── Data ────────────────────────────────────────────────────────

  Callback callback;

  int64_t period = 0;       // cycle period (ns)
  int64_t highTime = 0;     // duration of HIGH phase (ns)
  int64_t origin = 0;       // absolute time of cycle start (ns)
  int64_t phaseOffset = 0;  // accumulated offset from sync/adjustPhase (ns)
  int64_t lastUpdateNs = 0; // timestamp of previous Update()

  bool wasHigh = false; // phase at previous Update()
  bool running = false;
  bool firedOneShot = false;

  TimerMode mode = TimerMode::Periodic;
  MissPolicy missPolicy = MissPolicy::SkipMissed;
};

} // namespace VCTR::Core

#endif
