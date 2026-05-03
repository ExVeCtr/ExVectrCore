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
  phaseOffset = 0;
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

bool IntervalTimer::needUpdate() const {
  if (!running)
    return false;
  if (period <= 0)
    return false;

  const int64_t nowNs = NOW();
  const int64_t elapsed = nowNs - lastUpdateNs;
  return elapsed >= period || isHighAtPosition(cyclePosition(nowNs)) != wasHigh;
}

// ════════════════════════════════════════════════════════════════════
// Configuration
// ════════════════════════════════════════════════════════════════════

void IntervalTimer::setPeriod(int64_t periodNs) {
  period = periodNs;
  // Clamp highTime to new period
  if (highTime > period)
    highTime = period;

  // Avoid artificial edge detection after runtime reconfiguration.
  if (running && period > 0) {
    const int64_t nowNs = NOW();
    wasHigh = isHighAtPosition(cyclePosition(nowNs));
    lastUpdateNs = nowNs;
  }
}

int64_t IntervalTimer::getPeriod() const { return period; }

void IntervalTimer::setHighTime(int64_t highTimeNs) {
  highTime = highTimeNs;
  if (highTime > period)
    highTime = period;
  if (highTime < 0)
    highTime = 0;

  // Avoid artificial edge detection after runtime reconfiguration.
  if (running && period > 0) {
    const int64_t nowNs = NOW();
    wasHigh = isHighAtPosition(cyclePosition(nowNs));
    lastUpdateNs = nowNs;
  }
}

int64_t IntervalTimer::getHighTime() const { return highTime; }

void IntervalTimer::setDutyCycleRatio(float ratio) {
  if (ratio < 0.0f)
    ratio = 0.0f;
  if (ratio > 1.0f)
    ratio = 1.0f;
  // Delegate to setHighTime so that wasHigh is recalculated when the
  // timer is already running, avoiding phantom edge detection.
  setHighTime(static_cast<int64_t>(static_cast<double>(period) *
                                   static_cast<double>(ratio)));
}

void IntervalTimer::setFallingEdgeOffset(int64_t offsetNs) {
  // The falling edge offset IS the high time (HIGH runs from 0 to offset).
  setHighTime(offsetNs);
}

void IntervalTimer::setCallback(Callback callback) {
  this->callback = std::move(callback);
}

void IntervalTimer::setMode(TimerMode mode) { this->mode = mode; }

void IntervalTimer::setMissPolicy(MissPolicy policy) {
  this->missPolicy = policy;
}

// ════════════════════════════════════════════════════════════════════
// Synchronisation
// ════════════════════════════════════════════════════════════════════

void IntervalTimer::sync(int64_t syncTimestampNs) {
  if (period <= 0)
    return;
  phaseOffset = calcSyncOffset(syncTimestampNs);
  // Do NOT reset wasHigh — the next update() will detect any edge
  // crossing caused by the phase adjustment and fire accordingly.
}

int64_t IntervalTimer::calcSyncOffset(int64_t syncTimestampNs) const {
  if (period <= 0)
    return 0;
  // Compute the phase offset that places a rising edge exactly at
  // syncTimestampNs.  We reduce modulo period so phaseOffset always
  // stays within [0, period) — never exceeding one full interval.
  int64_t raw = (syncTimestampNs - origin) % period;
  if (raw < 0)
    raw += period;
  return raw;
}

void IntervalTimer::adjustPhase(int64_t deltaNs) { phaseOffset += deltaNs; }

int64_t IntervalTimer::getPhaseOffset() const { return phaseOffset; }

void IntervalTimer::clearPhaseOffset() { phaseOffset = 0; }

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

float IntervalTimer::getCycleElapsedPercent() const {
  if (period <= 0)
    return 0.0f;
  return static_cast<float>(getCycleElapsed()) / static_cast<float>(period);
}

int64_t IntervalTimer::getCompletedCycles() const {
  return getCompletedCycles(NOW());
}

int64_t IntervalTimer::getCompletedCycles(int64_t nowNs) const {
  if (period <= 0)
    return 0;
  const int64_t effectiveOrigin = origin + phaseOffset;
  int64_t elapsed = nowNs - effectiveOrigin;
  if (elapsed < 0)
    return 0;
  return elapsed / period;
}

int64_t IntervalTimer::getIntervalStart(int32_t cycleOffset) const {
  return getIntervalStart(cycleOffset, NOW());
}

int64_t IntervalTimer::getIntervalStart(int32_t cycleOffset,
                                        int64_t nowNs) const {
  const int64_t effectiveOrigin = origin + phaseOffset;
  if (period <= 0)
    return effectiveOrigin;
  // Floor-divide elapsed time to find the index of the current cycle,
  // then offset by the requested amount.
  int64_t elapsed = nowNs - effectiveOrigin;
  // Floor division that works for negative elapsed too.
  int64_t currentCycle = elapsed / period;
  if (elapsed < 0 && (elapsed % period) != 0)
    --currentCycle;
  return effectiveOrigin + (currentCycle + cycleOffset) * period;
}

// ════════════════════════════════════════════════════════════════════
// Internal helpers
// ════════════════════════════════════════════════════════════════════

int64_t IntervalTimer::cyclePosition(int64_t nowNs) const {
  const int64_t effectiveOrigin = origin + phaseOffset;
  int64_t elapsed = nowNs - effectiveOrigin;
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
        wasHigh = isHighAtPosition(cyclePosition(nowNs));
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

  // Re-evaluate with current settings in case callback changed duty/period.
  wasHigh = isHighAtPosition(cyclePosition(nowNs));
}

} // namespace VCTR::Core
