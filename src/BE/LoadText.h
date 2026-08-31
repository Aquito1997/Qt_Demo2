/**
 * @file       LoadText.h
 * @brief      文件功能描述
 * @author     Aquito
 * @date       2026-03-26
 * @version    v1.0.0
 * @copyright  Copyright (c) 2026
 * @par 修改日志：
 * <table>
 * <tr><th>日期       <th>版本                     <th>作者    <th>描述
 * <tr><td>2026/03/26 Time: 15:14   <td>1.0    <td>Aquito <td>创建文件
 * </table>
 */

#ifndef __LoadText_H__
#define __LoadText_H__

// #include "ft2build.h"
// #include FT_FREETYPE_H

#include <freetype/freetype.h>
#include <ft2build.h>

#include <GL/gl.h>
#include <map>

#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <qopenglext.h>
#include <qopenglfunctions_4_5_core.h>
#include <qvector2d.h>


typedef struct __TextInfo {
    GLuint TextureID; // ID handle of the glyph texture
    QVector2D Size;   // Size of glyph
    QVector2D Bearing;// Offset from baseline to left/top of glyph
    GLuint Advance;   // Horizontal offset to advance to next glyph
} TextInfo;

class LoadText : public QOpenGLFunctions_4_5_Core
{
private:
    FT_Library mLibrary;
    FT_Face mFace;
    std::map<wchar_t, TextInfo> charTexture;
    FT_Error mError;
    QOpenGLFunctions_4_5_Core* mGL;

public:
    LoadText(int TextSize);
    ~LoadText();
    void initialize(QOpenGLFunctions_4_5_Core* gl);

    // insert && update
    void Insert(wchar_t wChr);
    // 可能字体大小会修改
    void Update(wchar_t wChr);
    void Delete(wchar_t wChr);
    int GetTexture(wchar_t wChr);
    TextInfo GetTextInfo(wchar_t wChr);
};


#endif