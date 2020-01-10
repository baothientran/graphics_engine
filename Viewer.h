#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <GLDriver.h>
#include <QOpenGLPaintDevice>
#include <QWindow>
#include "Drawables.h"
#include "Plugins.h"


class Viewer : public QWindow
{
    Q_OBJECT

public:
    Viewer(int sample, QWindow *parent = nullptr);

    void setAnimating(bool animating);

    inline DrawContext &getDrawContext() { return _context; }

    virtual void initialize();

    virtual void render(QPainter *painter);

    virtual void render();

signals:
    void onResizeEvent(QResizeEvent *);
    void onKeyPressEvent(QKeyEvent *);
    void onKeyReleaseEvent(QKeyEvent *);
    void onMouseMoveEvent(QMouseEvent *);
    void onMousePressEvent(QMouseEvent *event);
    void onMouseReleaseEvent(QMouseEvent *event);
    void onWheelEvent(QWheelEvent *event);
    void onDragEnterEvent(QDragEnterEvent *event);
    void onDropEvent(QDropEvent *event);

public slots:
    void renderLater();

    void renderNow();

protected:
    bool event(QEvent *) override;

    void exposeEvent(QExposeEvent *) override;

    void resizeEvent(QResizeEvent *event) override;

    void keyPressEvent(QKeyEvent *event) override;

    void keyReleaseEvent(QKeyEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void wheelEvent(QWheelEvent *event) override;


private:
    std::unique_ptr<ViewerPlugin> _cameraControlPlugin;
    std::unique_ptr<ViewerPlugin> _cameraProjectionPlugin;
    std::unique_ptr<ViewerPlugin> _importMeshFilePlugin;


    DrawContext _context;
    bool _initialize;
    bool _animating;
};
#endif // MAINWINDOW_H
