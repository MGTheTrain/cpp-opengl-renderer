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
    this->windowParams = std::make_unique<WindowParams>();
    this->windowParams->name = "opengl-viewer";
    this->windowParams->width = 1000.0f;
    this->windowParams->height = 1000.0f;

    this->glmMatrices = std::make_unique<GlmMatrices>();
    this->glfwWindow = std::make_unique<Mgtt::Window::GlfwWindow>(this->windowParams->name, this->windowParams->width, this->windowParams->height);
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error("GLEW ERROR: Glew could not be initialized");
    }
    glEnable(GL_DEPTH_TEST);

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

        this->glmMatrices->projection = glm::perspective(glm::radians(45.0f), this->windowParams->width / this->windowParams->height, 0.1f, 1000.0f);
        this->glmMatrices->view = glm::translate(this->glmMatrices->view, glm::vec3(0.0f, 0.0f, -3.0f));

        for (auto& node : this->mgttScene.nodes) {
            this->TraverseSceneNode(node);
        }

        // Check brdf lut
        //this->renderTextureContainer.brdfLutShader.Use();

        //glBindVertexArray(this->renderTextureContainer.quadVao);
        //glDrawArrays(GL_TRIANGLES, 0, 4);
        //glBindVertexArray(0);

        // Check env map
        //this->renderTextureContainer.envMapShader.Use();
        //this->renderTextureContainer.envMapShader.SetMat4("projection", this->glmMatrices->projection);
        //this->renderTextureContainer.envMapShader.SetMat4("view", this->glmMatrices->view);
        //this->renderTextureContainer.envMapShader.SetInt("envMap", 0);

        //glDepthFunc(GL_LEQUAL);
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_CUBE_MAP, this->renderTextureContainer.cubeMapTextureId);
        //glBindVertexArray(this->renderTextureContainer.cubeVao);
        //glDrawArrays(GL_TRIANGLES, 0, 36);
        //glBindVertexArray(0);
        //glDepthFunc(GL_LESS);

        this->glfwWindow->SwapBuffersAndPollEvents();
    }
}


/**
 * @brief Iterates recursively over all nodes in the scene
 *
 * This function is responsible for iteraing recursively over all nodes in the scene
 *
 * @param node A shared pointer to the 3D node to be rendered.
 **/
void Mgtt::Apps::OpenGlViewer::TraverseSceneNode(std::shared_ptr<Mgtt::Rendering::Node> node) {
    if (node->mesh) {
        glUniformMatrix4fv(glGetUniformLocation(this->mgttScene.shader.GetProgramId(), "matrix"), 1, GL_FALSE, &node->mesh->matrix[0][0]);
        RenderMesh(node);
    }

    // recursevily draw
    for (auto& child : node->children) {
        this->TraverseSceneNode(child);
    }
}

/**
 * @brief Renders the mesh using the specified rendering technique.
 *
 * This function is responsible for rendering the mesh using the current rendering
 * technique and associated settings. It should be called within the rendering loop.
 */
void Mgtt::Apps::OpenGlViewer::RenderMesh(std::shared_ptr<Mgtt::Rendering::Node> node) {

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