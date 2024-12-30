# Shovel

Fun project aimed to deliver TUI application, which works in both Windows and Linux environments.

Supports timer, handling keyboard input, has a diff based rendering in shell with buffering.

## Building
hit `./runme.sh` to build using cmake. The same solution works on Windows, it just requires to manually trigger cmake.

## Tests
All subprojects have test builds (suffixed with _test) which are triggered by cmake 

## Demos
Projects which are designed to be executed in consoles are demos. Files are suffixed with _demo.cxx and artifacts are suffixed with `Demo`, like TetrisDemo.