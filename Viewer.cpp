#include "Viewer.h"

#include <QPainter>


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
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setMajorVersion(4);
    format.setMinorVersion(2);
    format.setSamples(sample);

    setFormat(format);
}


void Viewer::initialize() {
    Effect *effect = _scene.createEffect("forward", {
                                                        {GL_VERTEX_SHADER,   ""},
                                                        {GL_FRAGMENT_SHADER, ""}
                                                    });
    auto root = _scene.getRoot();

    std::vector<int> elements;
    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    auto &geoNode = root->emplaceChild();
    geoNode.getDrawable().setEffectProperty(effect->createProperty());
}


void Viewer::setAnimating(bool animating) {
    _animating = animating;
    if (_animating)
        renderLater();
}


void Viewer::render(QPainter *) {
}


void Viewer::render()
{
    auto driver = _scene.getDriver();
    driver->setSize(size() * devicePixelRatio());
    driver->setDevicePixelRatio(devicePixelRatio());

    QPainter painter = driver->createPainter();
    render(&painter);
}


void Viewer::renderLater() {
    requestUpdate();
}


void Viewer::renderNow() {
    if (!isExposed())
        return;

    auto driver = _scene.getDriver();

    if (!_initialize) {
        driver->initialize(this);
        initialize();

        _initialize = true;
    }

    driver->makeCurrent(this);

    render();

    driver->swapBuffers(this);

    if (_animating)
        renderLater();
}


bool Viewer::event(QEvent *e) {
    switch (e->type()) {
    case QEvent::UpdateRequest:
        renderNow();
        return true;
    default:
        return QWindow::event(e);
    }
}


void Viewer::exposeEvent(QExposeEvent *) {
    if (isExposed())
        renderNow();
}

