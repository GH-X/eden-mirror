#ifndef RENDERWINDOW_H
#define RENDERWINDOW_H

#include <QQmlEngine>
#include <QQuickWindow>
#include <QTimer>
#include "core/frontend/emu_window.h"
#include "input_common/drivers/tas_input.h"
#include "input_common/drivers/mouse.h"
#include "input_common/main.h"
#include "qt_common/emu/emu_thread.h"

class RenderWindow : public QQuickWindow, public Core::Frontend::EmuWindow
{
    Q_OBJECT
    QML_ELEMENT
public:
    RenderWindow(QQuickWindow *parent,
                 EmuThread *emu_thread,
                 std::shared_ptr<InputCommon::InputSubsystem> input_subsystem);

    static int QtKeyToSwitchKey(Qt::Key qt_key);
    static int QtModifierToSwitchModifier(Qt::KeyboardModifiers qt_modifiers);

    qreal windowPixelRatio() const;

    std::pair<u32, u32> ScaleTouch(const QPointF& pos) const;

    std::unique_ptr<Core::Frontend::GraphicsContext> CreateSharedContext() const override;

    void closeEvent(QCloseEvent* event) override;

    void resizeEvent(QResizeEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

    /// Converts a Qt mouse button into MouseInput mouse button
    static InputCommon::MouseButton QtButtonToMouseButton(Qt::MouseButton button);

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

    void InitializeCamera();
    void FinalizeCamera();

    bool event(QEvent* event) override;

    void focusOutEvent(QFocusEvent* event) override;

    bool InitRenderTarget();

    /// Destroy the previous run's child_widget which should also destroy the child_window
    void ReleaseRenderTarget();

    bool IsLoadingComplete() const;

    void CaptureScreenshot(const QString& screenshot_path);

    /**
     * Instructs the window to re-launch the application using the specified program_index.
     * @param program_index Specifies the index within the application of the program to launch.
     */
    void ExecuteProgram(std::size_t program_index);

    /// Instructs the window to exit the application.
    void Exit();

public slots:
    void OnEmulationStarting(EmuThread* emu_thread_);
    void OnEmulationStopping();
    void OnFramebufferSizeChanged();

signals:
    /// Emitted when the window is closed
    void Closed();
    void FirstFrameDisplayed();
    void ExecuteProgramSignal(std::size_t program_index);
    void ExitSignal();
    void MouseActivity();
    void TasPlaybackStateChanged();

private:
    void TouchBeginEvent(const QTouchEvent* event);
    void TouchUpdateEvent(const QTouchEvent* event);
    void TouchEndEvent();
    void ConstrainMouse();

    void RequestCameraCapture();
    void OnCameraCapture(int requestId, const QImage& img);

    void OnMinimalClientAreaChangeRequest(std::pair<u32, u32> minimal_size) override;

    bool InitializeOpenGL();
    bool InitializeVulkan();
    void InitializeNull();
    bool LoadOpenGL();
    QStringList GetUnsupportedGLExtensions() const;

    EmuThread* emu_thread;
    std::shared_ptr<InputCommon::InputSubsystem> input_subsystem;

    // Main context that will be shared with all other contexts that are requested.
    // If this is used in a shared context setting, then this should not be used directly, but
    // should instead be shared from
    std::shared_ptr<Core::Frontend::GraphicsContext> main_context;

    /// Temporary storage of the screenshot taken
    QImage screenshot_image;

    QByteArray geometry;

    QWidget* child_widget = nullptr;

    bool first_frame = false;
    InputCommon::TasInput::TasState last_tas_state;

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0)) && YUZU_USE_QT_MULTIMEDIA
    bool is_virtual_camera;
    int pending_camera_snapshots;
    std::vector<u32> camera_data;
    std::unique_ptr<QCamera> camera;
    std::unique_ptr<QCameraImageCapture> camera_capture;
    std::unique_ptr<QTimer> camera_timer;
#endif

    QTimer mouse_constrain_timer;

protected:
    void showEvent(QShowEvent* event) override;
    bool eventFilter(QObject* object, QEvent* event) override;
};

#endif // RENDERWINDOW_H
