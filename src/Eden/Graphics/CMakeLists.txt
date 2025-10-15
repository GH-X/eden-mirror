# SPDX-FileCopyrightText: Copyright 2025 Eden Emulator Project
# SPDX-License-Identifier: GPL-3.0-or-later

# SPDX-FileCopyrightText: Copyright 2025 crueter
# SPDX-License-Identifier: GPL-3.0-or-later

EdenModule(
    NAME Graphics
    URI Eden.Graphics
    SOURCES
        SettingsInterface.h SettingsInterface.cpp
        QMLSetting.h QMLSetting.cpp
        MetaObjectHelper.h
        QMLConfig.h
        TitleManager.h TitleManager.cpp
        RenderItems.h
    LIBRARIES
        Qt6::Quick
        Qt6::Core
        glad::glad
)

target_sources(EdenGraphics
  PRIVATE
    RenderWindow.h RenderWindow.cpp
)
