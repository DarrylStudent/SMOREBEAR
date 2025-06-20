#pragma once
#include <conio.h>
#include <string>
#include <iostream>

// Polls for a line of input, echoes characters, handles backspace and enter.
// Returns the entered line when Enter is pressed.
inline std::string pollInput(const std::string& prompt)
{
    std::string input;
    std::cout << prompt;
    while (true)
    {
        if (_kbhit())
        {
            char ch = _getch();
            if (ch == '\r') // Enter key
            {
                std::cout << std::endl;
                break;
            }
            else if (ch == '\b') // Backspace
            {
                if (!input.empty())
                {
                    input.pop_back();
                    std::cout << "\b \b";
                }
            }
            else if (isprint(static_cast<unsigned char>(ch)))
            {
                input += ch;
                std::cout << ch;
            }
        }
    }
    return input;
}