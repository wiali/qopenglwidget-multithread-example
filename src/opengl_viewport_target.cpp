/* ---------------------------------------------------------------- *
   Author: Kuumies <kuumies@gmail.com>
   Desc:   Implmentation of kuu::opengl::ViewportTarget class.
 * ---------------------------------------------------------------- */

#include "opengl_viewport_target.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include "opengl_mesh.h"
#include "opengl_shader.h"

namespace kuu
{
namespace opengl
{

namespace
{

/* ---------------------------------------------------------------- *
   Returns the OpenGL shader info log 
 * ---------------------------------------------------------------- */
std::string shaderInfoLog(GLint id)
{
    GLint length = 0;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

    if (length <= 0)
        return std::string();

    std::string log;
    log.resize(length + 1);
    glGetShaderInfoLog(id, length, NULL, (GLchar*)log.c_str());

    log.erase(std::remove(log.begin(), log.end(), '\0'), log.end());
    return log;
}
    
} // anonymous namespace

/* ---------------------------------------------------------------- *
   The data of the viewport target.
 * ---------------------------------------------------------------- */
struct ViewportTarget::Data
{
    Data()
    {
        // Create vertex data.
        const float w = 1.0f;
        const float h = 1.0f;
        const std::vector<float> vertexData =
        {
          // x   y   z     r     g     b,    u,    v
            -w, -h, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
             w, -h, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
             w,  h, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
            -w,  h, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f
        };

        // Create triangle indices (two triangles)
        const std::vector<unsigned int> indexData =
        {
            0u, 1u, 2u,
            2u, 3u, 0u
        };

        mesh = std::make_shared<Mesh>();
        mesh->writeVertexData(vertexData);
        mesh->writeIndexData(indexData);
        mesh->setAttributeDefinition(0, 3, 8 * sizeof(float), 0);
        mesh->setAttributeDefinition(1, 3, 8 * sizeof(float),
                                     3 * sizeof(float));
        mesh->setAttributeDefinition(2, 3, 8 * sizeof(float),
                                     6 * sizeof(float));

        const std::string vshSource =
            "#version 330 core\r\n" // note linebreak
            "layout (location = 0) in vec3 position;"
            "layout (location = 1) in vec3 color;"
            "layout (location = 2) in vec2 texCoord;"
            "out vec4 colorIn;"
            "out vec2 texCoordIn;"
            "void main(void)"
            "{"
               " gl_Position = vec4(position, 1.0);"
                "colorIn = vec4(color, 1.0);"
                "texCoordIn = texCoord;"
            "}";

        const std::string fshSource =
            "#version 330 core\r\n" // note linebreak
            "uniform sampler2D tex;"
            "in vec4 colorIn;"
            "in vec2 texCoordIn;"
            "out vec4 colorOut;"
            "void main(void)"
            "{"
                "colorOut = texture(tex, texCoordIn);"
            "}";

        shader = std::make_shared<Shader>();
        shader->setVertexShader(vshSource);
        shader->setFragmentShader(fshSource);
        shader->link();
    }

    std::shared_ptr<Mesh> mesh;
    std::shared_ptr<Shader> shader;
};

/* ---------------------------------------------------------------- *
   Constructs the viewport target.
 * -----------------------------------------------------------------*/
ViewportTarget::ViewportTarget()
    : d(std::make_shared<Data>())
{}

/* ---------------------------------------------------------------- *
   Renders the viewport target.
 * -----------------------------------------------------------------*/
void ViewportTarget::render(GLuint textureId)
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureId);

    d->mesh->bind();
    d->shader->bind();
    d->shader->setUniform("tex", 0);
    d->mesh->render(GL_TRIANGLES);
    d->shader->release();
    d->mesh->release();

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

} // namespace opengl
} // namespace kuu
