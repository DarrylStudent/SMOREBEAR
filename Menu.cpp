#include <iostream>
#include "Menu.h" 
 
void printMenu()
{
    std::cout << R"( _____  _____  ____  _____  ______  _______     __
  / ____|/ ____|/ __ \|  __ \|  ____|/ ____\ \   / /
 | |    | (___ | |  | | |__) | |__  | (___  \ \_/ / 
 | |     \___ \| |  | |  ___/|  __|  \___ \  \   /  
 | |____ ____) | |__| | |    | |____ ____) |  | |   
  \_____|_____/ \____/|_|    |______|_____/   |_|  )"
              << std::endl;
    std::cout << "Welcome to CSOPESY Emulator!" << std::endl;
    std::cout << std::endl;
    std::cout << "Developers:" << std::endl;
    std::cout << "Dulin, Andrea & Javier, Darryl" << std::endl;
    std::cout << std::endl;
    std::cout << "Last updated: \033[1;35m01-18-2024\033[0m" << std::endl; // \033[1;35m for magenta, \033[0m reset
    std::cout << "-------------------------------------------------------------" << std::endl;
    std::cout << std::endl;
}