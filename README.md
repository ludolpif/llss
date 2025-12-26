# Purpose and status
Libre Live Streaming Software (LLSS) for Streamers making live brodcasts on platforms like [twitch.tv](https://twitch.tv).

## Design goals summary
- Be a game by itself as many streamers enjoy spending time to *not* stream but prepare new stream ideas
- Be intuitive by using Streamers known concepts: a simplified Photoshop-like composition system and UI
- Be a [Free Software](https://www.gnu.org/philosophy/free-sw.html) that lowers the bar for Streamers to be autonomous and creative
- Be live hackable like [Hack'n'Slash](https://en.wikipedia.org/wiki/Hack_%27n%27_Slash) game by the Streamer himself or a remote helping person
- Be heavily moddable like Minecraft game by enthousiast communities with diverse programming skills
- Be portable, unzip it works, software + mods + config on an USB stick for streaming on leased computers
- Trivally have a well-tested configured version and a draft config + new version on the same computer

I also leave public an unfiltered-yet dubious goals ideas: [GOALS-scratchpad.md](doc/dev/early/GOALS-scratchpad.md).

## How to use it
You barely can't yet.

- Project current state: a fairly advanced "Hello World" with dynamic mod loading !
- This software will never be production-ready while I am the (sole) developer. Jump in !
- I want to find, reserve then change to a better project name if I feel it can really reach users one day

## How to write it, mod it

Build instructions for:
- [GNU/Linux](doc/BUILDING-Linux.md) (which I use)
- [Windows 10/11](doc/BUILDING-Windows.md) (which it works)
- [Mac OS 15/26](doc/BUILDING-MacOS.md) (which I barely it make work for now)

Code and other help/contribution instructions here: [CONTRIBUTING.md](doc/CONTRIBUTING.md).

# Technical choices
- Early stage, we can make huge changes now but not later, jump in !
- Don't dive into costly complex and platform-specific [short-term hopefully performant](doc/PERFORMANCE.md) C++ code
  - Keep low gatekeeping on who can actually write code here
  - Tried Rust + Bevy, powerful, not easy, fast changing, slow compiling, no stable ABI for mods
  - Trying C + SDL3, fine and very *fun to write* and C is king for other languages bindings
  - Prove me Zig or other can help to have more contributors here (I can really just switch if it really happens)
- Few but strong base libraries 
  - [FLECS](https://www.flecs.dev/flecs/): ECS are mind blowing building block for live-hackable data and behaviors
  - [Dear ImGui](https://github.com/ocornut/imgui) + [C bindings](https://github.com/dearimgui/dear_bindings): *enjoyable and simple* GUI crafting/modding
  - [SDL3](https://wiki.libsdl.org/SDL3/FrontPage): cross-platform development library handling platform-specific sadness for us
  - [FFMPEG NUT](https://ffmpeg.org/nut.html): specific container format to pass audio+video+subtitles to a separate ffmpeg process for encoding
  - Help me choose the few next

