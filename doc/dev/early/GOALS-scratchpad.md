# Project Goals

## Design takes

Streaming software used to be a tool for gamers. Let's change this to : a game for streamer. A sandbox and hack'n'slash kind of game.

- distribution, installation
  - Streamers needs to use different computers when on the road, LAN, events
  - Streamers needs to secure a copy of their working software setup, program and config while testing new things
  - Streamers needs to feel joy instead of fear when trying to upgrade their config to new version
  - Design replies to this :
    - make it portable with unzip method of installation
    - config and software in same visible user-choosen user folder name with app version
    - wizard at first run to import config : it will duplicate it, not upgrade it in place

- ecosystem through integrations and plugins
  - Streamers have their own take about what aspect of content or interaction is important for their community
  - Most streamers aim to provide their own ideas of content making, differentiate from others through creativity
  - Design replies to this :
    - make native plugin code not to hard to write, compile and distribute
    - make app state and data available to plugins, even data from other plugins
    - use a ECS system instead of fixed API for each data type to have queries and custom hooks (systems)
    - use a skinnable and easy to program UI system that plugin writer can easily use for provide plugin specific UI and in-stream UI
  
- Live streaming is not TV video production
  - Video composition for live streaming is about mixing video streams but not only, rich components are useful for viewership entertainment
  - Scene composition live alteration on viewer-triggered event is desirable
  - Scene composition is mostly prepared before streams but is frequently edited live, even on active scene
  - Gaming streams tends to have full-screen hi-res Camera before and after gaming, but a really tiny Camera source during gameplay
  - Design replies to this :
    - Don't define a source-set per scene because this implies difficulties to make transitions that aren't just a shader mixing Scene A and Scene B output video for a short period of time.
    - Scenes could be more like animation keyframes : a set of properties to reach in a given timespan
    - Group scenes per source-set : transitions between scenes could move, reorder, enable/disable filters on every source between scenes.
    - On different source-set, lets say "Just Chatting" and "Gaming", the same Camera could be processed at max-resolution and at fairly low resolution respectively, because in the Gaming source-set, it will always be rendered small, saving a very huge amount of PCI-Express traffic (1080p60fps raw video could be 3GBit/s). This is amount of ressource gain for the game.
    - both UI and API could live edit scenes


- fit current games and hardware strength and weaknesses
  - Games have anti-cheat code fighting with game video capture
  - App should perform well on a laptop used as portative dual PC stream solution with USB HDMI capture card
  - Streamers will always want heavy plugins like browser-source, AI sound or video processing
  - Design replies to this
    - balance core app performance goals and app maintainance pain
    - ideal performance world is very platform dependant and can't let room for newcomer new ideas
    - allow native-code plugins for advanced plugins even if load foreign code is not secure
    - ship a real shader compiler in-app to let plugin writer publish galleries of visual effects and streamers modify them
  - Technical notes
    - VRAM to RAM downstreams were very slow in 2010
    - So camera and screen capturing, compositing, sending to h264 encoder without download was a critical game-changer
    - Not this bad anymore, and this is mostly eaten by heavy weight plugins used in the pipeline anyway
    - Games generally aren't inherently as parrallel as typical CPU cores numbers we have nowadays.


- hack'n'slash but I don't have a strong background in programming
  - ...
  - Design replies to this
    - have ECS reflection, clear namings for non-developers, and publish it to (web)socket for apps nearby or remote control
    - (remote data tree sandbox, query / system scaffolding examples)



command palette (omni search)


## Feature ideas

- ffmpeg -f nut -i -
- vdo.ninja integration (or in-app multi-streamer helpers and stream forwarding)

