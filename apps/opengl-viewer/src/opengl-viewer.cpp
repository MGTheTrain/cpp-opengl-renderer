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

#include <opengl-viewer.h>

/**
 * @brief Destructs the OpenGlViewer object.
 */
Mgtt::Apps::OpenGlViewer::~OpenGlViewer() {
    std::cout << "Hi from destructor" << std::endl;
}

/**
 * @brief Constructs an OpenGlViewer object.
 */
Mgtt::Apps::OpenGlViewer::OpenGlViewer() {
    std::cout << "Hi from constructor" << std::endl;
}

/**
 * @brief Initializes the OpenGL viewer, setting up the necessary resources.
 */
void Mgtt::Apps::OpenGlViewer::Init() {
    std::cout << "Hi from Init" << std::endl;
}

/**
 * @brief Renders the scene using OpenGL.
 *
 * This method is responsible for rendering the contents of the scene using OpenGL.
 */
void Mgtt::Apps::OpenGlViewer::Render() {
    std::cout << "Hi from Render()" << std::endl;
}

/**
 * @brief Clears the rendering buffer and performs cleanup.
 *
 * This method is called to clear the rendering buffer and perform any necessary cleanup
 * before exiting or switching to a new scene.
 */
void Mgtt::Apps::OpenGlViewer::Clear() {
    std::cout << "Hi from Clear()" << std::endl;
}

int main() {
    Mgtt::Apps::OpenGlViewer openGlViewer;

    openGlViewer.Init();
    openGlViewer.Render();
    openGlViewer.Clear();
    return 0;
}