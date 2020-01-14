#include <QPainter>
#include "Viewer.h"
#include "Plugins.h"
#include "Effects.h"



/***************************************************
 * Viewer definitions
 ***************************************************/
Viewer::Viewer(int sample, QWindow *parent)
    : QWindow(parent),
    _initialize{false},
    _animating{false}
{
    setSurfaceType(QWindow::OpenGLSurface);

    QSurfaceFormat format;
    format.setSamples(sample);
    format.setDepthBufferSize(1);
    format.setStencilBufferSize(1);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setMajorVersion(4);
    format.setMinorVersion(2);
    format.setProfile(QSurfaceFormat::CoreProfile);

    setFormat(format);
}


void Viewer::initialize() {
    // initialize effects
    _context.createEffect<ColorEffect>(ColorEffect::EFFECT_NAME);
    _context.createEffect<ForwardPhongEffect>(ForwardPhongEffect::EFFECT_NAME);

    // initialize plugins
    _cameraControlPlugin = std::make_unique<OrbitCameraPlugin>(this);
    _cameraProjectionPlugin = std::make_unique<PerspectiveCameraPlugin>(this);
    _importMeshFilePlugin = std::make_unique<ImportMeshFilePlugin>(this);

    // initialize scene
    auto &root = _context.getRoot();
    auto pointLight = _context.createDrawable<PointLight>(glm::vec3(2.0f), 1000.0f);
    auto &lightNode = root.emplaceChild(std::move(pointLight));
    lightNode.position() = glm::vec3{250.0f, 250.0f, 250.0f};
}


void Viewer::setAnimating(bool animating) {
    _animating = animating;
    if (_animating)
        renderLater();
}


void Viewer::render(QPainter *) {
    // reset drivers
    auto &driver = _context.getDriver();
    driver.clearBufferBit(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    driver.clearColor({0.23f, 0.23f, 0.23f, 1.0f});
    driver.setViewport(0, 0, width(), height());

    _context.getRoot().draw();
}


void Viewer::render() {
    auto &driver = _context.getDriver();
    driver.setSize(size() * devicePixelRatio());
    driver.setDevicePixelRatio(devicePixelRatio());

    QPainter painter = driver.createPainter();
    render(&painter);
}


void Viewer::renderLater() {
    requestUpdate();
}


void Viewer::renderNow() {
    if (!isExposed())
        return;

    auto &driver = _context.getDriver();

    if (!_initialize) {
        driver.initialize(this);
        initialize();

        _initialize = true;
    }

    driver.makeCurrent(this);
    render();
    driver.swapBuffers(this);

    if (_animating)
        renderLater();
}


bool Viewer::event(QEvent *e) {
    switch (e->type()) {
    case QEvent::UpdateRequest:
        renderNow();
        return true;
    case QEvent::DragEnter:
        emit onDragEnterEvent(static_cast<QDragEnterEvent *>(e));
        return true;
    case QEvent::Drop:
        emit onDropEvent(static_cast<QDropEvent *>(e));
        return true;
    default:
        return QWindow::event(e);
    }
}


void Viewer::exposeEvent(QExposeEvent *) {
    if (isExposed())
        renderNow();
}


void Viewer::resizeEvent(QResizeEvent *event) {
    emit onResizeEvent(event);
}


void Viewer::keyPressEvent(QKeyEvent *event) {
    emit onKeyPressEvent(event);
}


void Viewer::keyReleaseEvent(QKeyEvent *event) {
    emit onKeyReleaseEvent(event);
}


void Viewer::mouseMoveEvent(QMouseEvent *event) {
    emit onMouseMoveEvent(event);
}


void Viewer::mousePressEvent(QMouseEvent *event) {
    emit onMousePressEvent(event);
}


void Viewer::mouseReleaseEvent(QMouseEvent *event) {
    emit onMouseReleaseEvent(event);
}


void Viewer::wheelEvent(QWheelEvent *event) {
    emit onWheelEvent(event);
}

