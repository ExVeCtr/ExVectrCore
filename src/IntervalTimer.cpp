#include "ExVectrCore/time_definitions.hpp"

#include "ExVectrCore/IntervalTimer.hpp"

namespace VCTR::Core {

// ════════════════════════════════════════════════════════════════════
// Construction
// ════════════════════════════════════════════════════════════════════

IntervalTimer::IntervalTimer(int64_t period, int64_t highTime,
                             Callback callback, TimerMode mode,
                             MissPolicy missPolicy)
    : callback(std::move(callback)), period(period), highTime(highTime),
      mode(mode), missPolicy(missPolicy) {}

// ════════════════════════════════════════════════════════════════════
// Lifecycle
// ════════════════════════════════════════════════════════════════════

void IntervalTimer::start() { start(NOW()); }

void IntervalTimer::start(int64_t originNs) {
  origin = originNs;
  lastUpdateNs = originNs;
  wasHigh = false; // cycle starts at position 0 → rising edge pending
  running = true;
  firedOneShot = false;
}

void IntervalTimer::stop() { running = false; }

bool IntervalTimer::isRunning() const { return running; }

// ════════════════════════════════════════════════════════════════════
// Update
// ════════════════════════════════════════════════════════════════════

void IntervalTimer::update() {
  if (!running)
    return;
  update(NOW());
}

void IntervalTimer::update(int64_t nowNs) {
  if (!running)
    return;
  if (period <= 0)
    return;

  processEdges(nowNs);
  lastUpdateNs = nowNs;
}

// ════════════════════════════════════════════════════════════════════
// Configuration
// ════════════════════════════════════════════════════════════════════

void IntervalTimer::setPeriod(int64_t periodNs) {
  period = periodNs;
  // Clamp highTime to new period
  if (highTime > period)
    highTime = period;
}

int64_t IntervalTimer::getPeriod() const { return period; }

void IntervalTimer::setHighTime(int64_t highTimeNs) {
  highTime = highTimeNs;
  if (highTime > period)
    highTime = period;
  if (highTime < 0)
    highTime = 0;
}

int64_t IntervalTimer::getHighTime() const { return highTime; }

void IntervalTimer::setDutyCycleRatio(float ratio) {
  if (ratio < 0.0f)
    ratio = 0.0f;
  if (ratio > 1.0f)
    ratio = 1.0f;
  // Integer multiply then divide to stay in int64_t land.
  // ratio is only used here at configuration time, not in the hot path.
  highTime = static_cast<int64_t>(static_cast<double>(period) *
                                  static_cast<double>(ratio));
}

void IntervalTimer::setFallingEdgeOffset(int64_t offsetNs) {
  // The falling edge offset IS the high time (HIGH runs from 0 to offset).
  setHighTime(offsetNs);
}

void IntervalTimer::setCallback(Callback callback) {
  callback = std::move(callback);
}

void IntervalTimer::setMode(TimerMode mode) { mode = mode; }

void IntervalTimer::setMissPolicy(MissPolicy policy) { missPolicy = policy; }

// ════════════════════════════════════════════════════════════════════
// Synchronisation
// ════════════════════════════════════════════════════════════════════

void IntervalTimer::sync(int64_t syncTimestampNs) {
  origin = syncTimestampNs;
  // Do NOT reset m_WasHigh — the next update() will detect any edge
  // crossing caused by the phase jump and fire accordingly.
}

void IntervalTimer::adjustPhase(int64_t deltaNs) { origin += deltaNs; }

// ════════════════════════════════════════════════════════════════════
// State queries
// ════════════════════════════════════════════════════════════════════

bool IntervalTimer::isInHighPhase() const { return isInHighPhase(NOW()); }

bool IntervalTimer::isInHighPhase(int64_t nowNs) const {
  if (!running || period <= 0)
    return false;
  return isHighAtPosition(cyclePosition(nowNs));
}

int64_t IntervalTimer::getCycleElapsed() const {
  return getCycleElapsed(NOW());
}

int64_t IntervalTimer::getCycleElapsed(int64_t nowNs) const {
  if (period <= 0)
    return 0;
  return cyclePosition(nowNs);
}

int64_t IntervalTimer::getCompletedCycles() const {
  return getCompletedCycles(NOW());
}

int64_t IntervalTimer::getCompletedCycles(int64_t nowNs) const {
  if (period <= 0)
    return 0;
  int64_t elapsed = nowNs - origin;
  if (elapsed < 0)
    return 0;
  return elapsed / period;
}

// ════════════════════════════════════════════════════════════════════
// Internal helpers
// ════════════════════════════════════════════════════════════════════

int64_t IntervalTimer::cyclePosition(int64_t nowNs) const {
  int64_t elapsed = nowNs - origin;
  // Modulo that always returns a non-negative result.
  int64_t pos = elapsed % period;
  if (pos < 0)
    pos += period;
  return pos;
}

bool IntervalTimer::isHighAtPosition(int64_t pos) const {
  return pos < highTime;
}

void IntervalTimer::processEdges(int64_t nowNs) {
  if (!callback)
    return;

  // One-shot guard: if already fired and in one-shot mode, do nothing.
  if (mode == TimerMode::OneShot && firedOneShot)
    return;

  const int64_t curPos = cyclePosition(nowNs);
  const bool curHigh = isHighAtPosition(curPos);

  if (missPolicy == MissPolicy::SkipMissed) {
    // ── Simple local-only detection ─────────────────────────────
    // Only compare the phase at the previous Update() vs now.
    if (!wasHigh && curHigh) {
      callback(EdgeType::Rising);
      if (mode == TimerMode::OneShot) {
        firedOneShot = true;
        wasHigh = curHigh;
        return;
      }
    }

    if (wasHigh && !curHigh) {
      callback(EdgeType::Falling);
    }

    // Handle the case where a full cycle (or more) elapsed and we
    // crossed both edges within a single Update() window, but
    // ended up in the same phase as before.
    if (wasHigh == curHigh) {
      int64_t elapsed = nowNs - lastUpdateNs;
      if (elapsed >= period) {
        // At least one full cycle passed — fire both edges once.
        // Order: if currently high → we went high→low→high,
        //        if currently low  → we went low→high→low.
        if (curHigh) {
          callback(EdgeType::Falling);
          callback(EdgeType::Rising);
        } else {
          callback(EdgeType::Rising);
          callback(EdgeType::Falling);
        }
        if (mode == TimerMode::OneShot)
          firedOneShot = true;
      }
    }
  } else // MissPolicy::FireAll
  {
    // ── Walk through every missed edge ──────────────────────────
    // We step from the previous update time to now, finding each
    // edge crossing in chronological order.

    int64_t t = lastUpdateNs;
    bool phase = wasHigh;

    while (t < nowNs) {
      int64_t pos = cyclePosition(t);
      int64_t nextEdge;

      if (phase) {
        // Currently HIGH — next edge is the falling edge at
        // (cycle start + highTime).
        nextEdge = highTime - pos;
        if (nextEdge <= 0)
          nextEdge += period;
      } else {
        // Currently LOW — next edge is the rising edge at the
        // start of the next cycle.
        nextEdge = period - pos;
        if (nextEdge <= 0)
          nextEdge += period;
      }

      int64_t edgeTime = t + nextEdge;

      if (edgeTime > nowNs)
        break; // no more edges before now

      // Fire the edge
      if (phase) {
        callback(EdgeType::Falling);
        phase = false;
      } else {
        callback(EdgeType::Rising);
        phase = true;
        if (mode == TimerMode::OneShot) {
          firedOneShot = true;
          break;
        }
      }

      t = edgeTime;
    }
  }

  wasHigh = curHigh;
}

} // namespace VCTR::Core
