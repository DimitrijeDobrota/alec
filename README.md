# alec

Abstraction Layer for Escape Codes

## Description

This project is a header-only library that allows for easy creation and usage
of ANSI Escape Codes from C++ project. It exports a set of template variables
to be used when the content is known at compile time, as well as a set of
functions to be used at run-time.

Since there are two set of functionalities I noticed a twofold code duplication
so I decided to add a preprocessing step that turns a bison-like config file
into the desired header, based on the set of simple rules. This step was not
100% necessary, and might be an overkill, but I wanted to experiment with this
concept for a while and now I have got the change.

Generator code shown here is written in flex and bison and can be easily
adapted for other use cases.


## Dependencies

* CMake 3.14 or latter
* Compiler with C++20 support (tested on clang version 16.0.5)
* Flex 2.6.4
* Bison 3.8.2


## Building and installing

See the [`BUILDING`](BUILDING.md) document.


## Usage

> Please reference demo folder for relevant usage examples.

Everything that this library provide is found in `alec` namespace.
Functions have names shown in the table below, whilst templates have suffix _v
and don't need to be called.

#### Available functions with short explanation

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


#### Enumeration

##### Ctrl

* BELL: Terminal bell
* BS: Backspace
* HT: Horizontal TAB
* LF: Linefeed (newline)
* VT: Vertical TAB
* FF: Formfeed
* CR: Carriage return
* ESC: Escape character
* DEL: Delete character


##### Motion

* END: erase from cursor until end of screen/line
* BEGIN: erase from cursor to beginning of screen/line
* WHOLE: erase entire screen/line


##### Color

* BLACK
* RED
* GREEN
* YELLOW
* BLUE
* MAGENTA
* CYAN
* WHITE
* DEFAULT


##### Decor

* RESET
* BOLD
* DIM
* ITALIC
* UNDERLINE
* BLINK
* INVERSE
* HIDE
* STRIKE


## Configuration

Configuration file `alec.rules.hpp` is used to customize the output of
`alec.hpp` file. Similarly to Flex and Bison, configuration files needs to have
3 sections separated by `/*%%*/`: prologue, grammar and epilogue. Prologue and
epilogue are copied as-is to the output file whilst the grammar section
contains rules for generating template and function code.

* Rules can be separated by an arbitrary number of blank lines.
* Everything can have arbitrary indentation that is not carried to the resulting file.
* There could be C-style comments that are copied to the resulting file.

Each rule consists of 4 lines:
1. Name: name of the generated function, must be valid C++ name
2. Parameters: list of `type name` pairs separated by a comma
3. Constraints: list of constraint functions separated by a comma
4. Rules: list of chars and ints (or expressions producing them) separated by a
comma, or a single string literal

> You *must* use `|` character in place of empty list rule

### Constraints

* All constraints used in code generation must be defined in the prologue
* Every constraint listed will be applied to all of the arguments one by one.
* Every constraint has to have a function and template concept variant with the
same name, but suffix _v for the template one.
* Function constraints are translated into asserts, whilst template ones are used
in requires clause of the teconceptmplate.

### Examples

```c++
// begining of a header file

// tamplate constraint
template <int n>
concept limit_pos_v = n >= 0;

// function constraint
static inline bool limit_pos(int n) { return n >= 0; };

/*%%*//*

    // comment that goes into output
    decor_reset
    Decor decor
    |
    (int)decor + 20, 'm'

    screen_mode_set
    int mode
    limit_pos
    '=', mode, 'h'

    paste_enable
    |
    |
    "?2004h"

*//*%%*/

// ending of a header file
```


## Version History

* 0.1
    * Initial Release


## Contributing

See the [`CONTRIBUTING`](CONTRIBUTING.md) document.


## License

This project is licensed under the MIT License - see the [`LICENSE`](LICENSE.md) file for details

## References

* [`ANSI Escape Sequences gist`](https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797)
* [`Wikipedia article`](https://en.wikipedia.org/wiki/ANSI_escape_code)

