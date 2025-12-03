# Purpose and status
Libre Live Streaming Software (LLSS) is my attempt to mockup a Streaming software, to make live brodcasts on platforms like https://twitch.tv.

This software will never be production-ready while I am the (sole) developer and will change to a better name if I feel it can really reach users one day.

## Design goals summary
- Be a game by itself as many streamers enjoy spending time to *not* stream but prepare new stream ideas
- Be intuitive by using Streamers known concepts: a simplified Photoshop-like composition system and UI
- Be a [Free Software](https://www.gnu.org/philosophy/free-sw.html) that lowers the bar for Streamers to be autonomous and creative
- Be live hackable like [Hack'n'Slash](https://en.wikipedia.org/wiki/Hack_%27n%27_Slash) game by the Streamer himself
- Be heavily moddable like Minecraft game by enthousiast communities with diverse programming skills
- Be portable, unzip it works, .exe, config, everything on an USB stick for Streamers going to events without their computer
- Make simple to have a production choosen version + config and a testing new version + separate config on same computer

I also leave public an unfiltered-yet dubious goals ideas: [GOALS-scratchpad.md](doc/dev/early/GOALS-scratchpad.md).

# How to run and build it

No releases it at all. I want to do it early.

Build instructions for [GNU/Linux](BUILDING-Linux.md) (which I use), [Windows 10/11](BUILDING-Windows.md) (which it works), [Mac OS X](BUILDING-MacOSX.md) (not yet).

# Firsts technical choices (jump in !)
- Don't dive into costly complex and platform-specific [short-term hopefully performant](doc/PERFORMANCE.md) C++ code
  - Keep low gatekeeping on who can actually write code here
  - Tried Rust + Bevy, powerful, not easy, fast changing, slow compiling, no stable ABI for mods
  - Trying C + SDL3, fine and very *fun to write* and C is king for other languages bindings
  - prove me Zig or other can help to have more contributors here (I can really just switch if it really happens)
- Few but strong base libraries 
  - [FLECS](https://www.flecs.dev/flecs/): ECS are mind blowing building block for live-hackable data and behaviors
  - [Dear ImGui](https://github.com/ocornut/imgui) + [C bindings](https://github.com/dearimgui/dear_bindings): *enjoyable and simple* GUI crafting/modding
  - [SDL3](https://wiki.libsdl.org/SDL3/FrontPage): cross-platform development library handling platform-specific sadness for us
  - help me choose the few next

