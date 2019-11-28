![Fox Fires](icon.png)

Fox Fires
=========

Simple program, that draws a famous polar lights.

![Fox Fires](example.png)

Usage
-----

* <kbd>Escape</kbd> - Close window (exit)
* <kbd>Space</kbd> - Pause
* <kbd>T</kbd> - Pause time
* <kbd>F3</kbd> - Debug screen (requires *font.ttf*)
* <kbd>F5</kbd> - Re-init layers
* <kbd>F5</kbd> + <kbd>Shift</kbd> - Re-init program
* <kbd>H</kbd> - Hide cursor
* <kbd>S</kbd> - Skip day
* <kbd>R</kbd> - Toggle realistic mode
* <kbd>F</kbd> (<kbd>F11</kbd>) - Fullscreen
* <kbd>←</kbd>, <kbd>→</kbd> - Change time (10 times faster when holding <kbd>Shift</kbd>)

Realistic mode
--------------

Difference between realistic and normal modes is only in colors. In this mode color update and processing completely disabled. This can give small FPS boost. Also this mode shows more natural northern lights because of colors, that were selected to match oxygen and nitrogen light emission.

![Fox Fires - Realistic mode](example-realistic.png)

Requirements
------------

### For Windows

#### Run

Everything, what is needed to run program, is packed in it except font (font.ttf).

#### Build

* SFML SDK
* C++ compiler (prefered: *MinGW*)

### For Linux

#### Run

* Package *libsfml* (*libsfml-dev* or similar)

#### Build

* Package *libsfml-dev* (or similar)
* C++ compiler (prefered: *g++*)
