#ifndef GRAPHICS_CONTEXTS_H
#define GRAPHICS_CONTEXTS_H

#include <QOffscreenSurface>
#include <QSurface>
#include "core/frontend/graphics_context.h"

#ifdef HAS_OPENGL

class OpenGLSharedContext : public Core::Frontend::GraphicsContext
{
public:
    /// Create the original context that should be shared from
    explicit OpenGLSharedContext(QSurface* surface_);

    /// Create the shared contexts for rendering and presentation
    explicit OpenGLSharedContext(QOpenGLContext* share_context, QSurface* main_surface = nullptr);

    ~OpenGLSharedContext() {
        DoneCurrent();
    }

    void SwapBuffers() override;

    void MakeCurrent() override;

    void DoneCurrent() override;

    QOpenGLContext* GetShareContext() {
        return context.get();
    }

    const QOpenGLContext* GetShareContext() const {
        return context.get();
    }

private:
    // Avoid using Qt parent system here since we might move the QObjects to new threads
    // As a note, this means we should avoid using slots/signals with the objects too
    std::unique_ptr<QOpenGLContext> context;
    std::unique_ptr<QOffscreenSurface> offscreen_surface{};
    QSurface* surface;
};
#endif

class DummyContext : public Core::Frontend::GraphicsContext {};

#endif // GRAPHICS_CONTEXTS_H
