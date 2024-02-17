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


#include <opengl-viewer.h>

/**
 * @brief Destructs the OpenGlViewer object.
 */
Mgtt::Apps::OpenGlViewer::~OpenGlViewer() {
    this->Clear();
}

/**
 * @brief Clears OpenGl and RAM allocated resources
 * 
 */
void Mgtt::Apps::OpenGlViewer::Clear() {
    this->gltfSceneImporter->Clear(this->mgttScene);
    this->textureManager->Clear(this->renderTextureContainer);
}

/**
 * @brief Constructs an OpenGlViewer object.
 */
Mgtt::Apps::OpenGlViewer::OpenGlViewer() {
    this->glfwWindow = std::make_unique<Mgtt::Window::GlfwWindow>("opengl-viewer", 1000.0f, 1000.0f);
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("GLEW ERROR: Glew could not be initialized");
    }

    // Compile shaders and link to OpenGl program
    auto pbrShaderPathes = std::make_pair<std::string, std::string>("assets/shader/core/pbr.vert","assets/shader/core/pbr.frag");
    this->mgttScene.shader.Compile(pbrShaderPathes);
    auto eq2BrdfLutShaderPathes = std::make_pair<std::string, std::string>("assets/shader/core/eq2CubeMap.vert", "assets/shader/core/eq2CubeMap.frag");
    auto brdfLutShaderPathes = std::make_pair<std::string, std::string>("assets/shader/core/genBrdf.vert", "assets/shader/core/genBrdf.frag");
    auto envMapShaderPathes = std::make_pair<std::string, std::string>("assets/shader/core/envMap.vert", "assets/shader/core/envMap.frag");
    this->renderTextureContainer = Mgtt::Rendering::RenderTexturesContainer(eq2BrdfLutShaderPathes, brdfLutShaderPathes, envMapShaderPathes);

    // scene
    this->gltfSceneImporter = std::make_unique<Mgtt::Rendering::GltfSceneImporter>();
    std::string mgttScenePath= "assets/scenes/water-bottle/WaterBottle.gltf";
    this->gltfSceneImporter->Load(this->mgttScene, mgttScenePath);

    // e2quirectangular to env map
    std::string hdrTexturePath = "assets/texture/surgery.jpg";
    this->textureManager->LoadFromHdr(this->renderTextureContainer, hdrTexturePath);

    // brdf lut
    this->textureManager->LoadBrdfLut(this->renderTextureContainer);

    glEnable(GL_DEPTH_TEST);
}


/**
 * @brief Renders the scene using OpenGL.
 *
 * This method is responsible for rendering the contents of the scene using OpenGL.
 */
void Mgtt::Apps::OpenGlViewer::Render() {
    while(!this->glfwWindow->WindowShouldClose()) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(skyBoxShader.id);
        glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.id, "projection"), 1, GL_FALSE, &projection[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(skyBoxShader.id, "view"), 1, GL_FALSE, &view[0][0]);
        glUniform1i(glGetUniformLocation(skyBoxShader.id, "skybox"), 0);

        glDepthFunc(GL_LEQUAL);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, this->renderTextureContainer.envMapVao);
        glBindVertexArray(this->renderTextureContainer.envMapVao);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        this->glfwWindow->SwapBuffersAndPollEvents();
    }
}
 
int main() {
    Mgtt::Apps::OpenGlViewer openGlViewer;
    try {
        openGlViewer.Render();

    } catch(const std::runtime_error& ex) {
        std::cout << ex.what() << std::endl;
        openGlViewer.Clear();
        return 1;
    }
    return 0;
}