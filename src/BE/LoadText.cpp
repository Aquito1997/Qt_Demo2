#include "LoadText.h"

#include <GL/gl.h>
#include <cassert>
#include <freetype/freetype.h>
#include <iostream>

#include <QFileInfo>
#include <qchar.h>
#include <qdebug.h>
#include <qopenglext.h>
#include <qvector2d.h>

#define ErrorDetail(val)                                                                                   \
    {                                                                                                      \
        if (mError != 0)                                                                                   \
            std::cout << __FILE__ << ":" << __LINE__ << " Error Code: " << mError                          \
                      << "\nsee: https://freetype.org/freetype2/docs/reference/ft2-error_code_values.html" \
                      << std::endl;                                                                        \
    }


LoadText::LoadText(int TextSize)
{
    mError = FT_Init_FreeType(&mLibrary);
    ErrorDetail(mError);

    mError = FT_New_Face(mLibrary, "./UbuntuMono-Regular.ttf", 0, &mFace);
    ErrorDetail(mError);

    //定义字体大小
    mError = FT_Set_Pixel_Sizes(mFace, 0, TextSize);
    ErrorDetail(mError);

    mError = FT_Set_Char_Size(mFace, 50 * 64, 0, 100, 0);
    ErrorDetail(mError);
}

LoadText::~LoadText()
{
    FT_Done_Face(mFace);
    FT_Done_FreeType(mLibrary);

    // 清除显存纹理
    for (auto pair : charTexture)
        glDeleteTextures(1, &pair.second.TextureID);
}

void LoadText::initialize(QOpenGLFunctions_4_5_Core* gl)
{
    assert(gl);
    mGL = gl;
    mGL->glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}


void LoadText::Insert(wchar_t wChr)
{
    auto val = charTexture.find(wChr);
    if (val != charTexture.end())// 已经存在 就返回
        return;

    mError = FT_Load_Char(mFace, wChr, FT_LOAD_RENDER);
    ErrorDetail(mError);

    FT_Bitmap* bitmap = &mFace->glyph->bitmap;
    int charWidth = bitmap->width;
    int charHight = bitmap->rows;

    GLuint texId = 0;
    mGL->glGenTextures(1, &texId);
    mGL->glBindTexture(GL_TEXTURE_2D, texId);
    mGL->glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, charWidth, charHight, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap->buffer);
    mGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    mGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    mGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    mGL->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    mGL->glBindTexture(GL_TEXTURE_2D, 0);

    TextInfo info = {texId,
                     QVector2D(bitmap->width, bitmap->rows),
                     QVector2D(mFace->glyph->bitmap_left, mFace->glyph->bitmap_top),
                     static_cast<GLuint>(mFace->glyph->advance.x)};
    //  static_cast<GLuint>(bitmap->width)};
    charTexture.insert(std::pair<wchar_t, TextInfo>(wChr, info));
    return;
}


void LoadText::Update(wchar_t wChr)
{
    auto val = charTexture.find(wChr);
    if (val != charTexture.end())// 已经存在 删除已经纹理
        glDeleteTextures(1, &val->second.TextureID);

    FT_Load_Char(mFace, wChr, FT_LOAD_RENDER);

    FT_Bitmap* bitmap = &mFace->glyph->bitmap;
    int charWidth = bitmap->width;
    int charHight = bitmap->rows;

    GLuint texId = 0;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, charWidth, charHight, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap->buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    TextInfo info = {texId,
                     QVector2D(bitmap->width, bitmap->rows),
                     QVector2D(mFace->glyph->bitmap_left, mFace->glyph->bitmap_top),
                     static_cast<unsigned int>(mFace->glyph->advance.x)};
    charTexture[wChr] = info;
}


void LoadText::Delete(wchar_t wChr)
{
    auto val = charTexture.find(wChr);
    if (val != charTexture.end())
    {
        glDeleteTextures(1, &val->second.TextureID);
        charTexture.erase(val);
    }
}


int LoadText::GetTexture(wchar_t wChr)
{
    auto pair = charTexture.find(wChr);
    if (pair == charTexture.end())
        Insert(wChr);

    return charTexture[wChr].TextureID;
}


TextInfo LoadText::GetTextInfo(wchar_t wChr)
{
    auto pair = charTexture.find(wChr);
    if (pair == charTexture.end())
        Insert(wChr);

    return charTexture[wChr];
}
