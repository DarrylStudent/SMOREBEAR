#include "Handler.h"
#include "menu.h"
#include "keyPoller.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <atomic>
#include <chrono>
#include <conio.h>
#include <string>

Handler::Handler(ControlState &cState) : cState(cState) {}

void Handler::viewProcess(Process *process)
{
    while (true)
    {
        std::string input = pollInput(process->getProcessName() + ":\\> ");

        if (input == "exit")
        {
            std::cout << "\033[2J\033[1;1H";
            printMenu();
            break;
        }
        else if (input == "process-smi")
        {
            process->viewProcess();
        }
        else if (!input.empty())
        {
            std::cout << "Unknown command: " << input << std::endl;
        }
    }
}

void Handler::loadConfig(const std::string &filename)
{
    std::ifstream file(filename);

    std::string key;
    while (file >> key)
    {
        if (key == "num-cpu")
            file >> cState.num_cpu;
        else if (key == "scheduler")
            file >> cState.scheduler;
        else if (key == "quantum-cycles")
            file >> cState.quantum_cycles;
        else if (key == "batch-process-freq")
            file >> cState.batch_process_freq;
        else if (key == "min-ins")
            file >> cState.min_ins;
        else if (key == "max-ins")
            file >> cState.max_ins;
        else if (key == "delay-per-exec")
            file >> cState.delay_per_exec;
        else
        {
            std::string dummy;
            std::getline(file, dummy); // Skip malformed line
        }
    }
}

bool Handler::handleCommand(const std::string &command)
{
    if (command == "exit")
    {
        return false;
    }

    if (!cState.initialized)
    {
        if (command == "initialize")
        {
            loadConfig("config.txt");
            std::cout << "Configuration loaded successfully:\n";
            cState.initialized = true;
        }
        else
        {
            std::cout << "Please initialize the system first by typing 'initialize'.\n";
        }
    }
    else
    {
        // After initialization, handle other commands
        if (command.rfind("screen -s ", 0) == 0)
        {
            std::string name = command.substr(10); // Extract name
            Process *process = processManager.createProcess(name, cState);

            if (!process)
            {
                std::cout << "A screen with that name already exists. Please choose a different name.\n";
            }
            else
            {
                std::cout << "\033[2J\033[1;1H"; // Clear
                process->viewProcess();
                viewProcess(process);
            }
        }
        else if (command.rfind("screen -r ", 0) == 0)
        {
            std::string name = command.substr(10);
            Process *process = processManager.getProcessByName(name, cState);
            if (!process)
            {
                std::cout << "Process " << name << " not found.\n";
            }
            else
            {
                std::cout << "\033[2J\033[1;1H";
                process->viewProcess();
                viewProcess(process);
            }
        }
        else if (command == "screen -ls")
        {
            processManager.printProcessList(cState);
        }
        else if (command == "report-util")
        {
            processManager.saveReportFile("csopesy-log.txt", cState);
            std::cout << "Report saved to csopesy-log.txt\n";
        }
        else if (command == "scheduler-test")
        {
            cState.batchRunning = true;
            return true;
        }
        else if (command == "scheduler-stop")
        {
            cState.batchRunning = false;
        }
        else if (!command.empty())
        {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }

    return true;
}