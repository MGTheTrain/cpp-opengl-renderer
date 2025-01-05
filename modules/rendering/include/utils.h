#pragma once

namespace Mgtt::Rendering {
/**
 * @brief Checks if any of the given variadic arguments are greater than zero.
 * 
 * This function evaluates each argument and returns `true` if at least one is greater than zero, 
 * otherwise it returns `false`. The evaluation uses a fold expression for efficiency.
 *
 * @tparam Args Variadic template parameter pack.
 * @param args The values to be checked.
 * @return `true` if any argument is greater than zero, `false` otherwise.
 */
template <typename... Args>
bool HasValuesGreaterThanZero(Args... args) {
    return (args > 0 || ...);  // Fold expression: checks if any arg is greater than 0
}
}