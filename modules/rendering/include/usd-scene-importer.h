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
#ifndef __EMSCRIPTEN__

#include <iscene-importer.h>

#include <string>
#include <string_view>
#include <tinyusdz.hh>
#include <tydra/render-data.hh>
#include <tydra/scene-access.hh>

namespace Mgtt::Rendering {

/**
 * @brief Concrete ISceneImporter for loading USD / USDA / USDC / USDZ scenes.
 *
 * CPU-side only. Parses the USD stage via tinyusdz and converts it to the
 * Mgtt::Rendering::Scene representation using the Tydra RenderScene API.
 * No GL calls are made. Call SceneUploader::Upload() afterwards.
 */
class UsdSceneImporter : public ISceneImporter {
 public:
  UsdSceneImporter() = default;
  ~UsdSceneImporter() noexcept = default;

  UsdSceneImporter(const UsdSceneImporter&) = delete;
  UsdSceneImporter& operator=(const UsdSceneImporter&) = delete;
  UsdSceneImporter(UsdSceneImporter&&) noexcept = default;
  UsdSceneImporter& operator=(UsdSceneImporter&&) noexcept = default;

  [[nodiscard]] Mgtt::Common::Result<void> Load(Mgtt::Rendering::Scene& scene,
                                                std::string_view path) override;

  void Clear(Mgtt::Rendering::Scene& scene) noexcept override;

 private:
  [[nodiscard]] Mgtt::Common::Result<void> LoadTextures(
      Mgtt::Rendering::Scene& scene,
      const tinyusdz::tydra::RenderScene& renderScene);

  void LoadMaterials(Mgtt::Rendering::Scene& scene,
                     const tinyusdz::tydra::RenderScene& renderScene);

  [[nodiscard]] Mgtt::Common::Result<void> LoadMeshes(
      Mgtt::Rendering::Scene& scene,
      const tinyusdz::tydra::RenderScene& renderScene);
};

}  // namespace Mgtt::Rendering
#endif
