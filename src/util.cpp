#include "util.h"

unsigned char to_hex(unsigned char x) { return  x > 9 ? x + 55 : x + 48; }

void url_encode(std::string& out, const std::string& src) {
    size_t pos = 0, start = 0;
    while((pos = src.find_first_not_of("abcdefghijklmnopqrstuvwxyqABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_.~", start)) != std::string::npos) {
        out.append(src.c_str() + start, pos - start);
                if(src[pos] == ' ')
                    out += "+";
                else {
                    out += '%';
                    out += to_hex((unsigned char)src[pos] >> 4);
                    out += to_hex((unsigned char)src[pos] & 0xF);
                }
                start = ++pos;
            }
    out.append(src.c_str() + start, src.length() - start);
}

