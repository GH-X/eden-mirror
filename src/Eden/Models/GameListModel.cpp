#include "GameListModel.h"

#include <QDirIterator>
#include <QGuiApplication>
#include <QThreadPool>

#include "GameIconProvider.h"
#include "GameListWorker.h"
#include "common/fs/fs_util.h"
#include "common/logging/filter.h"
#include "common/string_util.h"
#include "core/file_sys/patch_manager.h"
#include "core/hle/kernel/k_process.h"
#include "core/hle/kernel/kernel.h"
#include "core/hle/service/am/applet_manager.h"
#include "core/loader/loader.h"
#include "hid_core/hid_core.h"
#include "qt_common/qt_common.h"
#include "qt_common/util/content.h"
#include "video_core/gpu.h"

GameListModel::GameListModel(QObject *parent, QQmlEngine *engine, QMLConfig *config)
    : QStandardItemModel(parent)
    , m_config(config)
{
    QHash<int, QByteArray> rez = QStandardItemModel::roleNames();
    rez.insert(GLMRoleTypes::NAME, "name");
    rez.insert(GLMRoleTypes::PATH, "path");
    rez.insert(GLMRoleTypes::FILESIZE, "size");
    rez.insert(GLMRoleTypes::ICON, "icon");

    QStandardItemModel::setItemRoleNames(rez);

    m_provider = new GameIconProvider;
    engine->addImageProvider(QStringLiteral("games"), m_provider);

    watcher = new QFileSystemWatcher(this);
    connect(watcher, &QFileSystemWatcher::directoryChanged, this, &GameListModel::RefreshGameDirectory);

    populateAsync(UISettings::values.game_dirs);
}

QVariant GameListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == GLMRoleTypes::NAME) {
        return itemFromIndex(index)->text();
    }

    return QStandardItemModel::data(index, role);
}

void GameListModel::RefreshGameDirectory()
{
    if (!UISettings::values.game_dirs.empty() && current_worker != nullptr) {
        LOG_INFO(Frontend, "Change detected in the games directory. Reloading game list.");
        populateAsync(UISettings::values.game_dirs);
    }
}

void GameListModel::addEntry(QStandardItem *entry, const UISettings::GameDir &parent_dir) {
    // TODO: Directory grouping
    QString text = entry->data(GLMRoleTypes::NAME).toString();
    QPixmap pixmap = entry->data(GLMRoleTypes::ICON).value<QPixmap>();

    qDebug() << "Adding pixmap" << text;
    m_provider->addPixmap(text, pixmap);
    invisibleRootItem()->appendRow(entry);
}

// TODO
void GameListModel::addDirEntry(const UISettings::GameDir &dir) {}

// TODO
void GameListModel::donePopulating(QStringList watch_list) {
    // emit ShowList(!empt());

    // Clear out the old directories to watch for changes and add the new ones
    auto watch_dirs = watcher->directories();
    if (!watch_dirs.isEmpty()) {
        watcher->removePaths(watch_dirs);
    }

    // Workaround: Add the watch paths in chunks to allow the gui to refresh
    // This prevents the UI from stalling when a large number of watch paths are added
    // Also artificially caps the watcher to a certain number of directories
    constexpr int LIMIT_WATCH_DIRECTORIES = 5000;
    constexpr int SLICE_SIZE = 25;
    int len = (std::min)(static_cast<int>(watch_list.size()), LIMIT_WATCH_DIRECTORIES);
    for (int i = 0; i < len; i += SLICE_SIZE) {
        watcher->addPaths(watch_list.mid(i, i + SLICE_SIZE));
        QGuiApplication::processEvents();
    }
}

// TODO: Disable view
void GameListModel::populateAsync(QVector<UISettings::GameDir> &game_dirs) {
    // Cancel any existing worker.
    current_worker.reset();

    /// clear image provider
    m_provider->clear();

    // Delete any rows that might already exist if we're repopulating
    removeRows(0, rowCount());

    current_worker = std::make_unique<GameListWorker>(game_dirs);

    // Get events from the worker as data becomes available
    connect(current_worker.get(), &GameListWorker::DataAvailable, this, &GameListModel::WorkerEvent,
            Qt::QueuedConnection);

    QThreadPool::globalInstance()->start(current_worker.get());
}

