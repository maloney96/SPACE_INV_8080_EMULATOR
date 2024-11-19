#include "glwidget.h"
#include <QVector2D>
#include <QDebug>

GLWidget::GLWidget(QWidget *parent)
    : QOpenGLWidget(parent), vertexBuffer(QOpenGLBuffer::VertexBuffer)
{
}

GLWidget::~GLWidget()
{
    makeCurrent();
    vertexBuffer.destroy();
    doneCurrent();
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    // Set up OpenGL clear color (background color)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // Black background

    // Compile and link shaders
    if (!shaderProgram.addShaderFromSourceCode(QOpenGLShader::Vertex,
                                               R"(
            #version 330
            in vec2 position;
            void main() {
                gl_Position = vec4(position, 0.0, 1.0);
            }
        )"))
        qDebug() << "Vertex shader compilation failed.";

    if (!shaderProgram.addShaderFromSourceCode(QOpenGLShader::Fragment,
                                               R"(
            #version 330
            uniform float pixelState;
            out vec4 fragColor;
            void main() {
                fragColor = vec4(vec3(pixelState), 1.0);  // White if pixelState is 1, black otherwise
            }
        )"))
        qDebug() << "Fragment shader compilation failed.";

    if (!shaderProgram.link())
        qDebug() << "Shader program linking failed.";

    // Set up vertex buffer
    vertexBuffer.create();
    vertexBuffer.setUsagePattern(QOpenGLBuffer::DynamicDraw);
}

void GLWidget::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);

    if (!videoEmulator) return;  // If no emulator is set, skip rendering

    updatePixelData();  // Update pixel data from video memory

    // Bind and set up the shader program
    shaderProgram.bind();
    vertexBuffer.bind();

    shaderProgram.enableAttributeArray("position");
    shaderProgram.setAttributeBuffer("position", GL_FLOAT, 0, 2, sizeof(QVector2D));

    // Render each pixel in the buffer
    for (int i = 0; i < pixelVertices.size(); ++i) {
        float pixelState = VideoEmulator_getPixel(videoEmulator, pixelVertices[i].x(), pixelVertices[i].y());
        shaderProgram.setUniformValue("pixelState", pixelState);
        glDrawArrays(GL_POINTS, i, 1);  // Draw each point individually
    }

    shaderProgram.disableAttributeArray("position");
    shaderProgram.release();
    vertexBuffer.release();
}

void GLWidget::renderFrame(VideoEmulator *emulator)
{
    videoEmulator = emulator;
    update();  // Trigger a repaint
}

void GLWidget::updatePixelData()
{
    pixelVertices.clear();
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            if (VideoEmulator_getPixel(videoEmulator, x, y) == 1) {  // Only add "on" pixels
                float normalizedX = (float(x) / SCREEN_WIDTH) * 2.0f - 1.0f;
                float normalizedY = 1.0f - (float(y) / SCREEN_HEIGHT) * 2.0f;
                pixelVertices.append(QVector2D(normalizedX, normalizedY));
            }
        }
    }

    // Update vertex buffer with new pixel data
    vertexBuffer.bind();
    vertexBuffer.allocate(pixelVertices.constData(), pixelVertices.size() * sizeof(QVector2D));
    vertexBuffer.release();
}
