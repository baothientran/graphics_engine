#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <GLDriver.h>
#include <QOpenGLPaintDevice>
#include <QWindow>
#include "Drawables.h"


class Viewer : public QWindow
{
    Q_OBJECT

public:
    Viewer(int sample, QWindow *parent = nullptr);

    void setAnimating(bool animating);

    virtual void initialize();

    virtual void render(QPainter *painter);

    virtual void render();

public slots:
    void renderLater();

    void renderNow();

protected:
    bool event(QEvent *) override;

    void exposeEvent(QExposeEvent *) override;

private:
    DrawContext _scene;
    bool _initialize;
    bool _animating;
};
#endif // MAINWINDOW_H
