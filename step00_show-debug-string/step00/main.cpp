// step00:
// Show debug string
//   Written by Tunjay Akbarli
// ===========================================

#include<Windows.h>

#include<iostream>

/// <summary>
/// Show debug formatted string to console
/// remarks only debug mode
/// </summary>
/// <param name="format">format (%d, %f, ...)</param>
/// <param name="">variable length parameter</param>
void DebugOutputFormatString(const char* format, ...)
{
    va_list valist;
    va_start(valist, format);
    vprintf(format, valist);
    va_end(valist);
}

/// <summary>
/// main
/// </summary>
/// <returns></returns>
int main()
{
    DebugOutputFormatString("Show window test.");

    return 0;
}

// ===========================================
// End of file