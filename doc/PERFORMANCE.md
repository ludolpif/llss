# Performance
More is good but hardly push it will make this a dead project. Choose only smart and usage-aligned spots.

- Is important but not as OBS crucially needed it in early years to be a barely usable thing
- We have now GPU video encoders (NVENC...) and many CPU idle cores while playing a AAA game
- Frame time budget is a thing to understand with multiple scales in mind from few µs to 16ms
  - 60 FPS rendering have 1s/60=16ms absolute maximal time frame budget
  - "2025 mandatory" heavy plugins like browser-source, IA plugins have >1 ms budget each
  - Platform-dependant hard to maintain core optimisation code is mostly in microseconds range
  - Just make a better product while explicitly not trying to run after those microseconds
- Make design choices allowing huge benefits in >100µs range that other software will not have

Example of a smart move starting from user behavior, examining other software behavior, imply our design:
- Game stream typical usage
  - "Just Chatting" with full-screen webcam
  - Then "Gaming" scene (screen + tiny cam) during the whole game execution (watts and time critical zone)
  - Stopping the game and "Just Chatting" with full-screen webcam
- Other software will stay at same full-res webcam capture resolution all the stream long
- In-game scene webcam source displayed below 640x360 = 0,2 Mpixels. Full-HD is 2Mpixels
- 4K resolution is 8 Mpixels. Divide from 10 to 20 the processing is saved Watts and >100µs
- It does not involve complex code, it costs some webcam source freezing during transition
- Make a few choosable standard video effect that make this freeze an intentional aesthetic thing
- Making it an optionnal good optimization for >4000$ gaming computer owner costs us barely a checkbox
