/**
 * @file       data.h
 * @brief      文件功能描述
 * @author     Aquito
 * @date       2026-03-26
 * @version    v1.0.0
 * @copyright  Copyright (c) 2026
 * @par 修改日志：
 * <table>
 * <tr><th>日期       <th>版本                     <th>作者    <th>描述
 * <tr><td>2026/03/26 Time: 21:32   <td>1.0    <td>Aquito <td>创建文件
 * </table>
 */

#ifndef __data_H__
#define __data_H__

#include <qvector3d.h>

namespace glData
{
static const float orthoLeft = -2.0f;
static const float orthoRight = 2.0f;
static const float orthoBottom = -2.0f;
static const float orthoTop = 2.0f;
static const float orthoNear = -4.0f;
static const float orthoFar = 4.0f;


static float xRed[3] = {1.0f, 0.0f, 0.0f};
static float xGreen[3] = {0.0f, 1.0f, 0.0f};
static float xBlue[3] = {0.0f, 0.0f, 1.0f};
static float xWhite[3] = {1.0f, 1.0f, 1.0f};
static float xGrey[3] = {0.4f, 0.4f, 0.4f};

static float vertices[] = {
    0.0f, 0.0f, 0.0f,// 原点

    0.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,// xAxis
    0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,// yAxis
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f,// zAxis

    /////////////////////////////////////////////////////
    0.1f, 0.0f, 0.0f,
    0.1f, 0.0f, 0.9f,// xz
    0.2f, 0.0f, 0.0f,
    0.2f, 0.0f, 0.9f,// xz
    0.3f, 0.0f, 0.0f,
    0.3f, 0.0f, 0.9f,// xz
    0.4f, 0.0f, 0.0f,
    0.4f, 0.0f, 0.9f,// xz
    0.5f, 0.0f, 0.0f,
    0.5f, 0.0f, 0.9f,// xz
    0.6f, 0.0f, 0.0f,
    0.6f, 0.0f, 0.9f,// xz
    0.7f, 0.0f, 0.0f,
    0.7f, 0.0f, 0.9f,// xz
    0.8f, 0.0f, 0.0f,
    0.8f, 0.0f, 0.9f,// xz
    0.9f, 0.0f, 0.0f,
    0.9f, 0.0f, 0.9f,// xz

    /////////////////////////////////////////////////////
    0.0f, 0.0f, 0.1f,
    0.9f, 0.0f, 0.1f,// zx
    0.0f, 0.0f, 0.2f,
    0.9f, 0.0f, 0.2f,// zx
    0.0f, 0.0f, 0.3f,
    0.9f, 0.0f, 0.3f,// zx
    0.0f, 0.0f, 0.4f,
    0.9f, 0.0f, 0.4f,// zx
    0.0f, 0.0f, 0.5f,
    0.9f, 0.0f, 0.5f,// zx
    0.0f, 0.0f, 0.6f,
    0.9f, 0.0f, 0.6f,// zx
    0.0f, 0.0f, 0.7f,
    0.9f, 0.0f, 0.7f,// zx
    0.0f, 0.0f, 0.8f,
    0.9f, 0.0f, 0.8f,// zx
    0.0f, 0.0f, 0.9f,
    0.9f, 0.0f, 0.9f,// zx

    /////////////////////////////////////
    0.0f, 0.1f, 0.0f,
    0.9f, 0.1f, 0.0f,// y
    0.0f, 0.2f, 0.0f,
    0.9f, 0.2f, 0.0f,// y
    0.0f, 0.3f, 0.0f,
    0.9f, 0.3f, 0.0f,// y
    0.0f, 0.4f, 0.0f,
    0.9f, 0.4f, 0.0f,// y
    0.0f, 0.5f, 0.0f,
    0.9f, 0.5f, 0.0f,// y
    0.0f, 0.6f, 0.0f,
    0.9f, 0.6f, 0.0f,// y
    0.0f, 0.7f, 0.0f,
    0.9f, 0.7f, 0.0f,// y
    0.0f, 0.8f, 0.0f,
    0.9f, 0.8f, 0.0f,// y
    0.0f, 0.9f, 0.0f,
    0.9f, 0.9f, 0.0f,// y

    //////////////////////////////////////////////////
    0.0f, 0.1f, 0.0f,
    0.0f, 0.1f, 0.9f,// z
    0.0f, 0.2f, 0.0f,
    0.0f, 0.2f, 0.9f,// z
    0.0f, 0.3f, 0.0f,
    0.0f, 0.3f, 0.9f,// z
    0.0f, 0.4f, 0.0f,
    0.0f, 0.4f, 0.9f,// z
    0.0f, 0.5f, 0.0f,
    0.0f, 0.5f, 0.9f,// z
    0.0f, 0.6f, 0.0f,
    0.0f, 0.6f, 0.9f,// z
    0.0f, 0.7f, 0.0f,
    0.0f, 0.7f, 0.9f,// z
    0.0f, 0.8f, 0.0f,
    0.0f, 0.8f, 0.9f,// z
    0.0f, 0.9f, 0.0f,
    0.0f, 0.9f, 0.9f,// z
};

static const QVector3D yPoint(vertices[12], vertices[13], vertices[14]);

static const float textColor[] = {1.0f, 1.0f, 1.0f};

static const float plane[] = {
    -0.5f, 0.0f, -0.5f,//
    +0.5f, 0.0f, -0.5f,//
    +0.5f, 0.0f, +0.5f,//
    +0.5f, 0.0f, +0.5f,//
    -0.5f, 0.0f, +0.5f,//
    -0.5f, 0.0f, -0.5f,//
};

static const float viewMatrix[] = {
    1.0, 0.0, +0.0, 0.0,
    0.0, 1.0, +0.0, 0.0,
    0.0, 0.0, +1.0, -3.0,
    0.0, 0.0, 0.0, 1.0//
};


static const char* vertexShaderSource = "#version 450 core\n"
                                        "layout (location = 0) in vec3 aPos;\n"
                                        "uniform mat4 cModel;\n"
                                        "uniform mat4 cView;\n"
                                        "uniform mat4 cProj;\n"
                                        "void main()\n"
                                        "{\n"
                                        "    gl_Position = cProj * cView * cModel * vec4(aPos,1.0);\n"
                                        // "    gl_Position = cProj * cModel * vec4(aPos,1.0);\n"
                                        // "    gl_Position = cModel * vec4(aPos,1.0);\n"
                                        "}\n\0";

static const char* fragmentShaderSource = "#version 450 core\n"
                                          "layout(location = 0) out vec4 FragColor;\n"
                                          "uniform vec3 inColor;"
                                          "void main()\n"
                                          "{\n"
                                          "    FragColor = vec4(inColor, 1.0f);\n"
                                          "}\n\0";


static const char* TextVertex = "#version 450 core\n"
                                "layout(location = 0) in vec4 vertex;\n"
                                "out vec2 TexCoords;\n"
                                "uniform mat4 tModel;\n"
                                "uniform mat4 tView;\n"
                                "uniform mat4 tProj;\n"
                                "void main() {\n"
                                "    gl_Position =  tProj * tView * tModel * vec4(vertex.xy,0.0,1.0);\n"
                                "    TexCoords = vertex.zw;\n"
                                "}\n\0";

static const char* TextFragment = "#version 450 core\n"
                                  "in vec2 TexCoords;\n"
                                  "out vec4 color;\n"
                                  "uniform sampler2D fontTex;\n"
                                  "uniform vec3 texColor;\n"
                                  "void main()\n"
                                  "{\n"
                                  "    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(fontTex, TexCoords).r);\n"
                                  "    color = vec4(texColor, 1.0) * sampled;\n"
                                  "}\n\0";
};// namespace glData


#endif