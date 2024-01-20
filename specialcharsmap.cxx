#include "specialcharsmap.h"

SpecialCharsMap SpecialCharsMapFactory::create() {
    SpecialCharsMap b;

    b[0xB0] = "░"; b[0xB1] = "▒"; b[0xB2] = "▓"; b[0xB3] = "│";
    b[0xB4] = "┤"; b[0xB5] = "╡"; b[0xB6] = "╢"; b[0xB7] = "╖";
    b[0xB8] = "╕"; b[0xB9] = "╣"; b[0xBA] = "║"; b[0xBB] = "╗";
    b[0xBC] = "╝"; b[0xBD] = "╜"; b[0xBE] = "╛"; b[0xBF] = "┐";

    b[0xC0] = "└"; b[0xC1] = "┴"; b[0xC2] = "┬"; b[0xC3] = "├";
    b[0xC4] = "─"; b[0xC5] = "┼"; b[0xC6] = "╞"; b[0xC7] = "╟";
    b[0xC8] = "╚"; b[0xC9] = "╔"; b[0xCA] = "╩"; b[0xCB] = "╦";
    b[0xCC] = "╠"; b[0xCD] = "═"; b[0xCE] = "╬"; b[0xCF] = "╧";

    b[0xD0] = "╨"; b[0xD1] = "╤"; b[0xD2] = "╥"; b[0xD3] = "╙";
    b[0xD4] = "╘"; b[0xD5] = "╒"; b[0xD6] = "╓"; b[0xD7] = "╫";
    b[0xD8] = "╪"; b[0xD9] = "┘"; b[0xDA] = "┌"; b[0xDB] = "█";
    b[0xDC] = "▄"; b[0xDD] = "▌"; b[0xDE] = "▐"; b[0xDF] = "▀";
    
    return b;
}
