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
#include <string>
#include <utility>

namespace Mgtt::Rendering {
/**
 * @brief Interface for managing shaders in a 3D rendering context.
 *
 */
class IShader {
 public:
  /**
   * @brief Virtual destructor
   */
  virtual ~IShader() = default;

  /**
   * @brief Compile the shader program from specified vertex and fragment shader
   * files.
   *
   * @param shaderPaths The vertex and fragment shader paths
   */
  virtual void Compile(
      const std::pair<std::string, std::string>& shaderPaths) = 0;

  /**
   * @brief Delete the shader program.
   */
  virtual void Clear() = 0;
};

}  // namespace Mgtt::Rendering
