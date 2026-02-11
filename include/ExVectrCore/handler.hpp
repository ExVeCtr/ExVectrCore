#ifndef EXVECTRCORE_HANDLER_HPP
#define EXVECTRCORE_HANDLER_HPP

#include "stddef.h"
#include "stdint.h"
#include <functional>

#include "list_array.hpp"

namespace VCTR::Core {

/**
 * @brief Base class for handlers. Offers a callback function to be called when
 * the handler is triggered.
 */
template <typename... Args> class HandlerGroup {
public:
  using HandlerFunction = std::function<void(Args...)>;

  /**
   * @brief Adds a handler to the group.
   * @param handler The handler function to be added.
   */
  void addHandler(HandlerFunction handler) { handlers_.append(handler); }

  /**
   * @brief Calls all handlers in the group with the given arguments.
   * @param args The arguments to be passed to the handlers.
   */
  void callHandlers(Args... args) {
    for (size_t i = 0; i < handlers_.size(); i++) {
      handlers_[i](args...);
    }
  }

  /**
   * @brief Clears all handlers from the group.
   */
  void clearHandlers() {
    for (size_t i = 0; i < handlers_.size(); i++) {
      handlers_[i] = nullptr;
    }
    handlers_.clear();
  }

private:
  ListArray<HandlerFunction> handlers_;
};

} // namespace VCTR::Core

#endif
