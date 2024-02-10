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
//
// Maintainers:
// - MGTheTrain 
//
// Contributors:
// - TBD

#pragma once
#include <string>

namespace Mgtt::Rendering {
    /**
     * @brief Interface for managing shaders in a 3D rendering context.
     * 
     * This interface defines methods for compiling shaders and clearing the shader state.
     */
    class IShader {
    public:
        /**
         * @brief Virtual destructor for the interface.
         */
        virtual ~IShader() {}

        /**
         * @brief Compile the shader program from specified vertex and fragment shader files.
         * 
         * This method compiles the vertex and fragment shaders, linking them into a shader program.
         * 
         * @param vsPath The file path to the vertex shader source code.
         * @param fsPath The file path to the fragment shader source code.
         */
        virtual void Compile(const std::string& vsPath, const std::string& fsPath) = 0;

        /**
         * @brief Clear the current state of the shader.
         * 
         * This method is used to reset the internal state of the shader, freeing resources.
         */
        virtual void Clear() = 0;
    };

}