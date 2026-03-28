#ifndef EXVECTRCORE_CANSERIALIZE_H
#define EXVECTRCORE_CANSERIALIZE_H

#include <concepts>
#include <cstddef>
#include <cstdint>

namespace VCTR::Core {
template <typename T>
concept CanSerialize =
    requires(T a, const T ca, uint8_t *buffer, const uint8_t *readBuffer) {
      { ca.numBytes() } -> std::convertible_to<size_t>;
      { ca.serialize(buffer) } -> std::same_as<void>;
      { a.deserialize(readBuffer) } -> std::same_as<bool>;
    };
}; // namespace VCTR::Core

#endif