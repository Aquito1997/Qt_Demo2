#include "OpenGLWidget.h"

#include <GL/gl.h>
// #include <GL/glu.h>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <iomanip>
#include <iostream>

#include <QDebug>
#include <qdebug.h>
#include <qglobal.h>
#include <qmatrix4x4.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qopenglext.h>
#include <qvector2d.h>
#include <qvector3d.h>
#include <qvector4d.h>
#include <utility>
#include <vector>

#include "data.h"
#include "freetype/freetype.h"
#include "src/BE/LoadText.h"
#include "src/BE/pick.h"

static QVector3D xCamPos(0, 0, 10);

template<typename T>
const T& xClamp(const T& value, const T& low, const T& high)
{
    return (value < low) ? low : ((value > high) ? high : value);
}


OpenGLWidget::OpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
    mMouseLastDir = QVector2D(0, 0);
    mMouseMoveDir = QVector2D(0, 0);
    mCoordModel.setToIdentity();
    mMoveGrid = 0;
    mSphereFact = 0.1;
    mFontScale = 0.08;
    mSceneScale = 1;

    mTagPos_Y = 0;
    mSwitchColor = false;
    mSize = minimumSize();

    mPick = new OpenGLPick();
}

void OpenGLWidget::CoordinateShaderProgram(const char* vertexSrc, const char* fragmentSrc, GLuint& program)
{
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSrc, NULL);
    glCompileShader(vertexShader);

    int success;
    char infolog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infolog);
        qDebug() << "ERROR SHADER Vertex COMPILATION_FAILED\n"
                 << infolog;
    }

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSrc, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infolog);
        qDebug() << "ERROR SHADER Fragment COMPILATION_FAILED\n"
                 << infolog;
    }

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glGetShaderiv(program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(program, 512, NULL, infolog);
        qDebug() << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
                 << infolog;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}