// Worker-related slots
void GameListModel::WorkerEvent() {
    current_worker->ProcessEvents(this);
}

// Games interface
void GameListModel::BootGame(const QString& filename, Service::AM::FrontendAppletParameters params,
         StartGameType type) {
    LOG_INFO(Frontend, "Eden starting...");

    // TODO
    // if (params.program_id == 0 ||
    //     params.program_id > static_cast<u64>(Service::AM::AppletProgramId::MaxProgramId)) {
    //     StoreRecentFile(filename); // Put the filename on top of the list
    // }

    // Save configurations
    // UpdateUISettings();
    // game_list->SaveInterfaceLayout();
    m_config->save();

    u64 title_id{0};

    // last_filename_booted = filename;

    QtCommon::Content::ConfigureFilesystemProvider(filename.toStdString());
    const auto v_file = Core::GetGameFileFromPath(QtCommon::vfs, filename.toUtf8().constData());
    const auto loader = Loader::GetLoader(*QtCommon::system, v_file, params.program_id, params.program_index);

    if (loader != nullptr && loader->ReadProgramId(title_id) == Loader::ResultStatus::Success &&
        type == StartGameType::Normal) {
        // Load per game settings
        const auto file_path =
            std::filesystem::path{Common::U16StringFromBuffer(filename.utf16(), filename.size())};
        const auto config_file_name = title_id == 0
                                          ? Common::FS::PathToUTF8String(file_path.filename())
                                          : fmt::format("{:016X}", title_id);
        QtConfig per_game_config(config_file_name, Config::ConfigType::PerGameConfig);
        QtCommon::system->HIDCore().ReloadInputDevices();
        QtCommon::system->ApplySettings();
    }

    Settings::LogSettings();

    // if (UISettings::values.select_user_on_boot)// && !user_flag_cmd_line) {
    //     const Core::Frontend::ProfileSelectParameters parameters{
    //                                                              .mode = Service::AM::Frontend::UiMode::UserSelector,
    //                                                              .invalid_uid_list = {},
    //                                                              .display_options = {},
    //                                                              .purpose = Service::AM::Frontend::UserSelectionPurpose::General,
    //                                                              };
    //     if (SelectAndSetCurrentUser(parameters) == false) {
    //         return;
    //     }
    // }

    // If the user specifies -u (successfully) on the cmd line, don't prompt for a user on first
    // game startup only. If the user stops emulation and starts a new one, go back to the expected
    // behavior of asking.
    // user_flag_cmd_line = false;

    if (!LoadROM(filename, params)) {
        return;
    }

    QtCommon::system->SetShuttingDown(false);
    // game_list->setDisabled(true);

    // Create and start the emulation thread
    // emu_thread = std::make_unique<EmuThread>(*QtCommon::system);
    // emit EmulationStarting(emu_thread.get());
    // emu_thread->start();

    // Register an ExecuteProgram callback such that Core can execute a sub-program
    // QtCommon::system->RegisterExecuteProgramCallback(
    //     [this](std::size_t program_index_) { render_window->ExecuteProgram(program_index_); });

    // QtCommon::system->RegisterExitCallback([this] {
    //     emu_thread->ForceStop();
    //     render_window->Exit();
    // });

    // connect(render_window, &QuickRenderWindow::Closed, this, &GMainWindow::OnStopGame);
    // connect(render_window, &QuickRenderWindow::MouseActivity, this, &GMainWindow::OnMouseActivity);
    // // BlockingQueuedConnection is important here, it makes sure we've finished refreshing our views
    // // before the CPU continues
    // connect(emu_thread.get(), &EmuThread::DebugModeEntered, waitTreeWidget,
    //         &WaitTreeWidget::OnDebugModeEntered, Qt::BlockingQueuedConnection);
    // connect(emu_thread.get(), &EmuThread::DebugModeLeft, waitTreeWidget,
    //         &WaitTreeWidget::OnDebugModeLeft, Qt::BlockingQueuedConnection);

    // connect(emu_thread.get(), &EmuThread::LoadProgress, loading_screen,
    //         &LoadingScreen::OnLoadProgress, Qt::QueuedConnection);

    // // Update the GUI
    // UpdateStatusButtons();
    // if (ui->action_Single_Window_Mode->isChecked()) {
    //     game_list->hide();
    //     game_list_placeholder->hide();
    // }
    // status_bar_update_timer.start(500);
    // renderer_status_button->setDisabled(true);
    // refresh_button->setDisabled(true);

    // if (UISettings::values.hide_mouse || Settings::values.mouse_panning) {
    //     render_window->installEventFilter(render_window);
    //     render_window->setAttribute(Qt::WA_Hover, true);
    // }

    // if (UISettings::values.hide_mouse) {
    //     mouse_hide_timer.start();
    // }

    // render_window->InitializeCamera();

    std::string title_name;
    std::string title_version;
    const auto res = QtCommon::system->GetGameName(title_name);

    const auto metadata = [title_id] {
        const FileSys::PatchManager pm(title_id, QtCommon::system->GetFileSystemController(),
                                       QtCommon::system->GetContentProvider());
        return pm.GetControlMetadata();
    }();
    if (metadata.first != nullptr) {
        title_version = metadata.first->GetVersionString();
        title_name = metadata.first->GetApplicationName();
    }
    if (res != Loader::ResultStatus::Success || title_name.empty()) {
        title_name = Common::FS::PathToUTF8String(
            std::filesystem::path{Common::U16StringFromBuffer(filename.utf16(), filename.size())}
                .filename());
    }
    const bool is_64bit = QtCommon::system->Kernel().ApplicationProcess()->Is64Bit();
    const auto instruction_set_suffix = is_64bit ? tr("(64-bit)") : tr("(32-bit)");
    title_name = tr("%1 %2", "%1 is the title name. %2 indicates if the title is 64-bit or 32-bit")
                     .arg(QString::fromStdString(title_name), instruction_set_suffix)
                     .toStdString();
    LOG_INFO(Frontend, "Booting game: {:016X} | {} | {}", title_id, title_name, title_version);
    const auto gpu_vendor = QtCommon::system->GPU().Renderer().GetDeviceVendor();
    UpdateWindowTitle(title_name, title_version, gpu_vendor);

    // loading_screen->Prepare(QtCommon::system->GetAppLoader());
    // loading_screen->show();

    // emulation_running = true;
    // if (ui->action_Fullscreen->isChecked()) {
    //     ShowFullscreen();
    // }
    OnStartGame();
}

