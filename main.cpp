#include <iostream>
#include <fstream>
#include <string>
#include "scheduler.h"
#include "Handler.h"
#include "menu.h"
#include "ControlState.h"
#include "keyPoller.h"

void schedulerLoop(ControlState &cState, ProcessManager &processManager);

int main()
{
    printMenu();
    ControlState controlState;
    Handler handler(controlState);

    // Wait for initialization before starting scheduler
    while (!controlState.initialized)
    {
        std::string command = pollInput("main:\\> ");
        if (!handler.handleCommand(command))
        {
            return 0; // Exit if user types "exit" before initializing
        }
    }

    // Now config is loaded, start the scheduler thread
    std::thread schedulerThread(schedulerLoop, std::ref(controlState), std::ref(handler.getProcessManager()));

    // Main command loop
    while (true)
    {
        std::string command = pollInput("main:\\> ");
        if (!handler.handleCommand(command))
        {
            break;
        }
    }

    // Optionally, signal the scheduler thread to exit if you want a clean shutdown
    // For example, set a flag in ControlState and check it in schedulerLoop

    if (schedulerThread.joinable())
    {
        schedulerThread.join();
    }

    return 0;
}