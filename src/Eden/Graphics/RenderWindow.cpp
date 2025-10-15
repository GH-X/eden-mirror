#include "RenderWindow.h"
#include "RenderItems.h"
#include "common/scm_rev.h"
#include "input_common/drivers/keyboard.h"
#include "input_common/drivers/touch_screen.h"
#include "qt_common/emu/graphics_contexts.h"
#include "qt_common/qt_common.h"

RenderWindow::RenderWindow(QQuickWindow *parent,
                           EmuThread *emu_thread,
                           std::shared_ptr<InputCommon::InputSubsystem> input_subsystem)
    : QQuickWindow(parent)
    , emu_thread(emu_thread)
    , input_subsystem{std::move(input_subsystem)}
{
    setTitle(QStringLiteral("Eden %1 | %2-%3")
                 .arg(QString::fromUtf8(Common::g_build_name),
                      QString::fromUtf8(Common::g_scm_branch),
                      QString::fromUtf8(Common::g_scm_desc)));

    strict_context_required = QGuiApplication::platformName() == QStringLiteral("wayland") ||
                              QGuiApplication::platformName() == QStringLiteral("wayland-egl");

    // connect(this, &RenderWindow::FirstFrameDisplayed, parent, &GMainWindow::OnLoadComplete);
    // connect(this, &RenderWindow::ExecuteProgramSignal, parent, &GMainWindow::OnExecuteProgram,
    //         Qt::QueuedConnection);
    // connect(this, &RenderWindow::ExitSignal, parent, &GMainWindow::OnExit, Qt::QueuedConnection);
    // connect(this, &RenderWindow::TasPlaybackStateChanged, parent, &GMainWindow::OnTasStateChanged);

    // mouse_constrain_timer.setInterval(default_mouse_constrain_timeout);
    // connect(&mouse_constrain_timer, &QTimer::timeout, this, &RenderWindow::ConstrainMouse);
}

std::pair<u32, u32> RenderWindow::ScaleTouch(const QPointF& pos) const {
    const qreal pixel_ratio = windowPixelRatio();
    return {static_cast<u32>((std::max)(std::round(pos.x() * pixel_ratio), qreal{0.0})),
            static_cast<u32>((std::max)(std::round(pos.y() * pixel_ratio), qreal{0.0}))};
}

void RenderWindow::closeEvent(QCloseEvent* event) {
    emit Closed();
    QQuickWindow::closeEvent(event);
}

