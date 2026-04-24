// Logger.hpp
#pragma once
#include <Windows.h>
#include <string>
#include <cstdio>
#include <cstdarg>

class Logger {
public:
    static void info(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        log("INFO", fmt, args);
        va_end(args);
    }

    static void warn(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        log("WARN", fmt, args);
        va_end(args);
    }

    static void error(const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        log("ERROR", fmt, args);
        va_end(args);
    }

private:
    static void log(const char* level, const char* fmt, va_list args) {
        char buf[1024];
        vsnprintf(buf, sizeof(buf), fmt, args);

        char out[1100];
        snprintf(out, sizeof(out), "[MCHook][%s] %s\n", level, buf);

        OutputDebugStringA(out);
    }
};