void OpenGLWidget::CoordinateVertexInit()
{
    glGenVertexArrays(1, &mCoordVao);
    glGenBuffers(1, &mCoordVbo);
    glBindVertexArray(mCoordVao);
    glBindBuffer(GL_ARRAY_BUFFER, mCoordVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glData::vertices), glData::vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 解绑
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void OpenGLWidget::TextVertexInit()
{
    glGenVertexArrays(1, &mTextVao);
    glGenBuffers(1, &mTextVbo);
    glBindVertexArray(mTextVao);
    glBindBuffer(GL_ARRAY_BUFFER, mTextVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


QMatrix4x4 OpenGLWidget::CreateBillBoardMatrix(const QVector3D& position, const QVector3D& camPos, const QVector3D& worldUp)
{
    QVector3D toCam = camPos - position;
    toCam.normalize();

    QVector3D coordRight = QVector3D::crossProduct(worldUp, toCam);
    coordRight.normalize();

    QVector3D coordUp = QVector3D::crossProduct(toCam, coordRight);
    coordUp.normalize();

    QMatrix4x4 billBoard;
    billBoard.setColumn(0, QVector4D(coordRight, 0.0f));
    billBoard.setColumn(1, QVector4D(coordUp, 0.0f));
    billBoard.setColumn(2, QVector4D(toCam, 0.0f));
    billBoard.setColumn(3, QVector4D(position, 1.0f));
    return billBoard;
}

void OpenGLWidget::DrawTag(GLuint shader, GLuint fbo, GLuint vao)
{
    QVector3D yAxis(mAxis.x(), mAxis.y(), mAxis.z());
    QVector3D tagPos(0, mTagPos_Y, 0);
    Trans2Model(tagPos);
    QMatrix4x4 billBoard = std::move(CreateBillBoardMatrix(tagPos, xCamPos));

    // 将 point 从 billBoard 坐标系转换到世界空间坐标系, 得到 point 这个坐标在 billBoard 空间坐标系的坐标，
    // 就是 point 这个坐标没变， 只是描述这个点的方式由世界空间坐标系变为了BillBoard空间坐标系
    QVector4D tmpPos = billBoard.inverted() * QVector4D(tagPos, 1.0f);
    QVector4D tmpYAxis = billBoard.inverted() * QVector4D(yAxis, 1.0f);
    if (tmpPos.y() < 0)
        tmpPos.setY(0);
    else if (tmpPos.y() >= tmpYAxis.y())
        tmpPos.setY(tmpYAxis.y());


    // std::cout << std::fixed << std::setprecision(4) << tmpPos.x() << ", " << tmpPos.y() << ", " << tmpPos.z() << std::endl
    //           << std::fixed << std::setprecision(4) << tagPos.x() << ", " << tagPos.y() << ", " << tagPos.z() << std::endl
    //           << std::fixed << std::setprecision(4) << tmpYAxis.x() << ", " << tmpYAxis.y() << ", " << tmpYAxis.z() << std::endl
    //           << std::endl;

    billBoard.scale(0.1);
    billBoard.rotate(90, QVector3D(1, 0, 0));
    billBoard.translate(0, tmpPos.y(), 0);

    glUseProgram(shader);
    glBindVertexArray(vao);

    glUniformMatrix4fv(mCoordUniModel, 1, false, billBoard.constData());
    glUniformMatrix4fv(mCoordUniView, 1, false, mCoordView.constData());
    glUniformMatrix4fv(mCoordUniProj, 1, false, mCoordProj.constData());
    glUniform3fv(mCoordUniColor, 1, glData::xGrey);


    static GLint prevFBO = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);

    // 绘制 鼠标选择需要的颜色信息
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    // 绘制场景信息
    {
        if (mSwitchColor)
            glUniform3fv(mCoordUniColor, 1, glData::xRed);
        else
            glUniform3fv(mCoordUniColor, 1, glData::xGrey);

        glUniformMatrix4fv(mCoordUniModel, 1, false, billBoard.constData());
        glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);// 恢复到之前的FBO
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glBindVertexArray(0);
}


void OpenGLWidget::DrawCoordinate()
{
    glUseProgram(mCoordSP);
    glBindVertexArray(mCoordVao);

    glUniformMatrix4fv(mCoordUniModel, 1, false, mCoordModel.constData());
    glUniformMatrix4fv(mCoordUniView, 1, false, mCoordView.constData());
    glUniformMatrix4fv(mCoordUniProj, 1, false, mCoordProj.constData());
    glUniform3fv(mCoordUniColor, 1, glData::xWhite);
    glPointSize(4.0f);            // 设置点大小
    glDrawArrays(GL_POINTS, 0, 1);// 绘制原点
    glPointSize(1.0f);            // 恢复点大小

    // grid
    QMatrix4x4 gridModel = mCoordModel;
    gridModel.translate(0, mMoveGrid, 0);
    gridModel.translate(0, mTagPos_Y, 0);
    glUniformMatrix4fv(mCoordUniModel, 1, false, gridModel.constData());
    glUniform3fv(mCoordUniColor, 1, glData::xGrey);
    glDrawArrays(GL_LINES, 7, 18); // 绘制xz平面网格
    glDrawArrays(GL_LINES, 25, 18);// 绘制zx平面网格

    glUniformMatrix4fv(mCoordUniModel, 1, false, mCoordModel.constData());
    glDrawArrays(GL_LINES, 43, 18);// 绘制xy 线段
    glDrawArrays(GL_LINES, 59, 20);// 绘制yz 线段


    // 绘制3条坐标轴
    glPointSize(2.0f);// 设置点大小
    glUniform3fv(mCoordUniColor, 1, glData::xRed);
    glDrawArrays(GL_LINES, 1, 2);
    glUniform3fv(mCoordUniColor, 1, glData::xGreen);
    glDrawArrays(GL_LINES, 3, 2);
    glUniform3fv(mCoordUniColor, 1, glData::xBlue);
    glDrawArrays(GL_LINES, 5, 2);
    glPointSize(1.0f);
}


void OpenGLWidget::DrawSphere()
{
    if (mSphereVAO == 0)
    {
        glGenVertexArrays(1, &mSphereVAO);
        glGenBuffers(1, &mSphereVBO);
        glGenBuffers(1, &mSphereEBO);

        std::vector<QVector3D> positions;
        std::vector<QVector2D> uv;
        std::vector<QVector3D> normals;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359f;
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                if (yPos < 0)
                    continue;

                positions.push_back(QVector3D(xPos, yPos, zPos));
                uv.push_back(QVector2D(xSegment, ySegment));
                normals.push_back(QVector3D(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow)// even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        mSphereIndexCount = static_cast<unsigned int>(indices.size());

        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x());
            data.push_back(positions[i].y());
            data.push_back(positions[i].z());
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x());
                data.push_back(normals[i].y());
                data.push_back(normals[i].z());
            }
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x());
                data.push_back(uv[i].y());
            }
        }
        glGenVertexArrays(1, &mSphereVAO);
        glGenBuffers(1, &mSphereVBO);
        glGenBuffers(1, &mSphereEBO);

        glBindVertexArray(mSphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mSphereVBO);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mSphereEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        unsigned int stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    }
    QMatrix4x4 sphereModel = mCoordModel;
    sphereModel.translate(0.5, 0.1, 0.5);
    sphereModel.scale(mSphereFact);

    glUseProgram(mCoordSP);
    glBindVertexArray(mSphereVAO);

    glUniform3fv(mCoordUniColor, 1, glData::xGreen);
    glUniformMatrix4fv(mCoordUniModel, 1, false, sphereModel.constData());
    glUniformMatrix4fv(mCoordUniView, 1, false, mCoordView.constData());
    glUniformMatrix4fv(mCoordUniProj, 1, false, mCoordProj.constData());

    glDrawElements(GL_TRIANGLE_STRIP, mSphereIndexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void OpenGLWidget::DrawPlane()
{
    if (mPlaneVAO == 0)
    {
        glGenVertexArrays(1, &mPlaneVAO);
        glGenBuffers(1, &mPlaneVBO);

        glBindVertexArray(mPlaneVAO);
        glBindBuffer(GL_ARRAY_BUFFER, mPlaneVBO);
        glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), &glData::plane[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    }

    QMatrix4x4 planeModel = mCoordModel;
    planeModel.translate(0.5, 0.1, 0.5);
    planeModel.scale(mSphereFact * 5);

    glUseProgram(mCoordSP);
    glBindVertexArray(mPlaneVAO);

    glUniform3fv(mCoordUniColor, 1, glData::xRed);
    glUniformMatrix4fv(mCoordUniModel, 1, false, planeModel.constData());
    glUniformMatrix4fv(mCoordUniView, 1, false, mCoordView.constData());
    glUniformMatrix4fv(mCoordUniProj, 1, false, mCoordProj.constData());

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}


void OpenGLWidget::xDrawText(const QString& str,
                             QVector3D& point,
                             const float& quadScale,
                             const float& modelScale)
{
    glUseProgram(mTextSP);
    glBindVertexArray(mTextVao);
    glActiveTexture(GL_TEXTURE0);

    QMatrix4x4 billBoard = std::move(CreateBillBoardMatrix(point, xCamPos));

    // 模型矩阵：在窗口中间显示
    QMatrix4x4 bbModel = billBoard;
    bbModel.scale(modelScale);

    // 将 point 从 billBoard 坐标系转换到世界空间坐标系, 得到 point 这个坐标在 billBoard 空间坐标系的坐标，
    // 就是 point 这个坐标没变， 只是描述这个点的方式由世界空间坐标系变为了BillBoard空间坐标系
    QVector4D tmpPos = bbModel.inverted() * QVector4D(point, 1.0f);
    QVector3D billPos = std::move(QVector3D(tmpPos.x(), tmpPos.y(), tmpPos.z()));
    bbModel.translate(billPos.x(), billPos.y(), billPos.z());


    // 设置uniform
    glUniformMatrix4fv(mTextUniModel, 1, false, bbModel.constData());
    glUniformMatrix4fv(mTextUniView, 1, false, mCoordView.constData());
    glUniformMatrix4fv(mTextUniProj, 1, false, mCoordProj.constData());
    glUniform3fv(mTextUniTextColor, 1, glData::xWhite);
    glUniform1i(mTextUniText, 0);

    // 获取字符对应的bitmap
    for (auto cStr : str.toStdWString())
    {
        mLoadText->Insert(cStr);
        const auto& info = mLoadText->GetTextInfo(cStr);

        float xPos = point.x() + info.Bearing.x() * quadScale;
        float yPos = point.y() - (info.Size.y() - info.Bearing.y()) * quadScale;
        float w = info.Size.x() * quadScale;
        float h = info.Size.y() * quadScale;

        // 字符所在的面
        std::vector<float> quadVertices = {
            xPos + 0, yPos + h, 0.0f, 0.0f,//
            xPos + 0, yPos + 0, 0.0f, 1.0f,//
            xPos + w, yPos + 0, 1.0f, 1.0f,//
            xPos + 0, yPos + h, 0.0f, 0.0f,//
            xPos + w, yPos + 0, 1.0f, 1.0f,//
            xPos + w, yPos + h, 1.0f, 0.0f,//
        };

        glBindTexture(GL_TEXTURE_2D, info.TextureID);

        glBindBuffer(GL_ARRAY_BUFFER, mTextVbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, quadVertices.size() * sizeof(float), quadVertices.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        point.setX(point.x() + (info.Advance >> 6) * quadScale);
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}


void OpenGLWidget::MoveGrid(int val)
{
    mMoveGrid = (float)val / 100;
}


void OpenGLWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        static QPoint msPos;
        msPos = event->pos();

        if (mEnableRotateScene)
        {
            mMouseMoveDir = QVector2D(msPos - mLastPoint);
            if (mMouseMoveDir == QVector2D(0, 0))
                mMouseMoveDir = QVector2D(1, 1);
        }
        else
        {
            // 此时的tmp.y()的值就是Y轴的长度， 经过model变换的后的长度
            // 举例本来Y轴长1.0，经过变换后长0.7
            mAxis = mCoordModel * QVector4D(glData::yPoint, 1.0);

            // 鼠标在model的坐标
            QVector3D pos = std::move(ConvertMousePos2Model(msPos));
            if (pos.y() > mAxis.y())
                mTagPos_Y = mAxis.y();
            else if (pos.y() < 0)
                mTagPos_Y = 0;
            else
                mTagPos_Y = pos.y() / mAxis.y();
        }

        mLastPoint = msPos;
        update();
    }
    QOpenGLWidget::mouseMoveEvent(event);
}


void OpenGLWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        QPoint pos = event->pos();
        int x = pos.x();
        int y = pos.y();
        MousePick(mPick->GetFBO(), x, y);
        update();
    }
    QOpenGLWidget::mousePressEvent(event);
}


void OpenGLWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        mSwitchColor = false;
        mEnableRotateScene = true;
        update();
    }
    QOpenGLWidget::mouseReleaseEvent(event);
}


void OpenGLWidget::DrawFont()
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    {
        QVector3D origin(0, 0, 0);
        Trans2Model(origin);
        xDrawText(QString("0"), origin, 0.01, 0.15);

        QVector3D xAxis(1, 0.02, 0);
        Trans2Model(xAxis);
        xDrawText(QString("X"), xAxis, 0.01, 0.15);

        QVector3D yAxis(-0.1, 1.02, 0);
        Trans2Model(yAxis);
        xDrawText(QString("Y"), yAxis, 0.01, 0.15);

        QVector3D zAxis(-0.1, 0, 1);
        Trans2Model(zAxis);
        xDrawText(QString("Z"), zAxis, 0.01, 0.15);

        // 坐标轴上 数字
        float val = 0;
        for (size_t cnt = 1; cnt <= 9; cnt++)
        {
            val = cnt * 0.1;

            QVector3D xAxis(val - 0.04, 0, 0);
            Trans2Model(xAxis);
            xDrawText(QString::number(val), xAxis, 0.01, mFontScale);

            QVector3D yAxis(-0.1, val - 0.04, 0);
            Trans2Model(yAxis);
            xDrawText(QString::number(val), yAxis, 0.01, mFontScale);

            QVector3D zAxis(-0.1, 0.00, val - 0.04);
            Trans2Model(zAxis);
            xDrawText(QString::number(val), zAxis, 0.01, mFontScale);
        }
    }
    glDisable(GL_BLEND);
}