int RenderWindow::QtKeyToSwitchKey(Qt::Key qt_key) {
    static constexpr std::array<std::pair<Qt::Key, Settings::NativeKeyboard::Keys>, 106> key_map = {
        std::pair<Qt::Key, Settings::NativeKeyboard::Keys>{Qt::Key_A, Settings::NativeKeyboard::A},
        {Qt::Key_A, Settings::NativeKeyboard::A},
        {Qt::Key_B, Settings::NativeKeyboard::B},
        {Qt::Key_C, Settings::NativeKeyboard::C},
        {Qt::Key_D, Settings::NativeKeyboard::D},
        {Qt::Key_E, Settings::NativeKeyboard::E},
        {Qt::Key_F, Settings::NativeKeyboard::F},
        {Qt::Key_G, Settings::NativeKeyboard::G},
        {Qt::Key_H, Settings::NativeKeyboard::H},
        {Qt::Key_I, Settings::NativeKeyboard::I},
        {Qt::Key_J, Settings::NativeKeyboard::J},
        {Qt::Key_K, Settings::NativeKeyboard::K},
        {Qt::Key_L, Settings::NativeKeyboard::L},
        {Qt::Key_M, Settings::NativeKeyboard::M},
        {Qt::Key_N, Settings::NativeKeyboard::N},
        {Qt::Key_O, Settings::NativeKeyboard::O},
        {Qt::Key_P, Settings::NativeKeyboard::P},
        {Qt::Key_Q, Settings::NativeKeyboard::Q},
        {Qt::Key_R, Settings::NativeKeyboard::R},
        {Qt::Key_S, Settings::NativeKeyboard::S},
        {Qt::Key_T, Settings::NativeKeyboard::T},
        {Qt::Key_U, Settings::NativeKeyboard::U},
        {Qt::Key_V, Settings::NativeKeyboard::V},
        {Qt::Key_W, Settings::NativeKeyboard::W},
        {Qt::Key_X, Settings::NativeKeyboard::X},
        {Qt::Key_Y, Settings::NativeKeyboard::Y},
        {Qt::Key_Z, Settings::NativeKeyboard::Z},
        {Qt::Key_1, Settings::NativeKeyboard::N1},
        {Qt::Key_2, Settings::NativeKeyboard::N2},
        {Qt::Key_3, Settings::NativeKeyboard::N3},
        {Qt::Key_4, Settings::NativeKeyboard::N4},
        {Qt::Key_5, Settings::NativeKeyboard::N5},
        {Qt::Key_6, Settings::NativeKeyboard::N6},
        {Qt::Key_7, Settings::NativeKeyboard::N7},
        {Qt::Key_8, Settings::NativeKeyboard::N8},
        {Qt::Key_9, Settings::NativeKeyboard::N9},
        {Qt::Key_0, Settings::NativeKeyboard::N0},
        {Qt::Key_Return, Settings::NativeKeyboard::Return},
        {Qt::Key_Escape, Settings::NativeKeyboard::Escape},
        {Qt::Key_Backspace, Settings::NativeKeyboard::Backspace},
        {Qt::Key_Tab, Settings::NativeKeyboard::Tab},
        {Qt::Key_Space, Settings::NativeKeyboard::Space},
        {Qt::Key_Minus, Settings::NativeKeyboard::Minus},
        {Qt::Key_Plus, Settings::NativeKeyboard::Plus},
        {Qt::Key_questiondown, Settings::NativeKeyboard::Plus},
        {Qt::Key_BracketLeft, Settings::NativeKeyboard::OpenBracket},
        {Qt::Key_BraceLeft, Settings::NativeKeyboard::OpenBracket},
        {Qt::Key_BracketRight, Settings::NativeKeyboard::CloseBracket},
        {Qt::Key_BraceRight, Settings::NativeKeyboard::CloseBracket},
        {Qt::Key_Bar, Settings::NativeKeyboard::Pipe},
        {Qt::Key_Dead_Tilde, Settings::NativeKeyboard::Tilde},
        {Qt::Key_Ntilde, Settings::NativeKeyboard::Semicolon},
        {Qt::Key_Semicolon, Settings::NativeKeyboard::Semicolon},
        {Qt::Key_Apostrophe, Settings::NativeKeyboard::Quote},
        {Qt::Key_Dead_Grave, Settings::NativeKeyboard::Backquote},
        {Qt::Key_Comma, Settings::NativeKeyboard::Comma},
        {Qt::Key_Period, Settings::NativeKeyboard::Period},
        {Qt::Key_Slash, Settings::NativeKeyboard::Slash},
        {Qt::Key_CapsLock, Settings::NativeKeyboard::CapsLockKey},
        {Qt::Key_F1, Settings::NativeKeyboard::F1},
        {Qt::Key_F2, Settings::NativeKeyboard::F2},
        {Qt::Key_F3, Settings::NativeKeyboard::F3},
        {Qt::Key_F4, Settings::NativeKeyboard::F4},
        {Qt::Key_F5, Settings::NativeKeyboard::F5},
        {Qt::Key_F6, Settings::NativeKeyboard::F6},
        {Qt::Key_F7, Settings::NativeKeyboard::F7},
        {Qt::Key_F8, Settings::NativeKeyboard::F8},
        {Qt::Key_F9, Settings::NativeKeyboard::F9},
        {Qt::Key_F10, Settings::NativeKeyboard::F10},
        {Qt::Key_F11, Settings::NativeKeyboard::F11},
        {Qt::Key_F12, Settings::NativeKeyboard::F12},
        {Qt::Key_Print, Settings::NativeKeyboard::PrintScreen},
        {Qt::Key_ScrollLock, Settings::NativeKeyboard::ScrollLockKey},
        {Qt::Key_Pause, Settings::NativeKeyboard::Pause},
        {Qt::Key_Insert, Settings::NativeKeyboard::Insert},
        {Qt::Key_Home, Settings::NativeKeyboard::Home},
        {Qt::Key_PageUp, Settings::NativeKeyboard::PageUp},
        {Qt::Key_Delete, Settings::NativeKeyboard::Delete},
        {Qt::Key_End, Settings::NativeKeyboard::End},
        {Qt::Key_PageDown, Settings::NativeKeyboard::PageDown},
        {Qt::Key_Right, Settings::NativeKeyboard::Right},
        {Qt::Key_Left, Settings::NativeKeyboard::Left},
        {Qt::Key_Down, Settings::NativeKeyboard::Down},
        {Qt::Key_Up, Settings::NativeKeyboard::Up},
        {Qt::Key_NumLock, Settings::NativeKeyboard::NumLockKey},
        // Numpad keys are missing here
        {Qt::Key_F13, Settings::NativeKeyboard::F13},
        {Qt::Key_F14, Settings::NativeKeyboard::F14},
        {Qt::Key_F15, Settings::NativeKeyboard::F15},
        {Qt::Key_F16, Settings::NativeKeyboard::F16},
        {Qt::Key_F17, Settings::NativeKeyboard::F17},
        {Qt::Key_F18, Settings::NativeKeyboard::F18},
        {Qt::Key_F19, Settings::NativeKeyboard::F19},
        {Qt::Key_F20, Settings::NativeKeyboard::F20},
        {Qt::Key_F21, Settings::NativeKeyboard::F21},
        {Qt::Key_F22, Settings::NativeKeyboard::F22},
        {Qt::Key_F23, Settings::NativeKeyboard::F23},
        {Qt::Key_F24, Settings::NativeKeyboard::F24},
        // {Qt::..., Settings::NativeKeyboard::KPComma},
        // {Qt::..., Settings::NativeKeyboard::Ro},
        {Qt::Key_Hiragana_Katakana, Settings::NativeKeyboard::KatakanaHiragana},
        {Qt::Key_yen, Settings::NativeKeyboard::Yen},
        {Qt::Key_Henkan, Settings::NativeKeyboard::Henkan},
        {Qt::Key_Muhenkan, Settings::NativeKeyboard::Muhenkan},
        // {Qt::..., Settings::NativeKeyboard::NumPadCommaPc98},
        {Qt::Key_Hangul, Settings::NativeKeyboard::HangulEnglish},
        {Qt::Key_Hangul_Hanja, Settings::NativeKeyboard::Hanja},
        {Qt::Key_Katakana, Settings::NativeKeyboard::KatakanaKey},
        {Qt::Key_Hiragana, Settings::NativeKeyboard::HiraganaKey},
        {Qt::Key_Zenkaku_Hankaku, Settings::NativeKeyboard::ZenkakuHankaku},
        // Modifier keys are handled by the modifier property
    };

    for (const auto& [qkey, nkey] : key_map) {
        if (qt_key == qkey) {
            return nkey;
        }
    }

    return Settings::NativeKeyboard::None;
}

