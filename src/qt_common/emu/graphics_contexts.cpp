#include "graphics_contexts.h"
#include "common/settings.h"
#include "common/logging/filter.h"

#include <QOpenGLContext>

OpenGLSharedContext::OpenGLSharedContext(QSurface *surface_)
    : surface{surface_}
{
    QSurfaceFormat format;
    format.setVersion(4, 6);
    format.setProfile(QSurfaceFormat::CompatibilityProfile);
    format.setOption(QSurfaceFormat::FormatOption::DeprecatedFunctions);
    if (Settings::values.renderer_debug) {
        format.setOption(QSurfaceFormat::FormatOption::DebugContext);
    }
    // TODO: expose a setting for buffer value (ie default/single/double/triple)
    format.setSwapBehavior(QSurfaceFormat::DefaultSwapBehavior);
    format.setSwapInterval(0);

    context = std::make_unique<QOpenGLContext>();
    context->setFormat(format);
    if (!context->create()) {
        LOG_ERROR(Frontend, "Unable to create main openGL context");
    }
}

OpenGLSharedContext::OpenGLSharedContext(QOpenGLContext *share_context, QSurface *main_surface)
{
    // disable vsync for any shared contexts
    auto format = share_context->format();
    const int swap_interval = Settings::values.vsync_mode.GetValue()
                                      == Settings::VSyncMode::Immediate
                                  ? 0
                                  : 1;

    format.setSwapInterval(main_surface ? swap_interval : 0);

    context = std::make_unique<QOpenGLContext>();
    context->setShareContext(share_context);
    context->setFormat(format);
    if (!context->create()) {
        LOG_ERROR(Frontend, "Unable to create shared openGL context");
    }

    if (!main_surface) {
        offscreen_surface = std::make_unique<QOffscreenSurface>(nullptr);
        offscreen_surface->setFormat(format);
        offscreen_surface->create();
        surface = offscreen_surface.get();
    } else {
        surface = main_surface;
    }
}

void OpenGLSharedContext::SwapBuffers()
{
    context->swapBuffers(surface);
}

void OpenGLSharedContext::MakeCurrent()
{
    // We can't track the current state of the underlying context in this wrapper class because
    // Qt may make the underlying context not current for one reason or another. In particular,
    // the WebBrowser uses GL, so it seems to conflict if we aren't careful.
    // Instead of always just making the context current (which does not have any caching to
    // check if the underlying context is already current) we can check for the current context
    // in the thread local data by calling `currentContext()` and checking if its ours.
    if (QOpenGLContext::currentContext() != context.get()) {
        context->makeCurrent(surface);
    }
}

void OpenGLSharedContext::DoneCurrent()
{
    context->doneCurrent();
}
