// NOTE: drop-in for the codebase, no C++23 needed
#pragma once
#include <optional>
#include <string>
#include <variant>

namespace Mgtt::Common {

namespace detail {
template <typename E>
struct ErrTag {
  E value;
};
}  // namespace detail

template <typename T, typename E = std::string>
class Result {
  std::variant<T, detail::ErrTag<E>> storage_;

  explicit Result(T val) : storage_(std::move(val)) {}
  explicit Result(detail::ErrTag<E> e) : storage_(std::move(e)) {}

 public:
  [[nodiscard]] static Result Ok(T val) { return Result(std::move(val)); }
  [[nodiscard]] static Result Err(E err) {
    return Result(detail::ErrTag<E>{std::move(err)});
  }

  [[nodiscard]] bool ok() const noexcept {
    return std::holds_alternative<T>(storage_);
  }
  [[nodiscard]] bool err() const noexcept { return !ok(); }

  [[nodiscard]] const T& value() const& { return std::get<T>(storage_); }
  [[nodiscard]] T& value() & { return std::get<T>(storage_); }
  [[nodiscard]] const E& error() const& {
    return std::get<detail::ErrTag<E>>(storage_).value;
  }
};

template <typename E>
class Result<void, E> {
  std::optional<E> err_;

  explicit Result() = default;
  explicit Result(E err) : err_(std::move(err)) {}

 public:
  [[nodiscard]] static Result Ok() { return Result{}; }
  [[nodiscard]] static Result Err(E err) { return Result(std::move(err)); }

  [[nodiscard]] bool ok() const noexcept { return !err_.has_value(); }
  [[nodiscard]] bool err() const noexcept { return err_.has_value(); }

  [[nodiscard]] const E& error() const { return *err_; }
};

}  // namespace Mgtt::Common