int RenderWindow::QtModifierToSwitchModifier(Qt::KeyboardModifiers qt_modifiers) {
    int modifier = 0;

    if ((qt_modifiers & Qt::KeyboardModifier::ShiftModifier) != 0) {
        modifier |= 1 << Settings::NativeKeyboard::LeftShift;
    }
    if ((qt_modifiers & Qt::KeyboardModifier::ControlModifier) != 0) {
        modifier |= 1 << Settings::NativeKeyboard::LeftControl;
    }
    if ((qt_modifiers & Qt::KeyboardModifier::AltModifier) != 0) {
        modifier |= 1 << Settings::NativeKeyboard::LeftAlt;
    }
    if ((qt_modifiers & Qt::KeyboardModifier::MetaModifier) != 0) {
        modifier |= 1 << Settings::NativeKeyboard::LeftMeta;
    }

    // TODO: These keys can't be obtained with Qt::KeyboardModifier

    // if ((qt_modifiers & 0x10) != 0) {
    //    modifier |= 1 << Settings::NativeKeyboard::RightShift;
    // }
    // if ((qt_modifiers & 0x20) != 0) {
    //    modifier |= 1 << Settings::NativeKeyboard::RightControl;
    // }
    // if ((qt_modifiers & 0x40) != 0) {
    //    modifier |= 1 << Settings::NativeKeyboard::RightAlt;
    // }
    // if ((qt_modifiers & 0x80) != 0) {
    //    modifier |= 1 << Settings::NativeKeyboard::RightMeta;
    // }
    // if ((qt_modifiers & 0x100) != 0) {
    //    modifier |= 1 << Settings::NativeKeyboard::CapsLock;
    // }
    // if ((qt_modifiers & 0x200) != 0) {
    //    modifier |= 1 << Settings::NativeKeyboard::NumLock;
    // }
    // if ((qt_modifiers & ???) != 0) {
    //    modifier |= 1 << Settings::NativeKeyboard::ScrollLock;
    // }
    // if ((qt_modifiers & ???) != 0) {
    //    modifier |= 1 << Settings::NativeKeyboard::Katakana;
    // }
    // if ((qt_modifiers & ???) != 0) {
    //    modifier |= 1 << Settings::NativeKeyboard::Hiragana;
    // }
    return modifier;
}

