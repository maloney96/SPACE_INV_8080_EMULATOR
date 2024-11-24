#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include "../outputmanager/videoemulator.h"

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT

public:
    explicit GLWidget(QWidget *parent = nullptr);
    ~GLWidget();

    // Set the video memory emulator to render
    void renderFrame(VideoEmulator *emulator);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:
    VideoEmulator *videoEmulator = nullptr;
    QOpenGLShaderProgram shaderProgram;  // Shader program for rendering
    QOpenGLBuffer vertexBuffer;          // Vertex buffer for pixel data
    QVector<QVector2D> pixelVertices;    // Store pixel vertices

    void updatePixelData();  // Helper to update pixel data from video memory
};

#endif // GLWIDGET_H
