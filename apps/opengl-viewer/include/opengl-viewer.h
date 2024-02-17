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
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <GL/glew.h>
#include <iostream>
#include <glfw-window.h>
#include <opengl-shader.h>
#include <gltf-scene-importer.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// To prevent linking errors, refrain from relocating the #define STB_IMAGE_IMPLEMENTATION macro to this location. It is restricted to a single usage.
// Tinygltf internally utilizes functions from the stb_image header and requires macros to be set in order to use those. 
// See: https://github.com/syoyo/tinygltf/blob/release/stb_image.h
//#include <stb_image.h>

namespace Mgtt::Apps {
    /**
 * @brief Represents window params
 */
    struct WindowParams {
        WindowParams() {
            this->name = "";
            this->width = 0.0f;
            this->height = 0.0f;
        }
        std::string name;
        float width;
        float height;
    };

    /**
     * @brief Represents glm matrices
     */
    struct GlmMatrices {
        GlmMatrices() {
            this->model = glm::mat4(1.0f);
            this->view = glm::mat4(1.0f);
            this->projection = glm::mat4(1.0f);
            this->mvp = glm::mat4(1.0f);
        }
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 mvp;
    };

    /**
     * @brief The OpenGlViewer class represents a simple OpenGL viewer.
     *
     * This class provides basic functionality for initializing an OpenGL context,
     * rendering scenes, and clearing the rendering buffer.
     */
    class OpenGlViewer {
    public:
        /**
         * @brief Constructs an OpenGlViewer object.
         */
        OpenGlViewer();

        /**
         * @brief Destructs the OpenGlViewer object.
         */
        ~OpenGlViewer();

        /**
         * @brief Clears OpenGl and RAM allocated resources
         *
         */
        void Clear();

        /**
         * @brief Renders the scene using OpenGL.
         *
         * This method is responsible for rendering the contents of the scene using OpenGL.
         */
        void Render();
    private:
        std::unique_ptr<WindowParams> windowParams;
        std::unique_ptr<GlmMatrices> glmMatrices;
        Mgtt::Rendering::Scene mgttScene;
        Mgtt::Rendering::RenderTexturesContainer renderTextureContainer;
        std::unique_ptr<Mgtt::Rendering::GltfSceneImporter> gltfSceneImporter;
        std::unique_ptr<Mgtt::Rendering::TextureManager> textureManager;
        std::unique_ptr<Mgtt::Window::GlfwWindow> glfwWindow;
    };

} // namespace Mgtt::Apps