void RenderWindow::keyPressEvent(QKeyEvent* event) {
    /**
     * This feature can be enhanced with the following functions, but they do not provide
     * cross-platform behavior.
     *
     * event->nativeVirtualKey() can distinguish between keys on the numpad.
     * event->nativeModifiers() can distinguish between left and right keys and numlock,
     * capslock, scroll lock.
     */
    if (!event->isAutoRepeat()) {
        const auto modifier = QtModifierToSwitchModifier(event->modifiers());
        const auto key = QtKeyToSwitchKey(Qt::Key(event->key()));
        input_subsystem->GetKeyboard()->SetKeyboardModifiers(modifier);
        input_subsystem->GetKeyboard()->PressKeyboardKey(key);
        // This is used for gamepads that can have any key mapped
        input_subsystem->GetKeyboard()->PressKey(event->key());
    }
}

void RenderWindow::keyReleaseEvent(QKeyEvent* event) {
    /**
     * This feature can be enhanced with the following functions, but they do not provide
     * cross-platform behavior.
     *
     * event->nativeVirtualKey() can distinguish between keys on the numpad.
     * event->nativeModifiers() can distinguish between left and right buttons and numlock,
     * capslock, scroll lock.
     */
    if (!event->isAutoRepeat()) {
        const auto modifier = QtModifierToSwitchModifier(event->modifiers());
        const auto key = QtKeyToSwitchKey(Qt::Key(event->key()));
        input_subsystem->GetKeyboard()->SetKeyboardModifiers(modifier);
        input_subsystem->GetKeyboard()->ReleaseKeyboardKey(key);
        // This is used for gamepads that can have any key mapped
        input_subsystem->GetKeyboard()->ReleaseKey(event->key());
    }
}

InputCommon::MouseButton RenderWindow::QtButtonToMouseButton(Qt::MouseButton button) {
    switch (button) {
    case Qt::LeftButton:
        return InputCommon::MouseButton::Left;
    case Qt::RightButton:
        return InputCommon::MouseButton::Right;
    case Qt::MiddleButton:
        return InputCommon::MouseButton::Wheel;
    case Qt::BackButton:
        return InputCommon::MouseButton::Backward;
    case Qt::ForwardButton:
        return InputCommon::MouseButton::Forward;
    case Qt::TaskButton:
        return InputCommon::MouseButton::Task;
    default:
        return InputCommon::MouseButton::Extra;
    }
}

void RenderWindow::mousePressEvent(QMouseEvent* event) {
    // Touch input is handled in TouchBeginEvent
    if (event->source() == Qt::MouseEventSynthesizedBySystem) {
        return;
    }
    // Qt sometimes returns the parent coordinates. To avoid this we read the global mouse
    // coordinates and map them to the current render area
    const auto pos = mapFromGlobal(QCursor::pos());
    const auto [x, y] = ScaleTouch(pos);
    const auto [touch_x, touch_y] = MapToTouchScreen(x, y);
    const auto button = QtButtonToMouseButton(event->button());

    input_subsystem->GetMouse()->PressMouseButton(button);
    input_subsystem->GetMouse()->PressButton(pos.x(), pos.y(), button);
    input_subsystem->GetMouse()->PressTouchButton(touch_x, touch_y, button);

    emit MouseActivity();
}

