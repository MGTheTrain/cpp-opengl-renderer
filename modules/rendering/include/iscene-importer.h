// The MIT License
//
// Copyright (c) 2026 MGTheTrain
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
#include <result.h>
#include <scene.h>

#include <string>
#include <string_view>

namespace Mgtt::Rendering {

/**
 * @brief Interface for importing 3D scenes.
 *
 * This interface defines methods for loading and clearing 3D scenes.
 */
class ISceneImporter {
 public:
  /**
   * @brief Virtual destructor.
   */
  virtual ~ISceneImporter() noexcept = default;

  // Non-copyable, non-movable interface. Concrete types decide their own
  // ownership semantics.
  ISceneImporter() = default;
  ISceneImporter(const ISceneImporter&) = delete;
  ISceneImporter& operator=(const ISceneImporter&) = delete;
  ISceneImporter(ISceneImporter&&) noexcept = default;
  ISceneImporter& operator=(ISceneImporter&&) noexcept = default;

  /**
   * @brief Load the scene from a specified file path.
   *
   * @param scene Reference to the 3D scene to populate.
   * @param path  File path from which to load the scene.
   * @return Ok on success, Err with a descriptive message on failure.
   */
  [[nodiscard]] virtual Mgtt::Common::Result<void> Load(
      Mgtt::Rendering::Scene& scene, std::string_view path) = 0;

  /**
   * @brief Release all RAM and VRAM resources held by the scene.
   *
   * @param scene Reference to the 3D scene to clear.
   */
  virtual void Clear(Mgtt::Rendering::Scene& scene) noexcept = 0;
};

}  // namespace Mgtt::Rendering