void OpenGLWidget::FontScale(int val)
{
    mFontScale = (float)val / 100;
    update();
}


void OpenGLWidget::SceneScale(int val)
{
    if (val < 1)
        val = 1;

    std::cout << "val: " << val << std::endl;
    mSceneScale = ((float)val) / 50;
    mCoordModel.scale(mSceneScale);
    update();
}

void OpenGLWidget::SphereScale(int val)
{
    mSphereFact = ((float)val) / 100;
    update();
}

void OpenGLWidget::SceneTrans(const QVector3D& point)
{
    mCoordModel.translate(point);
    update();
}


void OpenGLWidget::Trans2Model(QVector3D& point)
{
    static QVector4D tmp;
    tmp = mCoordModel * QVector4D(point, 1.0f);
    point = std::move(QVector3D(tmp.x(), tmp.y(), tmp.z()));
}


QVector3D OpenGLWidget::ConvertMousePos2Model(const QPoint& mousePos)
{
    // 1. 转换为NDC坐标
    float ndcX = glData::orthoRight * mousePos.x() / width() - 1.0f;
    float ndcY = 1.0f - glData::orthoTop * mousePos.y() / height();// Y轴翻转

    // 2. 指定深度（正交投影的Z方向是线性的）
    float ndcZ = 0.0f;// 在近平面和远平面之间，0表示在中间

    // 3. 获取正交投影矩阵的逆矩阵
    QMatrix4x4 inverseProj = mCoordProj.inverted();

    // 4. 转换到世界空间
    QVector4D worldPos = inverseProj * QVector4D(ndcX, ndcY, ndcZ, 1.0f);

    // 5. 齐次除法
    if (worldPos.w() != 0.0f)
        worldPos /= worldPos.w();

    return QVector3D(worldPos.x(), worldPos.y(), worldPos.z());
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();// GLAD
    glEnable(GL_DEPTH_TEST);    // 深度测试

    CoordinateShaderProgram(glData::vertexShaderSource, glData::fragmentShaderSource, mCoordSP);
    glUseProgram(mCoordSP);
    CoordinateVertexInit();
    mCoordUniModel = glGetUniformLocation(mCoordSP, "cModel");
    mCoordUniView = glGetUniformLocation(mCoordSP, "cView");
    mCoordUniProj = glGetUniformLocation(mCoordSP, "cProj");
    mCoordUniColor = glGetUniformLocation(mCoordSP, "inColor");


    CoordinateShaderProgram(glData::TextVertex, glData::TextFragment, mTextSP);
    glUseProgram(mTextSP);
    TextVertexInit();
    mTextUniModel = glGetUniformLocation(mTextSP, "tModel");
    mTextUniProj = glGetUniformLocation(mTextSP, "tProj");
    mTextUniView = glGetUniformLocation(mTextSP, "tView");
    mTextUniText = glGetUniformLocation(mTextSP, "fontTex");
    mTextUniTextColor = glGetUniformLocation(mTextSP, "texColor");

    // 由于 LoadText 没有初始化 opengl 的上下文，
    // 所以在 initializeOpenGLFunctions  之后
    // 在调用 LoadText 的构造， 这样 mLoadText 就有 OpenGL 的上下文了
    mLoadText = new LoadText(48);
    mLoadText->initialize(this);

    mPick->InitOpenGL(this);
    mPick->VertexAttribInit();
    mPick->GenFBO_Tex(width(), height());
    // mPick->GenFBO_Tex(mSize.width(), mSize.height());

    mCoordView = std::move(QMatrix4x4(glData::viewMatrix));

    mCoordProj.ortho(glData::orthoLeft, glData::orthoRight,
                     glData::orthoBottom, glData::orthoTop,
                     glData::orthoNear, glData::orthoFar);

    // glViewport(0, 0, mSize.width(), mSize.height());
}


