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

namespace Mgtt::Camera {
    /**
     * @brief Interface for defining a generic camera.
     * 
     * This interface outlines methods for initializing and clearing a camera's state.
     */
    class ICamera {
    public:
        /**
         * @brief Virtual destructor for the camera interface.
         */
        virtual ~ICamera() {}

        /**
         * @brief Initialize the camera.
         * 
         * This method should be implemented to set up the initial state of the camera.
         */
        virtual void Init() = 0;

        /**
         * @brief Clear the camera's state.
         * 
         * This method should be implemented to reset the internal state of the camera, freeing resources.
         */
        virtual void Clear() = 0;
    };
}