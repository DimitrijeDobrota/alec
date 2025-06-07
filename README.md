# alec

Abstraction Layer for Escape Codes

## Description

This project is a header-only library that allows for easy creation and usage
of ANSI Escape Codes from C++ project. It exports a set of template variables
to be used when the content is known at compile time, as well as a set of
functions to be used at run-time.


## Dependencies

* CMake 3.14 or latter
* Compiler with C++20 support (tested on clang version 16.0.5)


## Building and installing

See the [`BUILDING`](BUILDING.md) document.


## Usage

> Please reference demo folder for relevant usage examples.

Everything that this library provide is found in `alec` namespace.
Functions have names shown in the table below, whilst templates have suffix _v
and don't need to be called.

### Available functions with short explanation

| Name              | parameters          | Description                                       |
|-------------------|---------------------|---------------------------------------------------|
| cursor_up         | int cnt             | Move cursor `cnt` lines up                        |
| cursor_down       | int cnt             | Move cursor `cnt` lines down                      |
| cursor_frwd       | int cnt             | Move cursor `cnt` lines left                      |
| cursor_back       | int cnt             | Move cursor `cnt` lines right                     |
| cursor_line_next  | int cnt             | Move cursor to beginning of line `cnt` lines down |
| cursor_line_prev  | int cnt             | Move cursor to beginning of line `cnt` lines up   |
| cursor_column     | int col             | Set cursor to specific position on current line   |
| cursor_position   | int col, int row    | Set cursor to specific position on screen         |
| erase_display     | Motion mtn          | Erase in display mode                             |
| erase_line        | Motion mtn          | Erase in line mode                                |
| scroll_up         | int cnt             | Scroll whole page up by `cnt`                     |
| scroll_down       | int cnt             | Scroll whole page down by `cnt`                   |
| foreground        | Color color         | Set foreground to `color`                         |
| background        | Color color         | Set background to `color`                         |
| foreground        | int idx             | Set foreground to `idx` in 256 color pallet       |
| background        | int idx             | Set background to `idx` in 256 color pallet       |
| foreground        | int R, int G, int B | Set foreground to `RGB` value                     |
| background        | int R, int G, int B | Set background to `RGB` value                     |
| decor_set         | Decor decor         | Turn specific decoration on                       |
| decor_reset       | Decor decor         | Turn specific decoration of                       |
| cursor_save       |                     | Saves cursor position                             |
| cursor_restore    |                     | Restore cursor position                           |
| screen_mode_set   | int mode            | Changes screen width or type to `mode`            |
| screen_mode_reset | int mode            | Reset screen width or type to `mode`              |
| screen_save       |                     | Save screen                                       |
| screen_restore    |                     | Restore screen                                    |
| cursor_show       |                     | Show cursor                                       |
| cursor_hide       |                     | Hide cursor                                       |
| abuf_enable       |                     | Enable alternate screen buffer                    |
| abuf_disable      |                     | Disable alternate screen buffer                   |
| paste_enable      |                     | Turn on bracketed paste mode                      |
| paste_disable     |                     | Turn off bracketed paste mode                     |


### Enumeration

#### Motion

* END: erase from cursor until end of screen/line
* BEGIN: erase from cursor to beginning of screen/line
* WHOLE: erase entire screen/line


#### Color

* BLACK
* RED
* GREEN
* YELLOW
* BLUE
* MAGENTA
* CYAN
* WHITE
* DEFAULT


#### Decor

* RESET
* BOLD
* DIM
* ITALIC
* UNDERLINE
* BLINK
* INVERSE
* HIDE
* STRIKE


## Contributing

See the [`CONTRIBUTING`](CONTRIBUTING.md) document.


## Version History

* 0.2
    * Drop generation phase
    * Modernize codebase
    * Basic user input facilities
* 0.1
    * Initial development


## License

This project is licensed under the MIT License - see the [`LICENSE`](LICENSE.md) file for details

## References

* [`ANSI Escape Sequences gist`](https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797)
* [`Wikipedia article`](https://en.wikipedia.org/wiki/ANSI_escape_code)