void RenderWindow::mouseMoveEvent(QMouseEvent* event) {
    // Touch input is handled in TouchUpdateEvent
    if (event->source() == Qt::MouseEventSynthesizedBySystem) {
        return;
    }
    // Qt sometimes returns the parent coordinates. To avoid this we read the global mouse
    // coordinates and map them to the current render area
    const auto pos = mapFromGlobal(QCursor::pos());
    const auto [x, y] = ScaleTouch(pos);
    const auto [touch_x, touch_y] = MapToTouchScreen(x, y);
    const int center_x = width() / 2;
    const int center_y = height() / 2;

    input_subsystem->GetMouse()->MouseMove(touch_x, touch_y);
    input_subsystem->GetMouse()->TouchMove(touch_x, touch_y);
    input_subsystem->GetMouse()->Move(pos.x(), pos.y(), center_x, center_y);

    // Center mouse for mouse panning
    if (Settings::values.mouse_panning && !Settings::values.mouse_enabled) {
        QCursor::setPos(mapToGlobal(QPoint{center_x, center_y}));
    }

    // Constrain mouse for mouse emulation with mouse panning
    if (Settings::values.mouse_panning && Settings::values.mouse_enabled) {
        const auto [clamped_mouse_x, clamped_mouse_y] = ClipToTouchScreen(x, y);
        QCursor::setPos(mapToGlobal(
            QPoint{static_cast<int>(clamped_mouse_x), static_cast<int>(clamped_mouse_y)}));
    }

    mouse_constrain_timer.stop();
    emit MouseActivity();
}

void RenderWindow::mouseReleaseEvent(QMouseEvent* event) {
    // Touch input is handled in TouchEndEvent
    if (event->source() == Qt::MouseEventSynthesizedBySystem) {
        return;
    }

    const auto button = QtButtonToMouseButton(event->button());
    input_subsystem->GetMouse()->ReleaseButton(button);
}

void RenderWindow::ConstrainMouse() {
    if (emu_thread == nullptr || !Settings::values.mouse_panning) {
        mouse_constrain_timer.stop();
        return;
    }
    if (!this->isActive()) {
        mouse_constrain_timer.stop();
        return;
    }

    if (Settings::values.mouse_enabled) {
        const auto pos = mapFromGlobal(QCursor::pos());
        const int new_pos_x = std::clamp(pos.x(), 0, width());
        const int new_pos_y = std::clamp(pos.y(), 0, height());

        QCursor::setPos(mapToGlobal(QPoint{new_pos_x, new_pos_y}));
        return;
    }

    const int center_x = width() / 2;
    const int center_y = height() / 2;

    QCursor::setPos(mapToGlobal(QPoint{center_x, center_y}));
}

void RenderWindow::wheelEvent(QWheelEvent* event) {
    const int x = event->angleDelta().x();
    const int y = event->angleDelta().y();
    input_subsystem->GetMouse()->MouseWheelChange(x, y);
}

void RenderWindow::TouchBeginEvent(const QTouchEvent* event) {
    QList<QTouchEvent::TouchPoint> touch_points = event->points();
    for (const auto& touch_point : touch_points) {
        const auto [x, y] = ScaleTouch(touch_point.position());
        const auto [touch_x, touch_y] = MapToTouchScreen(x, y);
        input_subsystem->GetTouchScreen()->TouchPressed(touch_x, touch_y, touch_point.id());
    }
}

void RenderWindow::TouchUpdateEvent(const QTouchEvent* event) {
    QList<QTouchEvent::TouchPoint> touch_points = event->points();
    input_subsystem->GetTouchScreen()->ClearActiveFlag();
    for (const auto& touch_point : touch_points) {
        const auto [x, y] = ScaleTouch(touch_point.position());
        const auto [touch_x, touch_y] = MapToTouchScreen(x, y);
        input_subsystem->GetTouchScreen()->TouchMoved(touch_x, touch_y, touch_point.id());
    }
    input_subsystem->GetTouchScreen()->ReleaseInactiveTouch();
}

void RenderWindow::TouchEndEvent() {
    input_subsystem->GetTouchScreen()->ReleaseAllTouch();
}

