// The MIT License
//
// Copyright (c) 2024 MGTheTrain
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <cstdint>
#include <initializer_list>

namespace Mgtt::Rendering {
/**
 * @brief Checks if any value in the provided initializer list is greater than zero.
 *
 * This method iterates through the elements in the initializer list and returns
 * true if at least one element is greater than zero; otherwise, it returns false.
 *
 * @param args A constant reference to an initializer list of unsigned integers to be checked.
 * @return true if any value in the list is greater than zero, false otherwise.
 */
inline bool HasValuesGreaterThanZero(const std::initializer_list<uint32_t>& args) {
    for (auto& val : args) {
        if (val > 0) {
            return true;
        }
    }
    return false;
}
}