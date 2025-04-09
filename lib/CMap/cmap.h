#pragma once

#include <cctype>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace cm {

struct RGB {
    uint8_t R, G, B;
    constexpr RGB() : R{0}, G{0}, B{0} {}
    constexpr RGB(uint8_t R, uint8_t G, uint8_t B) : R{R}, G{G}, B{B} {}
    constexpr RGB(int R, int G, int B) : R{uint8_t(R)}, G{uint8_t(G)}, B{uint8_t(B)} {}
    constexpr RGB(double R, double G, double B) : R{uint8_t(R * 255)}, G{uint8_t(G * 255)}, B{uint8_t(B * 255)} {}
    RGB(const std::string& ccode)
    {
        if (ccode.length() == 6 && ccode.find_first_not_of("0123456789ABCDEFabcdef") == std::string::npos) {
            int color = std::stoi(ccode, nullptr, 16);
            R = (color & 0xFF0000) >> 16;
            G = (color & 0x00FF00) >> 8;
            B = (color & 0x0000FF);
        } else {
            throw std::invalid_argument(ccode);
        }
    }
    std::string toString() const
    {
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

public:
    // static const ConsoleColor kBlack;
    // static const ConsoleColor kRed;
    // static const ConsoleColor kGreen;
    // static const ConsoleColor kYellow;
    // static const ConsoleColor kBlue;
    // static const ConsoleColor kMagenta;
    // static const ConsoleColor kCyan;
    // static const ConsoleColor kWhite;
    // static const ConsoleColor kBrightBlack;
    // static const ConsoleColor kBrightRed;
    // static const ConsoleColor kBrightGreen;
    // static const ConsoleColor kBrightYellow;
    // static const ConsoleColor kBrightBlue;
    // static const ConsoleColor kBrightMagenta;
    // static const ConsoleColor kBrightCyan;
    // static const ConsoleColor kBrightWhite;

    constexpr ConsoleColor() : R{0}, G{0}, B{0}, mode{kDefault} {}
    constexpr ConsoleColor(uint8_t R, uint8_t G, uint8_t B, Mode mode = kForeground) : R{R}, G{G}, B{B}, mode{mode} {}
    constexpr ConsoleColor(RGB rgb, Mode mode = kForeground) : ConsoleColor{rgb.R, rgb.G, rgb.B, mode} {}

    friend std::ostream& operator<<(std::ostream& os, const ConsoleColor& cc)
    {
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
        struct {
            uint8_t R, G, B;
        };
        RGB color;
    };
    Mode mode;
};

class CMap {
public:
    enum Mode {
        kLinear,
        kBspline
    };

    CMap() : knots_{{0x00, 0x00, 0x00}, {0xff, 0xff, 0xff}}, mode_{kLinear}, start_{0.0}, end_{1.0} {}
    CMap(std::vector<RGB> knots, double start = 0.0, double end = 1.0, Mode mode = kLinear) : knots_{std::move(knots)}, mode_{mode}, start_{start}, end_{end} {}
    CMap(std::initializer_list<RGB> knots, double start = 0.0, double end = 1.0, Mode mode = kLinear) : knots_{knots}, mode_{mode}, start_{start}, end_{end} {}

    CMap setRange(double start, double end) const
    {
        return {knots_, start, end, mode_};
    }

    RGB operator[](double x) const
    {
        double x_ratio = (x - start_) / (end_ - start_);
        size_t knots_size = knots_.size();
        int knot_index = int(x_ratio * (knots_size - 1));
        const RGB& cstart = knots_[knot_index % knots_size];
        const RGB& cend = knots_[(knot_index + 1) % knots_size];
        double cstart_ratio = double(knot_index % knots_size) / (knots_size - 1);
        double cend_ratio = double((knot_index + 1) % knots_size) / (knots_size - 1);
        double r = (x_ratio - cstart_ratio) / (cend_ratio - cstart_ratio);
        return {
            uint8_t((1 - r) * cstart.R + r * cend.R),
            uint8_t((1 - r) * cstart.G + r * cend.G),
            uint8_t((1 - r) * cstart.B + r * cend.B)};
    }

private:
    std::vector<RGB> knots_;
    Mode mode_;
    double start_;
    double end_;

public:
    static std::map<std::string, CMap> palettes;
    static CMap default_color_map;
};

} // namespace cm