void RenderWindow::InitializeCamera() {
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0)) && YUZU_USE_QT_MULTIMEDIA
    constexpr auto camera_update_ms = std::chrono::milliseconds{50}; // (50ms, 20Hz)
    if (!Settings::values.enable_ir_sensor) {
        return;
    }

    bool camera_found = false;
    const QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
    for (const QCameraInfo& cameraInfo : cameras) {
        if (Settings::values.ir_sensor_device.GetValue() == cameraInfo.deviceName().toStdString() ||
            Settings::values.ir_sensor_device.GetValue() == "Auto") {
            camera = std::make_unique<QCamera>(cameraInfo);
            if (!camera->isCaptureModeSupported(QCamera::CaptureMode::CaptureViewfinder) &&
                !camera->isCaptureModeSupported(QCamera::CaptureMode::CaptureStillImage)) {
                LOG_ERROR(Frontend,
                          "Camera doesn't support CaptureViewfinder or CaptureStillImage");
                continue;
            }
            camera_found = true;
            break;
        }
    }

    if (!camera_found) {
        return;
    }

    camera_capture = std::make_unique<QCameraImageCapture>(camera.get());

    if (!camera_capture->isCaptureDestinationSupported(
            QCameraImageCapture::CaptureDestination::CaptureToBuffer)) {
        LOG_ERROR(Frontend, "Camera doesn't support saving to buffer");
        return;
    }

    const auto camera_width = input_subsystem->GetCamera()->getImageWidth();
    const auto camera_height = input_subsystem->GetCamera()->getImageHeight();
    camera_data.resize(camera_width * camera_height);
    camera_capture->setCaptureDestination(QCameraImageCapture::CaptureDestination::CaptureToBuffer);
    connect(camera_capture.get(), &QCameraImageCapture::imageCaptured, this,
            &RenderWindow::OnCameraCapture);
    camera->unload();
    if (camera->isCaptureModeSupported(QCamera::CaptureMode::CaptureViewfinder)) {
        camera->setCaptureMode(QCamera::CaptureViewfinder);
    } else if (camera->isCaptureModeSupported(QCamera::CaptureMode::CaptureStillImage)) {
        camera->setCaptureMode(QCamera::CaptureStillImage);
    }
    camera->load();
    camera->start();

    pending_camera_snapshots = 0;
    is_virtual_camera = false;

    camera_timer = std::make_unique<QTimer>();
    connect(camera_timer.get(), &QTimer::timeout, [this] { RequestCameraCapture(); });
    // This timer should be dependent of camera resolution 5ms for every 100 pixels
    camera_timer->start(camera_update_ms);
#endif
}

void RenderWindow::FinalizeCamera() {
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0)) && YUZU_USE_QT_MULTIMEDIA
    if (camera_timer) {
        camera_timer->stop();
    }
    if (camera) {
        camera->unload();
    }
#endif
}

void RenderWindow::RequestCameraCapture() {
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0)) && YUZU_USE_QT_MULTIMEDIA
    if (!Settings::values.enable_ir_sensor) {
        return;
    }

    // If the camera doesn't capture, test for virtual cameras
    if (pending_camera_snapshots > 5) {
        is_virtual_camera = true;
    }
    // Virtual cameras like obs need to reset the camera every capture
    if (is_virtual_camera) {
        camera->stop();
        camera->start();
    }

    pending_camera_snapshots++;
    camera_capture->capture();
#endif
}

void RenderWindow::OnCameraCapture(int requestId, const QImage& img) {
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0)) && YUZU_USE_QT_MULTIMEDIA
    // TODO: Capture directly in the format and resolution needed
    const auto camera_width = input_subsystem->GetCamera()->getImageWidth();
    const auto camera_height = input_subsystem->GetCamera()->getImageHeight();
    const auto converted =
        img.scaled(static_cast<int>(camera_width), static_cast<int>(camera_height),
                   Qt::AspectRatioMode::IgnoreAspectRatio,
                   Qt::TransformationMode::SmoothTransformation)
            .mirrored(false, true);
    if (camera_data.size() != camera_width * camera_height) {
        camera_data.resize(camera_width * camera_height);
    }
    std::memcpy(camera_data.data(), converted.bits(), camera_width * camera_height * sizeof(u32));
    input_subsystem->GetCamera()->SetCameraData(camera_width, camera_height, camera_data);
    pending_camera_snapshots = 0;
#endif
}

bool RenderWindow::event(QEvent* event) {
    if (event->type() == QEvent::TouchBegin) {
        TouchBeginEvent(static_cast<QTouchEvent*>(event));
        return true;
    } else if (event->type() == QEvent::TouchUpdate) {
        TouchUpdateEvent(static_cast<QTouchEvent*>(event));
        return true;
    } else if (event->type() == QEvent::TouchEnd || event->type() == QEvent::TouchCancel) {
        TouchEndEvent();
        return true;
    }

    return QQuickWindow::event(event);
}

void RenderWindow::focusOutEvent(QFocusEvent* event) {
    QQuickWindow::focusOutEvent(event);
    input_subsystem->GetKeyboard()->ReleaseAllKeys();
    input_subsystem->GetMouse()->ReleaseAllButtons();
    input_subsystem->GetTouchScreen()->ReleaseAllTouch();
}