void GameListModel::BootGameFromList(const QString& filename, StartGameType with_config) {
    BootGame(filename, ApplicationAppletParameters(), with_config);
}

bool GameListModel::LoadROM(const QString& filename, Service::AM::FrontendAppletParameters params) {
    // Shutdown previous session if the emu thread is still active...
    // if (emu_thread != nullptr) {
    //     ShutdownGame();
    // }

    // if (!render_window->InitRenderTarget()) {
    //     return false;
    // }

    QtCommon::system->SetFilesystem(QtCommon::vfs);

    if (params.launch_type == Service::AM::LaunchType::FrontendInitiated) {
        QtCommon::system->GetUserChannel().clear();
    }

    // QtCommon::system->SetFrontendAppletSet({
    //     std::make_unique<QtAmiiboSettings>(*this), // Amiibo Settings
    //     (UISettings::values.controller_applet_disabled.GetValue() == true)
    //         ? nullptr
    //         : std::make_unique<QtControllerSelector>(*this), // Controller Selector
    //     std::make_unique<QtErrorDisplay>(*this),             // Error Display
    //     nullptr,                                             // Mii Editor
    //     nullptr,                                             // Parental Controls
    //     nullptr,                                             // Photo Viewer
    //     std::make_unique<QtProfileSelector>(*this),          // Profile Selector
    //     std::make_unique<QtSoftwareKeyboard>(*this),         // Software Keyboard
    //     std::make_unique<QtWebBrowser>(*this),               // Web Browser
    //     nullptr,                                             // Net Connect
    // });

    /** firmware check */
    if (!QtCommon::Content::CheckGameFirmware(params.program_id, this)) {
        return false;
    }

    /** Exec */
    const Core::SystemResultStatus result{
                                          QtCommon::system->Load(*render_window, filename.toStdString(), params)};

    const auto drd_callout = (UISettings::values.callout_flags.GetValue() &
                              static_cast<u32>(CalloutFlag::DRDDeprecation)) == 0;

    if (result == Core::SystemResultStatus::Success &&
        QtCommon::system->GetAppLoader().GetFileType() == Loader::FileType::DeconstructedRomDirectory &&
        drd_callout) {
        UISettings::values.callout_flags = UISettings::values.callout_flags.GetValue() |
                                           static_cast<u32>(CalloutFlag::DRDDeprecation);
        QMessageBox::warning(
            this, tr("Warning: Outdated Game Format"),
            tr("You are using the deconstructed ROM directory format for this game, which is an "
               "outdated format that has been superseded by others such as NCA, NAX, XCI, or "
               "NSP. Deconstructed ROM directories lack icons, metadata, and update "
               "support.<br><br>For an explanation of the various Switch formats Eden supports, <a "
               "href='https://eden-emulator.github.io/wiki/overview-of-switch-game-formats'>check "
               "out our "
               "wiki</a>. This message will not be shown again."));
    }

    if (result != Core::SystemResultStatus::Success) {
        switch (result) {
        case Core::SystemResultStatus::ErrorGetLoader:
            LOG_CRITICAL(Frontend, "Failed to obtain loader for {}!", filename.toStdString());
            QMessageBox::critical(this, tr("Error while loading ROM!"),
                                  tr("The ROM format is not supported."));
            break;
        case Core::SystemResultStatus::ErrorVideoCore:
            QMessageBox::critical(
                this, tr("An error occurred initializing the video core."),
                tr("Eden has encountered an error while running the video core. "
                   "This is usually caused by outdated GPU drivers, including integrated ones. "
                   "Please see the log for more details. "
                   "For more information on accessing the log, please see the following page: "
                   "<a href='https://yuzu-mirror.github.io/help/reference/log-files/'>"
                   "How to Upload the Log File</a>. "));
            break;
        default:
            if (result > Core::SystemResultStatus::ErrorLoader) {
                const u16 loader_id = static_cast<u16>(Core::SystemResultStatus::ErrorLoader);
                const u16 error_id = static_cast<u16>(result) - loader_id;
                const std::string error_code = fmt::format("({:04X}-{:04X})", loader_id, error_id);
                LOG_CRITICAL(Frontend, "Failed to load ROM! {}", error_code);

                const auto title =
                    tr("Error while loading ROM! %1", "%1 signifies a numeric error code.")
                        .arg(QString::fromStdString(error_code));
                const auto description =
                    tr("%1<br>Please redump your files or ask on Discord/Revolt for help.",
                       "%1 signifies an error string.")
                        .arg(QString::fromStdString(
                            GetResultStatusString(static_cast<Loader::ResultStatus>(error_id))));

                QMessageBox::critical(this, title, description);
            } else {
                QMessageBox::critical(
                    this, tr("Error while loading ROM!"),
                    tr("An unknown error occurred. Please see the log for more details."));
            }
            break;
        }
        return false;
    }
    current_game_path = filename;

    return true;
}

void GameListModel::OnStartGame() {
    QtCommon::OS::PreventOSSleep();

    // emu_thread->SetRunning(true);

    // UpdateMenuState();
    // OnTasStateChanged();

    // play_time_manager->SetProgramId(QtCommon::system->GetApplicationProcessProgramID());
    // play_time_manager->Start();

    // discord_rpc->Update();

#ifdef __linux__
    Common::Linux::StartGamemode();
#endif
}
