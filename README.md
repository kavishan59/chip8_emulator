# CHIP-8 Emulator

A simple and efficient CHIP-8 emulator written in C using **SDL2** for graphics and **SDL\_mixer** for sound.

## Features

✅ **Full CHIP-8 CPU Emulation** (Instructions, Registers, Timers, Memory) 

✅ **Customizable Options**:

- `-l` **(Load Mode)**: Controls whether Fx55 and Fx65 increment the index register.
- `-s` **(Shift Mode)**: Defines how shift operations (`8XY6` and `8XYE`) behave.
  

✅ **Graphics Rendering with SDL2**-

✅ **Sound Support using SDL\_mixer** (Beep sound on `FX18` opcode)

✅ **Passes the Timendus CHIP-8 Test Suite**

## Installation

### **🔹 Dependencies**

Make sure you have the following installed:

- **SDL2** (for graphics)
- **SDL2\_mixer** (for sound)

**Linux (Debian/Ubuntu)**:

```bash
sudo apt install libsdl2-dev libsdl2-mixer-dev
```

**macOS (Homebrew)**:

```bash
brew install sdl2 sdl2_mixer
```

**Windows (MSYS2)**:

```bash
pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_mixer
```

### **🔹 Build the Emulator**

Clone the repository and compile the emulator using the provided Makefile:

```bash
git clone https://github.com/yourusername/chip8-emulator.git
cd chip8-emulator
make
```

This will generate an executable named `chip8`.

## Usage

### **Run a CHIP-8 ROM**

```bash
./chip8 <rom_path> [options]
```

### **Available Options**

| Option | Description                                                           |
| ------ | --------------------------------------------------------------------- |
| `-l`   | Fx55 and Fx65 **increment** `I` (Super CHIP-8 behavior)               |
| `-s`   | Use `VY` as the shifting register in `8XY6` and `8XYE` (Super CHIP-8) |

### **Example Usage**

```bash
./chip8 roms/INVADERS.ch8 -l -s 
```

## Key Mappings (CHIP-8 → Keyboard)

| CHIP-8 Keypad | Mapped Keyboard |
|--------------|----------------|
| `1` `2` `3` `C` | `7` `8` `9` `*` |
| `4` `5` `6` `D` | `4` `5` `6` `-` |
| `7` `8` `9` `E` | `1` `2` `3` `+` |
| `A` `0` `B` `F` | `→` `0` `.` `Enter` |

## 📜 License

This project is released under the **MIT License**. Feel free to modify and distribute.

## 🎮 Credits

- **Original CHIP-8 Specification** by **Joseph Weisbecker**
- **SDL2 and SDL\_mixer** for graphics & sound
- **Test ROMs** from **Timendus** and **other open-source developers**