void RenderWindow::resizeEvent(QResizeEvent* event) {
    QQuickWindow::resizeEvent(event);
    OnFramebufferSizeChanged();
}

std::unique_ptr<Core::Frontend::GraphicsContext> RenderWindow::CreateSharedContext() const {
#ifdef HAS_OPENGL
    if (Settings::values.renderer_backend.GetValue() == Settings::RendererBackend::OpenGL) {
        auto c = static_cast<OpenGLSharedContext*>(main_context.get());
        // Bind the shared contexts to the main surface in case the backend wants to take over
        // presentation
        return std::make_unique<OpenGLSharedContext>(c->GetShareContext(),
                                                     child_widget->windowHandle());
    }
#endif
    return std::make_unique<DummyContext>();
}

bool RenderWindow::InitRenderTarget() {
    ReleaseRenderTarget();

    first_frame = false;

    switch (Settings::values.renderer_backend.GetValue()) {
    case Settings::RendererBackend::OpenGL:
        if (!InitializeOpenGL()) {
            return false;
        }
        break;
    case Settings::RendererBackend::Vulkan:
        if (!InitializeVulkan()) {
            return false;
        }
        break;
    case Settings::RendererBackend::Null:
        InitializeNull();
        break;
    }

    // Update the Window System information with the new render target
    window_info = QtCommon::GetWindowSystemInfo(child_widget->windowHandle());

    child_widget->resize(Layout::ScreenUndocked::Width, Layout::ScreenUndocked::Height);
    layout()->addWidget(child_widget);
    // Reset minimum required size to avoid resizing issues on the main window after restarting.
    setMinimumSize(1, 1);

    resize(Layout::ScreenUndocked::Width, Layout::ScreenUndocked::Height);

    OnMinimalClientAreaChangeRequest(GetActiveConfig().min_client_area_size);
    OnFramebufferSizeChanged();
    BackupGeometry();

    if (Settings::values.renderer_backend.GetValue() == Settings::RendererBackend::OpenGL) {
        if (!LoadOpenGL()) {
            return false;
        }
    }

    return true;
}

void RenderWindow::ReleaseRenderTarget() {
    if (child_widget) {
        layout()->removeWidget(child_widget);
        child_widget->deleteLater();
        child_widget = nullptr;
    }
    main_context.reset();
}

void RenderWindow::CaptureScreenshot(const QString& screenshot_path) {
    auto& renderer = system.Renderer();

    if (renderer.IsScreenshotPending()) {
        LOG_WARNING(Render,
                    "A screenshot is already requested or in progress, ignoring the request");
        return;
    }

    const Layout::FramebufferLayout layout{[]() {
        u32 height = UISettings::values.screenshot_height.GetValue();
        if (height == 0) {
            height = Settings::IsDockedMode() ? Layout::ScreenDocked::Height
                                              : Layout::ScreenUndocked::Height;
            height *= Settings::values.resolution_info.up_factor;
        }
        const u32 width =
            UISettings::CalculateWidth(height, Settings::values.aspect_ratio.GetValue());
        return Layout::DefaultFrameLayout(width, height);
    }()};

    screenshot_image = QImage(QSize(layout.width, layout.height), QImage::Format_RGB32);
    renderer.RequestScreenshot(
        screenshot_image.bits(),
        [=, this](bool invert_y) {
            const std::string std_screenshot_path = screenshot_path.toStdString();
            if (screenshot_image.mirrored(false, invert_y).save(screenshot_path)) {
                LOG_INFO(Frontend, "Screenshot saved to \"{}\"", std_screenshot_path);
            } else {
                LOG_ERROR(Frontend, "Failed to save screenshot to \"{}\"", std_screenshot_path);
            }
        },
        layout);
}

bool RenderWindow::IsLoadingComplete() const {
    return first_frame;
}

void RenderWindow::OnMinimalClientAreaChangeRequest(std::pair<u32, u32> minimal_size) {
    setMinimumSize(minimal_size.first, minimal_size.second);
}

