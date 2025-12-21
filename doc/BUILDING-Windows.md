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
  - Run this: `git clone https://github.com/ludolpif/llss.git`
- Get the pre-built dependancies
  - The whole source of thoses dependencies is in the llss-build-dep project
  - We aim to help newcomers and build-time, it comes as separate .zip
  - We provide a powershell script named `build-dep.ps1` to get it and unzip at right place
  - Run this from a powershell prompt:
```
cd C:\dev\git\llss
.\build-dep.ps1 Debug
```
  - You should now have a `c:\dev\git\llss\lib\ui\imgui\imgui.h` and `c:\dev\git\llss\lib\platform\sdl3-devel-vc\lib\x64\SDL3.DLL`

## Actually Building

### From VisualStudio GUI

Instructions below are in context of `c:\dev\git\llss` folder.

From the less fragile to the more fragile :
- Open `app.sln`
- Check that the active configuration is `Debug`, not `Release`
- build `app` by right-clicking on it on the panel at the right and select `Build`
- From `explorer.exe`, copy the previously mentionned `SDL3.DLL` next to `llss.exe` in `c:\dev\git\llss\program\x64\Debug`
- Run `llss.exe` (without mods), it should not fail. You can start it from a console to see the realtime logging
- From VisualStudio, build mods projects. This will produce `c:\dev\git\llss\mods\mod-*\program\x64\Debug\mod-*.dll`.
- Re-run `llss.exe` (with some mods), it should not fail

### From command-line, with MSBuild

Alternative option to get a detailled build log.

- Run this from a powershell prompt:
```
cd C:\dev\git\llss
.\build.ps1 Debug
```

Note: if you have installed VS2022 in a non-default folder, you may have to tweak `build.ps1` first line to put a matching path for `msbuild.exe`.

### Reporting build errors

Open an issue if you do have building errors or newcomer questions.
- If the error occurs using the GUI, please try also MSBuild to attach it's full terminal output in the bug report
- You can compare with a successufl build trace here: [dev/early/build-trace-msbuild.txt](dev/early/build-trace-msbuild.txt)

You can also reach me on discord or sometimes Twitch, see [ludolpif.fr](https://ludolpif.fr) (in French).

## Continuous Integration and Windows

- On Github CI, public Windows runner are only Windows-server ones.
- We have fully scripted this in `.github/workflow/msbuild.xml`
- it boils down to
```
.\build-dep.ps1 Debug
.\build.ps1 Debug
```

## Debugging tools
TODO
