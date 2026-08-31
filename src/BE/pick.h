/**
 * @file       Select.h
 * @brief      文件功能描述
 * @author     Aquito
 * @date       2026-03-29
 * @version    v1.0.0
 * @copyright  Copyright (c) 2026
 * @par 修改日志：
 * <table>
 * <tr><th>日期       <th>版本                     <th>作者    <th>描述
 * <tr><td>2026/03/29 Time: 02:16   <td>1.0    <td>Aquito <td>创建文件
 * </table>
 */

#ifndef __Select_H__
#define __Select_H__


#include <GL/gl.h>
#include <cstddef>
#include <qmatrix4x4.h>
#include <qopenglfunctions_4_5_core.h>


class OpenGLPick : public QOpenGLFunctions_4_5_Core
{
private:
    QOpenGLFunctions_4_5_Core* mGL;
    GLuint mFBO, mTex, mVao, mVbo;
    size_t mWidth, mHeight;


public:
    explicit OpenGLPick();
    void InitOpenGL(QOpenGLFunctions_4_5_Core* gl);

    void VertexAttribInit();
    void GenFBO_Tex(int w, int h);

    void Resize(int w, int h);

    GLuint GetVAO();
    GLuint GetFBO();
    GLuint GetShader();

    size_t FB_Width();
    size_t FB_Height();
};

#endif//__Select_H__