bool RenderWindow::InitializeOpenGL() {
#ifdef HAS_OPENGL
    if (!QOpenGLContext::supportsThreadedOpenGL()) {
        QMessageBox::warning(this, tr("OpenGL not available!"),
                             tr("OpenGL shared contexts are not supported."));
        return false;
    }

    // TODO: One of these flags might be interesting: WA_OpaquePaintEvent, WA_NoBackground,
    // WA_DontShowOnScreen, WA_DeleteOnClose
    auto child = new OpenGLRenderWidget(this);
    child_widget = child;
    child_widget->windowHandle()->create();
    auto context = std::make_shared<OpenGLSharedContext>(child->windowHandle());
    main_context = context;
    child->SetContext(
        std::make_unique<OpenGLSharedContext>(context->GetShareContext(), child->windowHandle()));

    return true;
#else
    QMessageBox::warning(this, tr("OpenGL not available!"),
                         tr("Eden has not been compiled with OpenGL support."));
    return false;
#endif
}

bool RenderWindow::InitializeVulkan() {
    auto child = new VulkanRenderWidget(this);
    child_widget = child;
    child_widget->windowHandle()->create();
    main_context = std::make_unique<DummyContext>();

    return true;
}

void RenderWindow::InitializeNull() {
    child_widget = new NullRenderWidget(this);
    main_context = std::make_unique<DummyContext>();
}

bool RenderWindow::LoadOpenGL() {
    auto context = CreateSharedContext();
    auto scope = context->Acquire();
    if (!gladLoadGL()) {
        QMessageBox::warning(
            this, tr("Error while initializing OpenGL!"),
            tr("Your GPU may not support OpenGL, or you do not have the latest graphics driver."));
        return false;
    }

    const QString renderer =
        QString::fromUtf8(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));

    if (!GLAD_GL_VERSION_4_6) {
        LOG_ERROR(Frontend, "GPU does not support OpenGL 4.6: {}", renderer.toStdString());
        QMessageBox::warning(this, tr("Error while initializing OpenGL 4.6!"),
                             tr("Your GPU may not support OpenGL 4.6, or you do not have the "
                                "latest graphics driver.<br><br>GL Renderer:<br>%1")
                                 .arg(renderer));
        return false;
    }

    QStringList unsupported_gl_extensions = GetUnsupportedGLExtensions();
    if (!unsupported_gl_extensions.empty()) {
        QMessageBox::warning(
            this, tr("Error while initializing OpenGL!"),
            tr("Your GPU may not support one or more required OpenGL extensions. Please ensure you "
               "have the latest graphics driver.<br><br>GL Renderer:<br>%1<br><br>Unsupported "
               "extensions:<br>%2")
                .arg(renderer)
                .arg(unsupported_gl_extensions.join(QStringLiteral("<br>"))));
        return false;
    }
    return true;
}

QStringList RenderWindow::GetUnsupportedGLExtensions() const {
    QStringList unsupported_ext;

    // Extensions required to support some texture formats.
    if (!GLAD_GL_EXT_texture_compression_s3tc) {
        unsupported_ext.append(QStringLiteral("EXT_texture_compression_s3tc"));
    }
    if (!GLAD_GL_ARB_texture_compression_rgtc) {
        unsupported_ext.append(QStringLiteral("ARB_texture_compression_rgtc"));
    }

    if (!unsupported_ext.empty()) {
        const std::string gl_renderer{reinterpret_cast<const char*>(glGetString(GL_RENDERER))};
        LOG_ERROR(Frontend, "GPU does not support all required extensions: {}", gl_renderer);
    }
    for (const QString& ext : unsupported_ext) {
        LOG_ERROR(Frontend, "Unsupported GL extension: {}", ext.toStdString());
    }

    return unsupported_ext;
}

void RenderWindow::OnEmulationStarting(EmuThread* emu_thread_) {
    emu_thread = emu_thread_;
}

void RenderWindow::OnEmulationStopping() {
    emu_thread = nullptr;
}

void RenderWindow::showEvent(QShowEvent* event) {
    QQuickWindow::showEvent(event);

    // windowHandle() is not initialized until the Window is shown, so we connect it here.
    connect(windowHandle(), &QWindow::screenChanged, this, &RenderWindow::OnFramebufferSizeChanged,
            Qt::UniqueConnection);
}

bool RenderWindow::eventFilter(QObject* object, QEvent* event) {
    if (event->type() == QEvent::HoverMove) {
        if (Settings::values.mouse_panning || Settings::values.mouse_enabled) {
            auto* hover_event = static_cast<QMouseEvent*>(event);
            mouseMoveEvent(hover_event);
            return false;
        }
        emit MouseActivity();
    }
    return false;
}
