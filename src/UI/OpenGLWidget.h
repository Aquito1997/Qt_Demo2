#ifndef _OPENGLWIDGET__H_
#define _OPENGLWIDGET__H_

#include "src/BE/LoadText.h"
#include "src/BE/pick.h"

#include <GL/gl.h>

#include <QEvent>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QOpenGLFunctions_4_5_Core>// 相当于 GLAD
#include <QOpenGLWidget>            // 相当于GLFW
#include <qcoreevent.h>
#include <qevent.h>
#include <qmatrix4x4.h>
#include <qobject.h>
#include <qopenglext.h>
#include <qopenglversionfunctions.h>
#include <qvector2d.h>
#include <qvector3d.h>

class OpenGLWidget : public QOpenGLWidget, QOpenGLFunctions_4_5_Core
{
    Q_OBJECT

public:
    explicit OpenGLWidget(QWidget* parent = nullptr);
    void MoveGrid(int);
    void triggerRepaint();

    void FontScale(int val);
    void SceneScale(int val);
    void SphereScale(int val);
    void SceneTrans(const QVector3D& point);

protected:
    /* 需要重载的 QOpenGLWidget 中的三个函数 */
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    // 编译 着色器程序
    void CoordinateShaderProgram(const char* vertexSrc, const char* FragSrc, GLuint& program);
    // 顶点 属性相关
    void CoordinateVertexInit();
    // 绘制坐标系
    void DrawCoordinate();

    void DrawSphere();

    void DrawPlane();
    // 渲染字体
    // str 要渲染的字体
    // point 渲染字体的起始位置
    // quadScale 渲染字体所在的矩形的缩放因子
    // modelScale model 缩放因子, 相对于坐标系
    void xDrawText(const QString& str, QVector3D& point, const float& quadScale, const float& modelScale);

    void DrawFont();

    // 将坐标转为世界空间的坐标
    void Trans2Model(QVector3D& point);

    // 将鼠标在控件的坐标， 转换为世界空间的坐标
    QVector3D ConvertMousePos2Model(const QPoint& mousePos);

    void TextVertexInit();

    // 顔色拾取法 获取场景中对象
    void MousePick(GLuint fbo, int x, int y);
    QMatrix4x4 CreateBillBoardMatrix(const QVector3D& position, const QVector3D& camPos, const QVector3D& worldUp = QVector3D(0, 1, 0));

    void DrawTag(GLuint shader, GLuint fbo, GLuint vao);


signals:

public slots:


private:
    // 渲染 坐标系 相关
    GLuint mCoordSP, mCoordUniModel, mCoordUniColor, mCoordUniProj, mCoordUniView;
    QMatrix4x4 mCoordModel, mCoordView, mCoordProj;
    GLuint mCoordVbo, mCoordVao;
    float mMoveGrid;

    // 渲染字体相关
    GLuint mTextVao, mTextVbo, mTextSP;
    GLuint mTextUniModel, mTextUniView, mTextUniProj, mTextUniText, mTextUniTextColor;// uniform 变量
    QMatrix4x4 mTextModel, MTextProj;
    GLuint mTextSampler;

    // 半球体
public:
    float mSphereFact, mFontScale, mSceneScale;


private:
    GLuint mSphereVAO, mSphereVBO, mSphereEBO, mSphereIndexCount;
    GLuint mSphereSP;

    // 平面
    GLuint mPlaneVAO, mPlaneVBO, mPlaneSP;

    OpenGLPick* mPick;                    // pick 逻辑
    float mTagPos_Y;                      // tag 指的是 鼠标能选中的那个灰色 矩形
    bool mSwitchColor, mEnableRotateScene;// mSwitchColor控制选中逻辑是否切换tag的颜色
                                          // mEnableRotateScene 使能 / 失能 mouseMoveEvent

    //用来获取Y轴的长度的
    QVector4D mAxis;


    QPoint mLastPoint;
    QVector2D mMouseMoveDir, mMouseLastDir;
    LoadText* mLoadText;

    QSize mSize;
};

#endif// _OPENGLWIDGET__H_
