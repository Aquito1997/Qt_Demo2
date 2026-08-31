#include "pick.h"
#include "data.h"
#include <GL/gl.h>
#include <cassert>
#include <qopenglext.h>


void OpenGLPick::VertexAttribInit()
{
    mGL->glGenVertexArrays(1, &mVao);
    mGL->glGenBuffers(1, &mVbo);
    mGL->glBindVertexArray(mVao);
    mGL->glBindBuffer(GL_ARRAY_BUFFER, mVbo);
    mGL->glBufferData(GL_ARRAY_BUFFER, sizeof(glData::plane), glData::plane, GL_STATIC_DRAW);
    mGL->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    mGL->glEnableVertexAttribArray(0);

    mGL->glBindBuffer(GL_ARRAY_BUFFER_BINDING, 0);
    mGL->glBindVertexArray(0);
}

void OpenGLPick::Resize(int w, int h)
{
    mGL->glBindTexture(GL_TEXTURE_2D, 0);
    mGL->glBindFramebuffer(GL_FRAMEBUFFER, 0);
    mGL->glDeleteFramebuffers(1, &mFBO);
    mGL->glDeleteTextures(1, &mTex);

    GenFBO_Tex(w, h);
}

GLuint OpenGLPick::GetVAO()
{
    return mVao;
}

void OpenGLPick::GenFBO_Tex(int w, int h)
{
    mWidth = w;
    mHeight = h;
    mGL->glGenFramebuffers(1, &mFBO);
    mGL->glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

    mGL->glGenTextures(1, &mTex);
    mGL->glBindTexture(GL_TEXTURE_2D, mTex);
    mGL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    mGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    mGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    mGL->glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mTex, 0);
    GLuint attachments = GL_COLOR_ATTACHMENT0;
    mGL->glDrawBuffer(attachments);

    mGL->glBindTexture(GL_TEXTURE_2D, 0);
    mGL->glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


GLuint OpenGLPick::GetFBO()
{
    return mFBO;
}


size_t OpenGLPick::FB_Width()
{
    return mWidth;
}


size_t OpenGLPick::FB_Height()
{
    return mHeight;
}

void OpenGLPick::InitOpenGL(QOpenGLFunctions_4_5_Core* gl)
{
    assert(gl);
    mGL = gl;
}

OpenGLPick::OpenGLPick()
{
}
