#pragma once

#include <QQuickItem>
#include <QQuickWindow>
#include "core/frontend/graphics_context.h"

class RenderItem : public QQuickItem {
public:
    explicit RenderItem(QQuickItem* parent) : QQuickItem(parent) {
        // TODO
        // setAttribute(Qt::WA_NativeWindow);
        // setAttribute(Qt::WA_PaintOnScreen);
        // if (QtCommon::GetWindowSystemType() == Core::Frontend::WindowSystemType::Wayland) {
        //     setAttribute(Qt::WA_DontCreateNativeAncestors);
        // }
    }

    virtual ~RenderItem() = default;
};

struct OpenGLRenderItem : public RenderItem {
    explicit OpenGLRenderItem(QQuickItem* parent) : RenderItem(parent) {
        window()->setSurfaceType(QWindow::OpenGLSurface);
    }

    void SetContext(std::unique_ptr<Core::Frontend::GraphicsContext>&& context_) {
        context = std::move(context_);
    }

private:
    std::unique_ptr<Core::Frontend::GraphicsContext> context;
};

struct VulkanRenderItem : public RenderItem {
    explicit VulkanRenderItem(QQuickItem* parent) : RenderItem(parent) {
        window()->setSurfaceType(QWindow::VulkanSurface);
    }
};

struct NullRenderItem : public RenderItem {
    explicit NullRenderItem(QQuickItem* parent) : RenderItem(parent) {}
};
