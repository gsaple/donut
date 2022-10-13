# donut
My simplified and extended attempt at [Andy Sloane](https://twitter.com/a1k0n)'s cute [donut](https://www.a1k0n.net/2011/07/20/donut-math.html).

https://user-images.githubusercontent.com/108666441/195562020-52e88f77-8d78-4afa-a008-066f302155cd.mp4

## build for linux
- dependencies: ncursesw, [xdotool](https://github.com/jordansissel/xdotool)
- steps:
    1. modify `zoom.h` to adjust zoom commands for your terminal (default is for out-of-box alacritty)
    2. run `make`
    3. move the built binary `donut` to one of your $PATH 

## usage
- dependencies: a terminal emulator that supports emoji and _**MANY**_ times of zoom out so that emojis can be morphed into pixels. It works at least in both out-of-box [alacritty](https://github.com/alacritty/alacritty) and unpatched [st](https://st.suckless.org).
- use:
    1. run `donut`
    2. zoom in or zoom out or resize the terminal to your liking
    
## other notes
- two opitons to zoom:
    1. press <kbd>j</kbd> to zoom out and <kbd>k</kbd> to zoom in: some delay (blank screen in the video) but no flickering
    2. directly zoom out or zoom in using your terminal commands: no delay but with discernible flickering
- non-emoji: patch `non-emoji.diff` in the `irrelevant` folder if emoji is not needed
- memory: when the terminal is zoomed out, the memory usage for alacritty increases while remains almost fixed for st; alacritty offers faster performance than st at certain 'tiny-font' levels.
