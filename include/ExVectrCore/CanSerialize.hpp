#ifndef EXVECTRCORE_CANSERIALIZE_H
#define EXVECTRCORE_CANSERIALIZE_H

#include <concepts>
#include <cstddef>
#include <cstdint>

namespace VCTR::Core {
template <typename T>
concept CanSerialize =
    requires(const T a, uint8_t *buffer, const uint8_t *readBuffer) {
      { a.numBytes() } -> std::convertible_to<size_t>;
      { a.serialize(buffer) } -> std::same_as<void>;
      { T::deserialize(readBuffer) } -> std::same_as<T>;
    };
}; // namespace VCTR::Core

#endif