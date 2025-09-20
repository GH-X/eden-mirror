// SPDX-FileCopyrightText: Copyright 2025 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

// SPDX-FileCopyrightText: Copyright 2024 Torzu Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

// SPDX-FileCopyrightText: Copyright 2023 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "shared_translation.h"

#include <QCoreApplication>
#include <QWidget>
#include "common/settings.h"
#include "common/settings_enums.h"
#include "common/settings_setting.h"
#include "common/time_zone.h"
#include "qt_common/uisettings.h"
#include <map>
#include <memory>
#include <tuple>
#include <utility>

namespace ConfigurationShared {

std::unique_ptr<TranslationMap> InitializeTranslations(QWidget* parent)
{
    std::unique_ptr<TranslationMap> translations = std::make_unique<TranslationMap>();
    const auto& tr = [parent](const char* text) -> QString { return parent->tr(text); };

#define INSERT(SETTINGS, ID, NAME, TOOLTIP) \
    translations->insert(std::pair{SETTINGS::values.ID.Id(), std::pair{(NAME), (TOOLTIP)}})

    // A setting can be ignored by giving it a blank name

    // Applets
    INSERT(Settings, cabinet_applet_mode, tr("Amiibo 编辑器"), QString());
    INSERT(Settings, controller_applet_mode, tr("控制器设置"), QString());
    INSERT(Settings, data_erase_applet_mode, tr("Data erase"), QString());
    INSERT(Settings, error_applet_mode, tr("错误"), QString());
    INSERT(Settings, net_connect_applet_mode, tr("Net connect"), QString());
    INSERT(Settings, player_select_applet_mode, tr("选择玩家"), QString());
    INSERT(Settings, swkbd_applet_mode, tr("软键盘"), QString());
    INSERT(Settings, mii_edit_applet_mode, tr("Mii Edit"), QString());
    INSERT(Settings, web_applet_mode, tr("在线网页"), QString());
    INSERT(Settings, shop_applet_mode, tr("Shop"), QString());
    INSERT(Settings, photo_viewer_applet_mode, tr("照片查看器"), QString());
    INSERT(Settings, offline_web_applet_mode, tr("离线网页"), QString());
    INSERT(Settings, login_share_applet_mode, tr("Login share"), QString());
    INSERT(Settings, wifi_web_auth_applet_mode, tr("Wifi web auth"), QString());
    INSERT(Settings, my_page_applet_mode, tr("My page"), QString());

    // Audio
    INSERT(Settings, sink_id, tr("输出引擎："), QString());
    INSERT(Settings, audio_output_device_id, tr("输出设备："), QString());
    INSERT(Settings, audio_input_device_id, tr("输入设备："), QString());
    INSERT(Settings, audio_muted, tr("静音"), QString());
    INSERT(Settings, volume, tr("音量："), QString());
    INSERT(Settings, dump_audio_commands, QString(), QString());
    INSERT(UISettings, mute_when_in_background, tr("模拟器后台运行时静音"), QString());

    // Core
    INSERT(
        Settings,
        use_multi_core,
        tr("多核 CPU 仿真"),
        tr("此选项将 CPU 模拟线程的数量从 1 增加到 Switch 实机的最大值 4。\n"
           "这是调试选项，不应被禁用。"));
    INSERT(
        Settings,
        memory_layout_mode,
        tr("内存布局"),
        tr("提升模拟内存容量，从零售 Switch 通常的 4GB 内存增加到开发机的 6/8GB 内存。\n"
           "不会提高稳定性和性能，"
           "而是让大型纹理 MOD 适用于模拟内存。启用时将增加内存使用量。\n"
           "建议不要启用，"
           "除非具有纹理 MOD 的某些游戏需要。"));
    INSERT(Settings, use_speed_limit, QString(), QString());
    INSERT(Settings,
           speed_limit,
           tr("运行速度限制"),
           tr("控制游戏的最大渲染速度，但这取决于游戏的实际运行速度。\n"
              "对于 30 FPS 的游戏，设置为 200% 则将最高运行速度限制为 60 FPS；\n"
              "对于 60 FPS 的游戏，设置为 200% 则将最高运行速度限制为 120 FPS。\n"
              "禁用此项将解锁帧率限制，尽可能快地运行游戏。"));
    INSERT(Settings,
           sync_core_speed,
           tr("同步核心速度"),
           tr("将 CPU 核心速度与游戏的最大渲染速度同步，从而提升 FPS，"
              "且不影响游戏速度(动画、物理、等等)。\n"
              "兼容性因游戏而异；许多游戏(尤其是老游戏)的响应速度可能不佳。\n"
              "也许有助于减少低帧率下的卡顿。"));

    // Cpu
    INSERT(Settings,
           cpu_accuracy,
           tr("精度："),
           tr("此选项控制模拟 CPU 的精度。\n"
              "如果你不确定，就不要更改此项。"));
    INSERT(Settings, cpu_backend, tr("后端："), QString());

    INSERT(Settings, use_fast_cpu_time, QString(), QString());
    INSERT(Settings,
           fast_cpu_time,
           tr("快速 CPU 时间"),
           tr("对模拟 CPU 进行超频，以消除部分 FPS 限制。较弱的 CPU 可能会降低性能，某些游戏可能会运行异常。\n"
              "Boost (1700MHz) 以最高原生时钟速度运行，"
              "Fast (2000MHz) 以双倍默认时钟速度运行。"));

    INSERT(Settings, use_custom_cpu_ticks, QString(), QString());
    INSERT(Settings,
           cpu_ticks,
           tr("自定义 CPU 时钟"),
           tr("设置自定义的 CPU 时钟值。更高的值可能提高性能，"
              "但也可能导致游戏卡顿。建议范围为77-21000。"));
    INSERT(Settings, cpu_backend, tr("Backend:"), QString());

    // Cpu Debug

    // Cpu Unsafe
    INSERT(Settings, cpuopt_unsafe_host_mmu, tr("Enable Host MMU Emulation (fastmem)"),
           tr("This optimization speeds up memory accesses by the guest program.\nEnabling it causes guest memory reads/writes to be done directly into memory and make use of Host's MMU.\nDisabling this forces all memory accesses to use Software MMU Emulation."));
    INSERT(
        Settings,
        cpuopt_unsafe_unfuse_fma,
        tr("Unfuse FMA (improve performance on CPUs without FMA)"),
        tr("This option improves speed by reducing accuracy of fused-multiply-add instructions on "
           "CPUs without native FMA support."));
    INSERT(
        Settings,
        cpuopt_unsafe_reduce_fp_error,
        tr("Faster FRSQRTE and FRECPE"),
        tr("This option improves the speed of some approximate floating-point functions by using "
           "less accurate native approximations."));
    INSERT(Settings,
           cpuopt_unsafe_ignore_standard_fpcr,
           tr("Faster ASIMD instructions (32 bits only)"),
           tr("This option improves the speed of 32 bits ASIMD floating-point functions by running "
              "with incorrect rounding modes."));
    INSERT(Settings,
           cpuopt_unsafe_inaccurate_nan,
           tr("Inaccurate NaN handling"),
           tr("This option improves speed by removing NaN checking.\nPlease note this also reduces "
              "accuracy of certain floating-point instructions."));
    INSERT(Settings,
           cpuopt_unsafe_fastmem_check,
           tr("Disable address space checks"),
           tr("This option improves speed by eliminating a safety check before every memory "
              "read/write in guest.\nDisabling it may allow a game to read/write the emulator's "
              "memory."));
    INSERT(
        Settings,
        cpuopt_unsafe_ignore_global_monitor,
        tr("Ignore global monitor"),
        tr("This option improves speed by relying only on the semantics of cmpxchg to ensure "
           "safety of exclusive access instructions.\nPlease note this may result in deadlocks and "
           "other race conditions."));

    // Renderer
    INSERT(
        Settings,
        renderer_backend,
        tr("界面："),
        tr("选择图形界面。\n大多数情况下建议使用 Vulkan。"));
    INSERT(Settings,
           vulkan_device,
           tr("设备："),
           tr("图形界面为 Vulkan 时所使用的 GPU。"));
    INSERT(Settings,
           shader_backend,
           tr("着色器后端："),
           tr("选择 OpenGL 渲染器的着色器后端。\n"
              "GLSL 具有最好的性能和渲染精度。\n"
              "GLASM 仅限于 NVIDIA GPU，"
              "以 FPS 和渲染精度为代价提供更好的着色器构建性能。\n"
              "SPIR-V 编译速度最快，但在大多数 GPU 驱动程序上表现很差。"));
    INSERT(Settings,
           resolution_setup,
           tr("分辨率："),
           tr("指定游戏画面的分辨率。\n"
              "更高的分辨率需要更多的显存和带宽。\n"
              "低于 1X 的选项可能造成渲染问题。"));
    INSERT(Settings, scaling_filter, tr("视窗滤镜："), QString());
    INSERT(Settings,
           fsr_sharpening_slider,
           tr("FSR 锐化度："),
           tr("指定使用 FSR 时图像的锐化程度。"));
    INSERT(Settings,
           anti_aliasing,
           tr("抗锯齿方式："),
           tr("选择抗锯齿的方式。\nSMAA 抗锯齿提供最佳质量。\n"
              "FXAA 对性能影响较小，可以在非常低的分辨率下生成更好、"
              "更稳定的图像。"));
    INSERT(Settings,
           fullscreen_mode,
           tr("全屏模式："),
           tr("指定游戏的全屏模式。\n"
              "无边框窗口对屏幕键盘具有最好的兼容性，"
              "适用于某些需要屏幕键盘进行输入的游戏。\n"
              "独占全屏提供更好的性能和同步(Freesync/Gsync)支持。"));
    INSERT(Settings,
           aspect_ratio,
           tr("屏幕纵横比："),
           tr("拉伸游戏画面以适应指定的屏幕纵横比。\n"
              "Switch 游戏只支持 16:9，因此需要 MOD 才能实现其它比例。\n"
              "此选项也决定捕获屏幕截图的纵横比。"));
    INSERT(Settings,
           use_disk_shader_cache,
           tr("启用磁盘管线缓存"),
           tr("将生成的着色器保存到硬盘，"
              "提高后续游戏过程中的着色器加载速度。\n"
              "请仅在调试时禁用此项。"));
    INSERT(Settings,
           optimize_spirv_output,
           tr("优化 SPIRV 输出着色器"),
           tr("优化编译后的 SPIRV 着色器。\n"
              "这将增加着色器编译所需的时间。\n"
              "可能会略微提升性能。\n此功能尚处于实验阶段。"));
    INSERT(
        Settings,
        use_asynchronous_gpu_emulation,
        tr("使用 GPU 异步模拟"),
        tr("使用额外的 CPU 线程进行渲染。\n此选项应始终保持启用状态。"));
    INSERT(Settings,
           nvdec_emulation,
           tr("NVDEC 模拟方式："),
           tr("指定解码视频的方式。\n"
              "可以使用 CPU 或 GPU 进行解码，也可以完全不进行解码(遇到视频则黑屏处理)。\n"
              "大多数情况下，使用 GPU 解码将提供最好的性能。"));
    INSERT(Settings,
           accelerate_astc,
           tr("ASTC 纹理解码方式："),
           tr("此选项控制 ASTC 纹理解码方式。\n"
              "CPU：使用 CPU 进行解码，速度最慢但最安全。\n"
              "GPU：使用 GPU 的计算着色器来解码 ASTC 纹理，"
              "建议大多数游戏和用户使用此项。\n"
              "CPU 异步：使用 CPU 在 ASTC 纹理到达时对其进行解码。\n"
              "消除 ASTC 解码带来的卡顿，"
              "但在解码时可能出现渲染问题。"));
    INSERT(
        Settings,
        astc_recompression,
        tr("ASTC 纹理重压缩方式："),
        tr("几乎所有台式机和笔记本电脑 GPU 都不支持 ASTC 纹理，"
           "这迫使模拟器解压纹理到 GPU 支持的中间格式 RGBA8。\n"
           "此选项可将 RGBA8 重新压缩为 BC1 或 BC3 格式以节省显存，"
           "但会对图像质量产生负面影响。"));
    INSERT(Settings,
           vram_usage_mode,
           tr("显存使用模式："),
           tr("选择模拟器是优先节省显存还是最大限度地利用显存以提升性能。\n"
              "激进模式可能会严重影响其他应用程序"
              "(例如录屏软件)。"));
    INSERT(Settings,
           skip_cpu_inner_invalidation,
           tr("跳过 CPU 内在无效化"),
           tr("在内存更新期间跳过某些 CPU 端缓存无效操作，从而降低 CPU 占用率并提升性能。"
              "可能会导致某些游戏出现故障或崩溃。"));
    INSERT(
        Settings,
        vsync_mode,
        tr("垂直同步模式："),
        tr("FIFO (垂直同步开)不会掉帧或产生画面撕裂，但受到屏幕刷新率的限制。\n"
           "FIFO Relaxed 类似于 FIFO，但允许从低 FPS 恢复时产生撕裂。\n"
           "Mailbox 具有比 FIFO 更低的延迟，不会产生撕裂但可能会掉帧。\n"
           "Immediate (垂直同步关)只显示可用内容，"
           "并可能产生撕裂。"));
    INSERT(Settings, bg_red, QString(), QString());
    INSERT(Settings, bg_green, QString(), QString());
    INSERT(Settings, bg_blue, QString(), QString());

    // Renderer (Advanced Graphics)
    INSERT(Settings, sync_memory_operations, tr("同步内存操作"),
           tr("确保计算和内存操作之间的数据一致性。\n此选项应该可以修复某些游戏中的问题，但在某些情况下也可能会降低性能。\n虚幻引擎 4 开发的游戏通常影响较大。"));
    INSERT(Settings,
           async_presentation,
           tr("启用异步帧提交 (仅限 Vulkan)"),
           tr("将帧提交并移动到单独的 CPU 线程，略微提高性能。"));
    INSERT(
        Settings,
        renderer_force_max_clock,
        tr("强制最大时钟 (仅限 Vulkan)"),
        tr("在后台运行的同时等待图形命令，"
           "以防止 GPU 降低时钟速度。"));
    INSERT(Settings,
           max_anisotropy,
           tr("各向异性过滤："),
           tr("控制斜角的纹理渲染质量。\n"
              "这是一个渲染相关的选项，在大多数 GPU 上设置为 16x 是安全的。"));
    INSERT(Settings,
           gpu_accuracy,
           tr("GPU 精度："),
           tr("控制 GPU 模拟精度。大多数游戏 [正常] 精度模式渲染效果良好。\n"
              "部分游戏需要 [高] 精度模式。粒子通常只有在 [高] 精度模式下才能正确渲染。\n"
              "[极致] 精度模式仅作为最后的备选手段。"));
    INSERT(Settings,
           dma_accuracy,
           tr("DMA 精度："),
           tr("控制直接内存访问精度。[安全] 模式可以修复某些游戏中的问题，但在一些情况下也会影响性能。\n如果不确定，请保持默认值。"));
    INSERT(Settings,
           use_asynchronous_shaders,
           tr("启用异步着色器构建 (不稳定)"),
           tr("启用异步着色器编译，"
              "可能会减少着色器卡顿。\n"
              "实验性功能。"));
    INSERT(Settings, use_fast_gpu_time, QString(), QString());
    INSERT(Settings,
           fast_gpu_time,
           tr("快速 GPU 时间 (不稳定)"),
           tr("对模拟器 GPU 进行超频以增加动态分辨率和渲染距离。\n"
              "低 (128) 可获得最佳性能，高 (512) 可获得最佳图形保真度"));

    INSERT(Settings,
           use_vulkan_driver_pipeline_cache,
           tr("启用 Vulkan 管线缓存"),
           tr("启用 GPU 供应商专用的管线缓存。\n"
              "在 Vulkan 驱动程序内部不使用管线缓存的情况下，"
              "此选项可显著提高着色器加载速度。"));
    INSERT(
        Settings,
        enable_compute_pipelines,
        tr("启用计算管线 (仅限 Intel 显卡 Vulkan 模式)"),
        tr("启用某些游戏所需的计算管线。\n"
           "此选项仅适用于英特尔专有驱动程序。如果启用，可能会造成崩溃。\n"
           "在其他的驱动程序上将始终启用计算管线。"));
    INSERT(
        Settings,
        use_reactive_flushing,
        tr("启用反应性刷新"),
        tr("使用反应性刷新取代预测性刷新，"
           "从而更精确地同步内存。"));
    INSERT(Settings,
           use_video_framerate,
           tr("播放视频时同步帧率"),
           tr("在视频播放期间以正常速度运行游戏，"
              "即使帧率未锁定。"));
    INSERT(Settings,
           barrier_feedback_loops,
           tr("屏障反馈环路"),
           tr("改进某些游戏中透明效果的渲染。"));

    // Renderer (Extensions)
    INSERT(Settings,
           enable_raii,
           tr("RAII"),
           tr("Vulkan 中的一种自动资源回收方法，"
              "可确保不再需要的资源正确释放，"
              "但可能会导致游戏崩溃。"));
    INSERT(Settings,
           dyna_state,
           tr("动态状态扩展"),
           tr("控制动态状态扩展可使用的功能数量。\n数值越高，功能越多，性能越好，但可能会导致某些驱动程序和供应商出现问题。\n默认值可能因系统和硬件而异。\n更改此值可在视觉质量和稳定性中取得平衡。"));

    INSERT(Settings,
           provoking_vertex,
           tr("引发顶点"),
           tr("改善某些游戏中的光照和顶点处理。\n"
              "仅支持 Vulkan 1.0+。"));

    INSERT(Settings,
           descriptor_indexing,
           tr("描述符索引"),
           tr("改进纹理和缓冲区处理以及 Maxwell 转换层。\n"
              "部分 Vulkan 1.1+ 和所有 1.2+ 支持。"));

    INSERT(Settings, sample_shading, QString(), QString());

    INSERT(Settings,
           sample_shading_fraction,
           tr("采样着色"),
           tr("允许片段着色器在多重采样片段中逐个样本执行，而不是每个片段执行一次。"
              "此设置可提升图形质量，但会牺牲一些性能。\n"
              "值越高，质量提升幅度越大，但性能也会随之降低。"));

    // Renderer (Debug)

    // System
    INSERT(Settings,
           rng_seed,
           tr("随机数生成器种子"),
           tr("控制随机数生成器的种子。\n"
              "主要用于快速通关。"));
    INSERT(Settings, rng_seed_enabled, QString(), QString());
    INSERT(Settings, device_name, tr("设备名称"), tr("模拟 Switch 主机的名称。"));
    INSERT(Settings,
           custom_rtc,
           tr("自定义系统时间："),
           tr("此选项允许更改 Switch 的模拟时钟。\n"
              "可用于在游戏中操纵时间。"));
    INSERT(Settings, custom_rtc_enabled, QString(), QString());
    INSERT(Settings,
           custom_rtc_offset,
           QStringLiteral(" "),
           QStringLiteral("当前 Unix 时间的秒数"));
    INSERT(Settings,
           language_index,
           tr("语言："),
           tr("注意：当 [地区] 设置为 [自动选择] 时，此设置可能会被覆盖。"));
    INSERT(Settings, region_index, tr("地区："), tr("模拟 Switch 主机的所属地区。"));
    INSERT(Settings, time_zone_index, tr("时区："), tr("模拟 Switch 主机的所属时区。"));
    INSERT(Settings, sound_index, tr("声音输出模式："), QString());
    INSERT(Settings,
           use_docked_mode,
           tr("Console Mode:"),
           tr("Selects if the console is emulated in Docked or Handheld mode.\nGames will change "
              "their resolution, details and supported controllers and depending on this setting.\n"
              "Setting to Handheld can help improve performance for low end systems."));
    INSERT(Settings, current_user, QString(), QString());

    // Controls

    // Data Storage

    // Debugging

    // Debugging Graphics

    // Network

    // Web Service

    // Ui

    // Ui General
    INSERT(UISettings,
           select_user_on_boot,
           tr("游戏启动时提示选择用户"),
           tr("每次启动时要求选择一个用户配置文件，如果多人使用同一设备，"
              "则非常有用。"));
    INSERT(UISettings,
           pause_when_in_background,
           tr("位于后台时暂停模拟"),
           tr("当焦点在其它窗口时，此设置会暂停模拟器"));
    INSERT(UISettings,
           confirm_before_stopping,
           tr("停止模拟时需要确认"),
           tr("此设置将覆盖游戏中确认停止游戏的提示。\n"
              "启用此项将绕过游戏中的提示并直接退出模拟。"));
    INSERT(UISettings,
           hide_mouse,
           tr("自动隐藏鼠标光标"),
           tr("当鼠标停止活动超过 2.5 秒时将隐藏鼠标光标。"));
    INSERT(UISettings,
           controller_applet_disabled,
           tr("禁用控制器小程序"),
           tr("强制禁用来宾程序使用控制器小程序。\n"
              "当来宾程序尝试打开控制器小程序时，控制器小程序会立即关闭。"));
    INSERT(UISettings,
           check_for_updates,
           tr("检查更新"),
           tr("是否在启动时检查更新。"));

    // Linux
    INSERT(Settings, enable_gamemode, tr("Enable Gamemode"), QString());

    // Ui Debugging

    // Ui Multiplayer

    // Ui Games list

#undef INSERT

    return translations;
}

std::unique_ptr<ComboboxTranslationMap> ComboboxEnumeration(QWidget* parent)
{
    std::unique_ptr<ComboboxTranslationMap> translations = std::make_unique<ComboboxTranslationMap>();
    const auto& tr = [&](const char* text, const char* context = "") {
        return parent->tr(text, context);
    };

#define PAIR(ENUM, VALUE, TRANSLATION) {static_cast<u32>(Settings::ENUM::VALUE), (TRANSLATION)}

    // Intentionally skipping VSyncMode to let the UI fill that one out
    translations->insert({Settings::EnumMetadata<Settings::AppletMode>::Index(),
                          {
                              PAIR(AppletMode, HLE, tr("自定义前端")),
                              PAIR(AppletMode, LLE, tr("真实的小程序")),
                          }});

    translations->insert({Settings::EnumMetadata<Settings::SpirvOptimizeMode>::Index(),
                          {
                              PAIR(SpirvOptimizeMode, Never, tr("决不")),
                              PAIR(SpirvOptimizeMode, OnLoad, tr("加载")),
                              PAIR(SpirvOptimizeMode, Always, tr("总是")),
                          }});
    translations->insert({Settings::EnumMetadata<Settings::AstcDecodeMode>::Index(),
                          {
                              PAIR(AstcDecodeMode, Cpu, tr("CPU")),
                              PAIR(AstcDecodeMode, Gpu, tr("GPU")),
                              PAIR(AstcDecodeMode, CpuAsynchronous, tr("CPU 异步")),
                          }});
    translations->insert(
        {Settings::EnumMetadata<Settings::AstcRecompression>::Index(),
         {
             PAIR(AstcRecompression, Uncompressed, tr("不压缩 (最高质量)")),
             PAIR(AstcRecompression, Bc1, tr("BC1 (低等质量)")),
             PAIR(AstcRecompression, Bc3, tr("BC3 (中等质量)")),
         }});
    translations->insert({Settings::EnumMetadata<Settings::VramUsageMode>::Index(),
                          {
                              PAIR(VramUsageMode, Conservative, tr("保守模式")),
                              PAIR(VramUsageMode, Aggressive, tr("激进模式")),
                          }});
    translations->insert({Settings::EnumMetadata<Settings::RendererBackend>::Index(),
                          {
#ifdef HAS_OPENGL
                              PAIR(RendererBackend, OpenGL, tr("OpenGL")),
#endif
                              PAIR(RendererBackend, Vulkan, tr("Vulkan")),
                              PAIR(RendererBackend, Null, tr("空")),
                          }});
    translations->insert(
        {Settings::EnumMetadata<Settings::ShaderBackend>::Index(),
         {
             PAIR(ShaderBackend, Glsl, tr("GLSL")),
             PAIR(ShaderBackend, Glasm, tr("GLASM (汇编着色器，仅限 NVIDIA 显卡)")),
             PAIR(ShaderBackend, SpirV, tr("SPIR-V (实验性，仅限 AMD/Mesa)")),
         }});
    translations->insert({Settings::EnumMetadata<Settings::GpuAccuracy>::Index(),
                          {
                              PAIR(GpuAccuracy, Normal, tr("正常")),
                              PAIR(GpuAccuracy, High, tr("高")),
                              PAIR(GpuAccuracy, Extreme, tr("极致")),
                          }});
    translations->insert({Settings::EnumMetadata<Settings::DmaAccuracy>::Index(),
                          {
                              PAIR(DmaAccuracy, Default, tr("默认")),
                              PAIR(DmaAccuracy, Unsafe, tr("快速 (不安全)")),
                              PAIR(DmaAccuracy, Safe, tr("安全 (稳定)")),
                          }});
    translations->insert(
        {Settings::EnumMetadata<Settings::CpuAccuracy>::Index(),
         {
             PAIR(CpuAccuracy, Auto, tr("自动")),
             PAIR(CpuAccuracy, Accurate, tr("高精度")),
             PAIR(CpuAccuracy, Unsafe, tr("低精度")),
             PAIR(CpuAccuracy, Paranoid, tr("偏执模式 (禁用绝大多数优化项)")),
         }});
    translations->insert({Settings::EnumMetadata<Settings::CpuBackend>::Index(),
                          {
                              PAIR(CpuBackend, Dynarmic, tr("Dynarmic")),
                              PAIR(CpuBackend, Nce, tr("NCE")),
                          }});
    translations->insert({Settings::EnumMetadata<Settings::FullscreenMode>::Index(),
                          {
                              PAIR(FullscreenMode, Borderless, tr("无边框窗口")),
                              PAIR(FullscreenMode, Exclusive, tr("独占全屏")),
                          }});
    translations->insert({Settings::EnumMetadata<Settings::NvdecEmulation>::Index(),
                          {
                              PAIR(NvdecEmulation, Off, tr("无视频输出")),
                              PAIR(NvdecEmulation, Cpu, tr("CPU 视频解码")),
                              PAIR(NvdecEmulation, Gpu, tr("GPU 视频解码 (默认)")),
                          }});
    translations->insert(
        {Settings::EnumMetadata<Settings::ResolutionSetup>::Index(),
         {
             PAIR(ResolutionSetup, Res1_4X, tr("0.25X (180p/270p) [实验性]")),
             PAIR(ResolutionSetup, Res1_2X, tr("0.5X (360p/540p) [实验性]")),
             PAIR(ResolutionSetup, Res3_4X, tr("0.75X (540p/810p) [实验性]")),
             PAIR(ResolutionSetup, Res1X, tr("1X (720p/1080p)")),
             PAIR(ResolutionSetup, Res3_2X, tr("1.5X (1080p/1620p) [实验性]")),
             PAIR(ResolutionSetup, Res2X, tr("2X (1440p/2160p)")),
             PAIR(ResolutionSetup, Res3X, tr("3X (2160p/3240p)")),
             PAIR(ResolutionSetup, Res4X, tr("4X (2880p/4320p)")),
             PAIR(ResolutionSetup, Res5X, tr("5X (3600p/5400p)")),
             PAIR(ResolutionSetup, Res6X, tr("6X (4320p/6480p)")),
             PAIR(ResolutionSetup, Res7X, tr("7X (5040p/7560p)")),
             PAIR(ResolutionSetup, Res8X, tr("8X (5760p/8640p)")),
         }});
    translations->insert({Settings::EnumMetadata<Settings::ScalingFilter>::Index(),
                          {
                              PAIR(ScalingFilter, NearestNeighbor, tr("近邻取样")),
                              PAIR(ScalingFilter, Bilinear, tr("双线性过滤")),
                              PAIR(ScalingFilter, Bicubic, tr("双三线过滤")),
                              PAIR(ScalingFilter, Gaussian, tr("高斯模糊")),
                              PAIR(ScalingFilter, ScaleForce, tr("强制缩放")),
                              PAIR(ScalingFilter, Fsr, tr("AMD FidelityFX™️ 超级分辨率锐画技术")),
                              PAIR(ScalingFilter, Area, tr("Area")),
                          }});
    translations->insert({Settings::EnumMetadata<Settings::AntiAliasing>::Index(),
                          {
                              PAIR(AntiAliasing, None, tr("无")),
                              PAIR(AntiAliasing, Fxaa, tr("FXAA")),
                              PAIR(AntiAliasing, Smaa, tr("SMAA")),
                          }});
    translations->insert({Settings::EnumMetadata<Settings::AspectRatio>::Index(),
                          {
                              PAIR(AspectRatio, R16_9, tr("默认 (16:9)")),
                              PAIR(AspectRatio, R4_3, tr("强制 4:3")),
                              PAIR(AspectRatio, R21_9, tr("强制 21:9")),
                              PAIR(AspectRatio, R16_10, tr("强制 16:10")),
                              PAIR(AspectRatio, Stretch, tr("拉伸到窗口")),
                          }});
    translations->insert({Settings::EnumMetadata<Settings::AnisotropyMode>::Index(),
                          {
                              PAIR(AnisotropyMode, Automatic, tr("自动")),
                              PAIR(AnisotropyMode, Default, tr("默认")),
                              PAIR(AnisotropyMode, X2, tr("2x")),
                              PAIR(AnisotropyMode, X4, tr("4x")),
                              PAIR(AnisotropyMode, X8, tr("8x")),
                              PAIR(AnisotropyMode, X16, tr("16x")),
                          }});
    translations->insert(
        {Settings::EnumMetadata<Settings::Language>::Index(),
         {
             PAIR(Language, Japanese, tr("Japanese (日本語)")),
             PAIR(Language, EnglishAmerican, tr("American English")),
             PAIR(Language, French, tr("French (français)")),
             PAIR(Language, German, tr("German (Deutsch)")),
             PAIR(Language, Italian, tr("Italian (italiano)")),
             PAIR(Language, Spanish, tr("Spanish (español)")),
             PAIR(Language, Chinese, tr("中文")),
             PAIR(Language, Korean, tr("Korean (한국어)")),
             PAIR(Language, Dutch, tr("Dutch (Nederlands)")),
             PAIR(Language, Portuguese, tr("Portuguese (português)")),
             PAIR(Language, Russian, tr("Russian (Русский)")),
             PAIR(Language, Taiwanese, tr("台湾中文")),
             PAIR(Language, EnglishBritish, tr("British English")),
             PAIR(Language, FrenchCanadian, tr("Canadian French")),
             PAIR(Language, SpanishLatin, tr("Latin American Spanish")),
             PAIR(Language, ChineseSimplified, tr("简体中文")),
             PAIR(Language, ChineseTraditional, tr("繁体中文 (正體中文)")),
             PAIR(Language, PortugueseBrazilian, tr("Brazilian Portuguese (português do Brasil)")),
             PAIR(Language, Serbian, tr("Serbian (српски)")),
         }});
    translations->insert({Settings::EnumMetadata<Settings::Region>::Index(),
                          {
                              PAIR(Region, Japan, tr("Japan")),
                              PAIR(Region, Usa, tr("USA")),
                              PAIR(Region, Europe, tr("Europe")),
                              PAIR(Region, Australia, tr("Australia")),
                              PAIR(Region, China, tr("中国")),
                              PAIR(Region, Korea, tr("Korea")),
                              PAIR(Region, Taiwan, tr("台湾")),
                          }});
    translations->insert(
        {Settings::EnumMetadata<Settings::TimeZone>::Index(),
         {
          {static_cast<u32>(Settings::TimeZone::Auto),
           tr("自动 (%1)", "Auto select time zone")
               .arg(QString::fromStdString(
                   Settings::GetTimeZoneString(Settings::TimeZone::Auto)))},
          {static_cast<u32>(Settings::TimeZone::Default),
           tr("默认 (%1)", "Default time zone")
               .arg(QString::fromStdString(Common::TimeZone::GetDefaultTimeZone()))},
          PAIR(TimeZone, Cet, tr("CET")),
          PAIR(TimeZone, Cst6Cdt, tr("CST6CDT")),
          PAIR(TimeZone, Cuba, tr("Cuba")),
          PAIR(TimeZone, Eet, tr("EET")),
          PAIR(TimeZone, Egypt, tr("Egypt")),
          PAIR(TimeZone, Eire, tr("Eire")),
          PAIR(TimeZone, Est, tr("EST")),
          PAIR(TimeZone, Est5Edt, tr("EST5EDT")),
          PAIR(TimeZone, Gb, tr("GB")),
          PAIR(TimeZone, GbEire, tr("GB-Eire")),
          PAIR(TimeZone, Gmt, tr("GMT")),
          PAIR(TimeZone, GmtPlusZero, tr("GMT+0")),
          PAIR(TimeZone, GmtMinusZero, tr("GMT-0")),
          PAIR(TimeZone, GmtZero, tr("GMT0")),
          PAIR(TimeZone, Greenwich, tr("Greenwich")),
          PAIR(TimeZone, Hongkong, tr("香港")),
          PAIR(TimeZone, Hst, tr("HST")),
          PAIR(TimeZone, Iceland, tr("Iceland")),
          PAIR(TimeZone, Iran, tr("Iran")),
          PAIR(TimeZone, Israel, tr("Israel")),
          PAIR(TimeZone, Jamaica, tr("Jamaica")),
          PAIR(TimeZone, Japan, tr("Japan")),
          PAIR(TimeZone, Kwajalein, tr("Kwajalein")),
          PAIR(TimeZone, Libya, tr("Libya")),
          PAIR(TimeZone, Met, tr("MET")),
          PAIR(TimeZone, Mst, tr("MST")),
          PAIR(TimeZone, Mst7Mdt, tr("MST7MDT")),
          PAIR(TimeZone, Navajo, tr("Navajo")),
          PAIR(TimeZone, Nz, tr("NZ")),
          PAIR(TimeZone, NzChat, tr("NZ-CHAT")),
          PAIR(TimeZone, Poland, tr("Poland")),
          PAIR(TimeZone, Portugal, tr("Portugal")),
          PAIR(TimeZone, Prc, tr("中国标准时间")),
          PAIR(TimeZone, Pst8Pdt, tr("PST8PDT")),
          PAIR(TimeZone, Roc, tr("台湾时间")),
          PAIR(TimeZone, Rok, tr("ROK")),
          PAIR(TimeZone, Singapore, tr("新加坡")),
          PAIR(TimeZone, Turkey, tr("Turkey")),
          PAIR(TimeZone, Uct, tr("UCT")),
          PAIR(TimeZone, Universal, tr("Universal")),
          PAIR(TimeZone, Utc, tr("UTC")),
          PAIR(TimeZone, WSu, tr("W-SU")),
          PAIR(TimeZone, Wet, tr("WET")),
          PAIR(TimeZone, Zulu, tr("Zulu")),
          }});
    translations->insert({Settings::EnumMetadata<Settings::AudioMode>::Index(),
                          {
                              PAIR(AudioMode, Mono, tr("单声道")),
                              PAIR(AudioMode, Stereo, tr("立体声")),
                              PAIR(AudioMode, Surround, tr("环绕声")),
                          }});
    translations->insert({Settings::EnumMetadata<Settings::MemoryLayout>::Index(),
                          {
                              PAIR(MemoryLayout, Memory_4Gb, tr("4GB DRAM (默认)")),
                              PAIR(MemoryLayout, Memory_6Gb, tr("6GB DRAM (不安全)")),
                              PAIR(MemoryLayout, Memory_8Gb, tr("8GB DRAM")),
                              PAIR(MemoryLayout, Memory_10Gb, tr("10GB DRAM (不安全)")),
                              PAIR(MemoryLayout, Memory_12Gb, tr("12GB DRAM (不安全)")),
                          }});
    translations->insert({Settings::EnumMetadata<Settings::ConsoleMode>::Index(),
                          {
                              PAIR(ConsoleMode, Docked, tr("Docked")),
                              PAIR(ConsoleMode, Handheld, tr("Handheld")),
                          }});
    translations->insert({Settings::EnumMetadata<Settings::CpuClock>::Index(),
                          {
                              PAIR(CpuClock, Boost, tr("Boost (1700MHz)")),
                              PAIR(CpuClock, Fast, tr("Fast (2000MHz)")),
                          }});
    translations->insert(
        {Settings::EnumMetadata<Settings::ConfirmStop>::Index(),
         {
             PAIR(ConfirmStop, Ask_Always, tr("总是询问 (默认)")),
             PAIR(ConfirmStop, Ask_Based_On_Game, tr("仅当游戏不希望停止时")),
             PAIR(ConfirmStop, Ask_Never, tr("从不询问")),
         }});
    translations->insert({Settings::EnumMetadata<Settings::GpuOverclock>::Index(),
                          {
                              PAIR(GpuOverclock, Low, tr("低 (128)")),
                              PAIR(GpuOverclock, Medium, tr("中 (256)")),
                              PAIR(GpuOverclock, High, tr("高 (512)")),
                          }});

#undef PAIR
#undef CTX_PAIR

    return translations;
}
} // namespace ConfigurationShared
