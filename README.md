# Console Roulette Simulator
A C++ console application that simulates a spinning roulette wheel with customizable options, rendered using ASCII art. Built with a lightweight 3D graphics engine ([Q3Engine](https://github.com/q12we34rt5/Q3Engine-cpp)) and a pixel matrix library, this program displays a rotating wheel with numbered segments and randomly selects a winner.

## Features
- **Customizable Roulette:** Define the number of segments, size, spin duration, and animation smoothness.
- **Color Support:** Specify text and highlight colors using hex codes.
- **Antialiasing:** Choose from multiple antialiasing modes (none, 2x, 4x, 8x, 16x) for smoother visuals.
- **Performance Control:** Set maximum FPS and TPS (ticks per second) limits, with optional high-precision timing.
- **Metrics Display:** Optionally show real-time FPS/TPS stats in the console.
- **Threaded Rendering:** Uses double buffering and separate threads for logic and rendering to ensure smooth animation.
- **Random Winner Selection:** Spins the wheel for a random number of rounds and stops at a randomly chosen segment.

## Dependencies
- C++17 or later
- POSIX-compatible terminal (for ANSI color support)
- GNU Make

## Build
1. Clone the repository:
```bash
git clone git@github.com:q12we34rt5/ConsoleRouletteSimulator.git
```
2. Navigate to the project directory:
```bash
cd ConsoleRouletteSimulator
```
3. Use the provided makefile to build the project:
```bash
make
```
> **Note:** The build may take a little longer on first run, as all number textures (0–9) are compiled directly into the executable.
4. Run the executable:
```bash
./roulette <n_numbers> [options]
```

## Usage
The program accepts a positional argument (n_numbers) and several optional arguments to customize the simulation.

### Command-Line Arguments
- **Positional Arguments:**
    - `n_numbers`: Number of segments on the roulette (e.g., 9 for numbers 1–9).
- **Optional Arguments:**
    - `-sz, --size <size>`: Size of the roulette display in pixels (default: `50`).
    - `-r, --rounds <rounds>`: Number of full spins before stopping (default: `10`).
    - `-st, --steps <steps>`: Number of animation steps for smoothness (default: `200`).
    - `--text-color <hex>`: Hex color code for text (default: `000000` - black).
    - `--highlight-color <hex>`: Hex color code for the highlighted number (default: `FF0000` - red).
    - `--aa <mode>`: Antialiasing mode (`none`, `2x`, `4x`, `8x`, `16x`; default: `4x`).
    - `--max-fps <fps>`: Maximum frames per second (0 = uncapped; default: `60`).
    - `--max-tps <tps>`: Maximum ticks per second (0 = uncapped; default: `100`).
    - `--show-metrics`: Display FPS/TPS stats in the console (default: off).
    - `--precise-timing`: Enable high-precision timing with busy-wait (default: off).
    - `-h, --help`: Show help message and exit.

### Example
Spin a roulette with 8 segments, a size of 150 pixels, 20 rounds, and 400 steps, using 8x antialiasing:
```bash
./roulette 8 -sz 150 -r 20 -st 400 --aa 8x
```

## How It Works
1. **Initialization:** Parses command-line arguments and configures the roulette with the specified settings.
2. **Rendering:** Creates a `Roulette` object with fan-shaped segments and text labels (numbers 1–9 looped from `assets/`), rendered to a framebuffer.
3. **Animation:** A `RotationManager` controls the spin, slowing down over a set number of steps until stopping at a random angle.
4. **Display:** Outputs the framebuffer to the console as ASCII art using double buffering.
5. **Multithreading:** Separate threads handle rendering and logic updates, capped by FPS and TPS limits.

## Limitations
- Console rendering quality depends on terminal support for ANSI escape codes.
- High antialiasing modes (e.g., 16x) may reduce performance on slower systems.
- Texture data is limited to numbers 1–9; additional numbers repeat this range.

## License
This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.