void OpenGLWidget::resizeGL(int w, int h)
{
    // mPick->Resize(mSize.width(), mSize.height());
    // glViewport(0, 0, mSize.width(), mSize.height());
}


void OpenGLWidget::paintGL()
{
    // 计算鼠标拖动场景旋转操作
    static float zAxis = 0;
    zAxis = xClamp((mMouseMoveDir.y() + 0.01f) / (mMouseMoveDir.x() + 0.01f), 0.01f, .099f);
    if (mMouseLastDir != mMouseMoveDir)
        mCoordModel.rotate(1.0f, QVector3D(mMouseMoveDir.y(), mMouseMoveDir.x(), zAxis));
    mMouseLastDir = mMouseMoveDir;

    // 显示鼠标可以操控的标签
    DrawTag(mCoordSP, mPick->GetFBO(), mPick->GetVAO());

    // 绘制坐标系
    DrawCoordinate();
    DrawPlane();
    DrawSphere();
    DrawFont();
}


void OpenGLWidget::MousePick(GLuint fbo, int x, int y)
{
    static GLint prevFBO = 0;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // 坐标转换 (y -> viewportHeight - y)
    unsigned char pixel[3];
    glReadPixels(x, y, 1, 1,
                 GL_RGB, GL_UNSIGNED_BYTE, pixel);

    std::vector<int> color = {pixel[0], pixel[1], pixel[2]};
    static std::vector<int> tagColor = {(int)(glData::xGrey[0] * 256),
                                        (int)(glData::xGrey[1] * 256),
                                        (int)(glData::xGrey[2] * 256)};
    if (color == tagColor)
    {
        mSwitchColor = true;
        mEnableRotateScene = false;
    }
    else
    {
        mSwitchColor = false;
        mEnableRotateScene = true;
    }
    std::cout << "Color: " << color[0] << ", " << color[1] << ", " << color[2] << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, prevFBO);
}