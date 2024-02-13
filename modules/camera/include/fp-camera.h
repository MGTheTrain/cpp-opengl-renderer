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
#include <icamera.h>

namespace Mgtt::Camera {
    /**
     * @brief Implementation of the ICamera interface for a first-person camera.
     * 
     * This class provides concrete implementation details for initializing and clearing an FPS camera's state.
     */
    class FpCamera : public ICamera {
    public:
        /**
         * @brief Default constructor for the FPS camera.
         */
        FpCamera();

        /**
         * @brief Destructor for the FPS camera.
         */
        ~FpCamera();

        /**
         * @brief Initialize the FPS camera.
         * 
         * This method overrides the corresponding method in the ICamera interface.
         * It is implemented to set up the initial state specific to an FPS camera.
         */
        void Init() override;

        /**
         * @brief Clear the FPS camera's state.
         * 
         * This method overrides the corresponding method in the ICamera interface.
         * It is implemented to reset the internal state of the FPS camera, freeing resources.
         */
        void Clear() override;
    };

}