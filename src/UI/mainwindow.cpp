#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qvector3d.h>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->horizontalSlider_4->setRange(-40, 40);
    ui->horizontalSlider_5->setRange(-40, 40);
    ui->horizontalSlider_6->setRange(-40, 40);

    ui->horizontalSlider_4->setValue(0);
    ui->horizontalSlider_5->setValue(0);
    ui->horizontalSlider_6->setValue(0);

    connect(ui->horizontalSlider, &QSlider::valueChanged, ui->openGLWidget, [this]() {
        ui->openGLWidget->FontScale(ui->horizontalSlider->value());
    });


    connect(ui->horizontalSlider_2, &QSlider::valueChanged, ui->openGLWidget, [this]() {
        ui->openGLWidget->SphereScale(ui->horizontalSlider_2->value());
    });


    connect(ui->horizontalSlider_3, &QSlider::valueChanged, ui->openGLWidget, [this]() {
        ui->openGLWidget->SceneScale(ui->horizontalSlider_3->value());
    });


    //////////////////////////////////////// 设置 平移
    connect(ui->horizontalSlider_4, &QSlider::valueChanged, ui->openGLWidget, [this]() {
        float x = (float)ui->horizontalSlider_4->value() / 10;
        float y = (float)ui->horizontalSlider_5->value() / 10;
        float z = (float)ui->horizontalSlider_6->value() / 10;

        QVector3D point(x, y, z);
        ui->openGLWidget->SceneTrans(point);
    });

    connect(ui->horizontalSlider_5, &QSlider::valueChanged, ui->openGLWidget, [this]() {
        float x = (float)ui->horizontalSlider_4->value() / 10;
        float y = (float)ui->horizontalSlider_5->value() / 10;
        float z = (float)ui->horizontalSlider_6->value() / 10;

        QVector3D point(x, y, z);
        ui->openGLWidget->SceneTrans(point);
    });

    connect(ui->horizontalSlider_6, &QSlider::valueChanged, ui->openGLWidget, [this]() {
        float x = (float)ui->horizontalSlider_4->value() / 10;
        float y = (float)ui->horizontalSlider_5->value() / 10;
        float z = (float)ui->horizontalSlider_6->value() / 10;

        QVector3D point(x, y, z);
        ui->openGLWidget->SceneTrans(point);
    });
}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
        close();
    else
        QWidget::keyPressEvent(event);
}

void MainWindow::ProgressBarInit()
{
    ui->horizontalSlider_4->setRange(-200, 200);
    ui->horizontalSlider_4->setValue(0);

    ui->horizontalSlider_5->setRange(-200, 200);
    ui->horizontalSlider_5->setValue(0);


    ui->horizontalSlider_6->setRange(-200, 200);
    ui->horizontalSlider_6->setValue(0);
}

void MainWindow::HideAll()
{
    ui->label->hide();
    ui->label_2->hide();
    ui->label_3->hide();
    ui->label_4->hide();
    ui->label_5->hide();
    ui->label_6->hide();
}
