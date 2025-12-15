# Windows developer and modder environment
I made Solution and Project files with a default install of VisualStudio 2022 Community on a Windows 10.

## Prerequisites

- Install VisualStudio 2022 Community (Free)
  - Should work on others versions, but ABI between mods and app is at risk
  - Be sure to install the 'Desktop Development with C++' workload
  - You may let it install in it's default `C:\Program Files\Microsoft Visual Studio\2022\Community` folder
  - Make a first launch, try to run an Hello World then exit
- Create some folders for development, I use `C:\dev\libs` and `C:\dev\git`
  - Others paths should be fine but if fails try those ones then fill an issue
  - I don't test builds with paths containing spaces. It may help to avoid them at first place.
- Get the project code
  - Install Git for Windows from https://git-scm.com/install/windows
  - From `explorer.exe` pointed at `C:\dev\git`, right-click, "Open Git bash here"
  - Run those commands :
```
git clone https://github.com/ludolpif/llss.git
cd llss
git submodule update --init --recursive lib/ui/imgui
```
  - You should now have a `c:\dev\git\llss\lib\ui\imgui\imgui.h` file
- You need to manually install a released developement package of SDL3
  - For now, I didn't choose to automate SDL3 download or detection (no vcpkg nor CMake).
  - Available there https://github.com/libsdl-org/SDL/releases
  - Don't take `SDL3-3.2.26-win32-x86.zip` as it does not contains needed headers files
  - The right one is named `SDL3-devel-3.2.26-VC.zip` 
  - As SDL releases produce many assets, it could be hidden until you click a link titled like "Show all 14 assets"
  - Unzip this somewhere handy, let's say `C:\dev\libs\`
  - Archive contains a subfolder `SDL3-3.2.26`. Zip extraction graphical tools tends to create one also, don't mess up
  - It could be `C:\dev\libs\SDL3-3.2.26`. You should have `C:\dev\libs\SDL3-3.2.26\include` folder
- Set a user environnement variable named `SDL3_DIR` with `C:\dev\libs\SDL3-3.2.26`
  - Start Menu, serach for "Settings", in "About", click on "Advanced system settings"
  - A "System Properties" windows appears, in "Advanced" tab, click on "Environment Variables..." button
  - Click on the top-most "New..." button (section "User variables for %username%" and not in "System variables")
  - When applied, you don't need to restart your session (true only since Windows 10 I think)
  - You may need to restart VisualStudio if already opened

## Why two Solution files
I made 2 `.sln` files : 
- 1 for the app
- 1 for the mod template

Because app developpers and plugins developpers should be different group of people, compiling at different time on different machines.

I plan to provide a simple plugin development SDK without the need to compile the whole app. It will ship `app.lib` and `mod-template.sln` and their dependencies.

## Actually Building

### From VisualStudio GUI

Choose if you want a Debug or a Release build, keep it consistent for all projects in your first build attempts.
Instructions below are in context of `c:\dev\git\llss` folder.

From the less fragile to the more fragile :
- Open `app.sln`
- build `flecs`, then `ImGuiLib`, then `app`
- Copy `%SDL_DIR\lib\x64\SDL3.DLL` next to `app.exe` that have been outputed at the root of the git cloned folder
- Run `app.exe` (without mods), it should not fail
- Close `app.sln` or the whole VisualStudio
- Open `mod-template.sln` VistualStudio
- build only `mod-template` as other referenced projects have already built
- Re-run `app.exe` (with a mod), it should not fail

### From command-line, with MSBuild

In a `cmd.exe`:
```
cd c:\dev\git\llss
make.cmd
```
Or in a git bash:
```
cd /c/dev/git/llss/
./make.cmd
```

Note: if you have installed VS2022 in a non-default folder, you may have to tweak make.cmd first line to put a matching path for `msbuild.exe`.

### Reporting build errors

Open an issue if you do have building errors but double check those before:
- SDL3 env var: in a `cmd.exe`, try run `echo %SDL3_DIR%`, it should reply `C:\dev\libs\SDL3-3.2.26`
- mandatory git submodule: type Win+R and run `notepad c:\dev\git\llss\lib\ui\imgui\imgui.h`, it should display ``// dear imgui, v1.92.5``

- If the error occurs using the GUI, please try also MSBuild to attach it's full terminal output in the bug report
- You can compare with a successufl build trace here: [dev/early/build-trace-msbuild.txt](dev/early/build-trace-msbuild.txt)

## Debugging tools
TODO
