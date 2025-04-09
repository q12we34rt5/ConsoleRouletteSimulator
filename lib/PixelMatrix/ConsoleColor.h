#pragma once

#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <cctype>
#include <stdexcept>

struct RGB {
    uint8_t R, G, B;
    constexpr RGB() : R{0}, G{0}, B{0} {}
    constexpr RGB(uint8_t R, uint8_t G, uint8_t B) : R{R}, G{G}, B{B} {}
    constexpr RGB(int R, int G, int B) : R{uint8_t(R)}, G{uint8_t(G)}, B{uint8_t(B)} {}
    constexpr RGB(double R, double G, double B) : R{uint8_t(R * 255)}, G{uint8_t(G * 255)}, B{uint8_t(B * 255)} {}
    RGB(const std::string& ccode) {
        if (ccode.length() == 6 && ccode.find_first_not_of("0123456789ABCDEFabcdef") == std::string::npos) {
            int color = std::stoi(ccode, nullptr, 16);
            R = (color & 0xFF0000) >> 16;
            G = (color & 0x00FF00) >> 8;
            B = (color & 0x0000FF);
        } else {
            throw std::invalid_argument(ccode);
        }
    }
    std::string toString() const {
        return static_cast<std::ostringstream&&>(
            std::ostringstream()
            << std::hex
            << std::setfill('0') << std::setw(2) << uint32_t(R)
            << std::setfill('0') << std::setw(2) << uint32_t(G)
            << std::setfill('0') << std::setw(2) << uint32_t(B))
            .str();
    }
};

class ConsoleColor {
public:
    enum Mode : uint8_t {
        kDefault,
        kDefaultForeground,
        kDefaultBackground,
        kForeground,
        kBackground,
    };

    constexpr ConsoleColor() : R{0}, G{0}, B{0}, mode{kDefault} {}
    constexpr ConsoleColor(uint8_t R, uint8_t G, uint8_t B, Mode mode = kForeground) : R{R}, G{G}, B{B}, mode{mode} {}
    constexpr ConsoleColor(RGB rgb, Mode mode = kForeground) : ConsoleColor{rgb.R, rgb.G, rgb.B, mode} {}

    constexpr bool operator==(const ConsoleColor& other) const { return mode == other.mode && R == other.R && G == other.G && B == other.B; }
    constexpr bool operator!=(const ConsoleColor& other) const { return !(*this == other); }

    friend std::ostream& operator<<(std::ostream& os, const ConsoleColor& cc) {
        std::string ansi_color_escape = "\033[";
        switch (cc.mode) {
            case kDefaultForeground:
                ansi_color_escape += "39m";
                break;
            case kDefaultBackground:
                ansi_color_escape += "49m";
                break;
            case kForeground:
                ansi_color_escape += "38;2;" + std::to_string(cc.R) + ';' + std::to_string(cc.G) + ';' + std::to_string(cc.B) + 'm';
                break;
            case kBackground:
                ansi_color_escape += "48;2;" + std::to_string(cc.R) + ';' + std::to_string(cc.G) + ';' + std::to_string(cc.B) + 'm';
                break;
            default:
                ansi_color_escape += "39m\033[49m";
                break;
        }
        return os << ansi_color_escape;
    }

    union {
        struct { uint8_t R, G, B; };
        RGB color;
    };
    Mode mode;
};
