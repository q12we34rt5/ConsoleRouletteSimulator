#pragma once

#include "ConsoleColor.h"
#include <ostream>

class PixelMatrix {
public:
    struct UTF32 {
        constexpr UTF32() : data{'\0', '\0', '\0', '\0'} {}
        constexpr UTF32(const char* s) : data{s[0], s[1], s[2], '\0'} {}
        friend std::ostream& operator<<(std::ostream& os, const UTF32& c) { return os << c.data; }
        char data[4];
    };

    PixelMatrix(int rows, int cols) : rows_{rows}, cols_{cols} {
        matrix_ = new ConsoleColor[rows * cols]{};
        display_char_ = new UTF32[((rows + 1) >> 1) * cols]{};
        for (int row = 0; row < rows; row++)
            for (int col = 0; col < cols; col++) {
                operator[](row)[col].mode = row & 1 ? ConsoleColor::kBackground : ConsoleColor::kForeground;
                display_char(row, col) = "▀";

                operator[](row)[col].mode = row & 1 ? ConsoleColor::kDefaultBackground : ConsoleColor::kDefaultForeground;
                display_char(row, col) = " \0\0";
            }
    }

    ~PixelMatrix() {
        delete[] display_char_;
        delete[] matrix_;
    }

    friend std::ostream& operator<<(std::ostream& os, const PixelMatrix& pm) {
        constexpr ConsoleColor empty{};
        for (int row = 0; row < pm.rows_ >> 1 << 1; row += 2) {
            ConsoleColor prev_top_color = empty;
            ConsoleColor prev_bottom_color = empty;
            for (int col = 0; col < pm.cols_; col++) {
                ConsoleColor top_color = pm[row][col];
                ConsoleColor bottom_color = pm[row + 1][col];
                if (top_color != prev_top_color) {
                    os << top_color;
                    prev_top_color = top_color;
                }
                if (bottom_color != prev_bottom_color) {
                    os << bottom_color;
                    prev_bottom_color = bottom_color;
                }
                os << pm.display_char(row, col);
                // os << pm[row][col] << pm[row + 1][col] << pm.display_char(row, col);
            }
            os << empty << '\n';
        }
        if (pm.rows_ & 1) {
            ConsoleColor prev_color = empty;
            for (int col = 0; col < pm.cols_; col++) {
                ConsoleColor color = pm[pm.rows_ - 1][col];
                if (color != prev_color) {
                    os << color;
                    prev_color = color;
                }
                os << pm.display_char(pm.rows_ - 1, col);
                // os << pm[pm.rows_ - 1][col] << pm.display_char(pm.rows_ - 1, col);
            }
            os << empty << '\n';
        }
        return os;
    }

    inline constexpr ConsoleColor* operator[](int row) const {
        return matrix_ + row * cols_;
    }

    inline constexpr UTF32& display_char(int row, int col) const {
        return display_char_[(row >> 1) * cols_ + col];
    }

    inline constexpr void enable(int row, int col) {
        if (row == rows_ - 1 && rows_ & 1) {
            // top half mode
            auto& th_mode = operator[](row >> 1 << 1)[col].mode;
            if (th_mode == ConsoleColor::kForeground) return;
            th_mode = ConsoleColor::kForeground;
            display_char(row, col) = "▀";
        } else {
            // top/bottom half mode
            auto& th_mode = operator[](row >> 1 << 1)[col].mode;
            auto& bh_mode = operator[]((row >> 1 << 1) + 1)[col].mode;
            // top/bottom half enabled
            bool th_enabled = th_mode == ConsoleColor::kForeground || th_mode == ConsoleColor::kBackground;
            bool bh_enabled = bh_mode == ConsoleColor::kForeground || bh_mode == ConsoleColor::kBackground;
            if ((row & 1) == 0) { // top half
                if (th_enabled) return;
                if (bh_enabled) {
                    // "▄" -> "█"
                    th_mode = ConsoleColor::kForeground;
                    bh_mode = ConsoleColor::kBackground;
                    display_char(row, col) = "▀";
                } else {
                    // " " -> "▀"
                    th_mode = ConsoleColor::kForeground;
                    display_char(row, col) = "▀";
                }
            } else { // bottom half
                if (bh_enabled) return;
                if (th_enabled) {
                    // "▀" -> "█"
                    bh_mode = ConsoleColor::kBackground;
                } else {
                    // " " -> "▄"
                    th_mode = ConsoleColor::kDefaultBackground;
                    bh_mode = ConsoleColor::kForeground;
                    display_char(row, col) = "▄";
                }
            }
        }
    }
    inline constexpr void disable(int row, int col) {
        if (row == rows_ - 1 && rows_ & 1) {
            // top half mode
            auto& th_mode = operator[](row >> 1 << 1)[col].mode;
            if (th_mode != ConsoleColor::kForeground) return;
            th_mode = ConsoleColor::kDefaultForeground;
            display_char(row, col) = " \0\0";
        } else {
            // top/bottom half mode
            auto& th_mode = operator[](row >> 1 << 1)[col].mode;
            auto& bh_mode = operator[]((row >> 1 << 1) + 1)[col].mode;
            // top/bottom half enabled
            bool th_enabled = th_mode == ConsoleColor::kForeground || th_mode == ConsoleColor::kBackground;
            bool bh_enabled = bh_mode == ConsoleColor::kForeground || bh_mode == ConsoleColor::kBackground;
            if ((row & 1) == 0) { // top half
                if (!th_enabled) return;
                if (bh_enabled) {
                    // "█" -> "▄"
                    th_mode = ConsoleColor::kDefaultBackground;
                    bh_mode = ConsoleColor::kForeground;
                    display_char(row, col) = "▄";
                } else {
                    // "▀" -> " "
                    th_mode = ConsoleColor::kDefaultForeground;
                    display_char(row, col) = " \0\0";
                }
            } else { // bottom half
                if (!bh_enabled) return;
                if (th_enabled) {
                    // "█" -> "▀"
                    bh_mode = ConsoleColor::kDefaultBackground;
                } else {
                    // "▄" -> " "
                    th_mode = ConsoleColor::kDefaultForeground;
                    bh_mode = ConsoleColor::kDefaultBackground;
                    display_char(row, col) = " \0\0";
                }
            }
        }
    }

    inline constexpr int rows() const { return rows_; }
    inline constexpr int cols() const { return cols_; }

private:
    int rows_, cols_;
    ConsoleColor* matrix_;
    UTF32* display_char_;
};
