# Windows developer and modder environment
I made Solution and Project files with a default install of VisualStudio 2022 on a Windows 10.

## Prerequisites
`git clone` this repository in a convenient folder with submodules. You need https://git-scm.com/install/windows.
You need at least to `git submodule update --init --recursive third-party/static/ui/imgui`.

For now, I didn't choose to automate SDL3 download or detection (no vcpkg nor CMake).

You need to manually install a released developement package of SDL3
- Available there https://github.com/libsdl-org/SDL/releases
- Don't take SDL3-3.2.26-win32-x86.zip as it does not contains needed headers files
- The right one is named like SDL3-devel-3.2.26-VC.zip 
- As SDL releases produce many assets, it could be hidden until you click a link titled like "Show all 14 assets"
- unzip this somewhere handy, let's say `C:\dev\libs\`
- archive contains a subfolder `SDL3-3.2.26`. Zip extraction graphical tools tends to create one also, don't mess up
- it could be `C:\dev\libs\SDL3-3.2.26`. You should have `C:\dev\libs\SDL3-3.2.26\include` folder
- set a user environnement variable named `SDL3_DIR` with `C:\dev\libs\SDL3-3.2.26`
  - in system "Environment variables" window, user variables are on the top of the UI, don't add in system variables
  - when applied, you don't need to restart your session (true only since Windows 10 I think)
  - you may need to restart VisualStudio if already opened

## Why two Solution files
I made 2 `.sln` files : 
- 1 for the app
- 1 for the mod template

Because app developpers and plugins developpers should be different group of people, compiling at different time on different machines.

I plan to provide a simple plugin development SDK without the need to compile the whole app. It will ship `llss.exe` and `llss-mod-template.sln` and their dependencies.

## Actually Building
Open an issue if you do have building errors and double-checked `echo %SDL3_DIR%` result in a `cmd.exe`.
Choose if you want a Debug or a Release build, keep it consistent for all projects in your first build attempts.
I don't test builds with paths containing spaces. It may help to avoid them at first place.

From the less fragile to the more fragile :
- Open `llss.sln`
- build `flecs`, then `ImGuiLib`, then `llss`
- Copy `%SDL_DIR\lib\x64\SDL3.DLL` next to `llss.exe` that have been outputed at the root of the git cloned folder
- Run `llss.exe` (without mods), it should not fail
- Close `llss.sln` or the whole VisualStudio
- Open `llss-mod-template.sln` VistualStudio
- build only `llss-mod-template` as other referenced projects have already built
- Re-run `llss.exe` (with a mod), it should not fail

## Debugging tools
TODO
