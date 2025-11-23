#!/bin/bash -x

# vendor/{static,shared,module} are names inspired by https://cmake.org/cmake/help/latest/command/add_library.html

mkdir -p vendor/static/ui
# Keep public URLs in .gitmodules for project users
git config unset submodule.dear_bindings.url
git config unset submodule.dear_imgui.url
git submodule add -b for-llss-main --name dear_bindings https://github.com/ludolpif/dear_bindings.git vendor/static/ui/dear_bindings
git submodule add -b for-llss-main --name dear_imgui https://github.com/ludolpif/dear_imgui.git vendor/static/ui/imgui
# Override URL for commiters
git config submodule.dear_bindings.url git@github.com:ludolpif/dear_bindings.git
git config submodule.dear_imgui.url git@github.com:ludolpif/dear_imgui.git

mkdir -p vendor/shared/graphics/SDL3
touch vendor/shared/graphics/SDL3/.gitkeep

# TODO download-script per OS
# Windows needs https://github.com/libsdl-org/SDL/releases/download/release-3.2.26/SDL3-devel-3.2.26-VC.zip
# MacOS needs https://github.com/libsdl-org/SDL/releases/download/release-3.2.26/SDL3-3.2.26.dmg
# Linux: per distro thing. Try to use system-wide. Debian: apt install libsdl3-dev libsdl3-0-dbgsym

mkdir -p vendor/module
# For future things we dynamically load / dlopen()
