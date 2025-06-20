#include "ProcessManager.h"
#include <iostream>
#include <fstream>
#include <random>

Process *ProcessManager::createProcess(const std::string &name, ControlState &cState)
{
    Process *existingProcess = getProcessByName(name, cState);
    if (existingProcess)
        return nullptr; // Process with this name already exists

    allProcesses.push_back(std::make_unique<Process>(name, cState));
    Process *newProcess = allProcesses.back().get();

    // Generate and assign random instructions
    int minIns = cState.min_ins;
    int maxIns = cState.max_ins;

    newProcess->setTotalLines(rand() % (maxIns - minIns + 1) + minIns); // Random total lines
    /* newScreen->setInstructions(generateRandomInstructions(minIns, maxIns, name)); */

    moveToReady(newProcess); // Add to ready queue initially
    return newProcess;
}

void ProcessManager::printProcessList(const ControlState &cState) const
{
    int total_cores = cState.num_cpu;
    int cores_used = runningProcesses.size();
    int cores_available = total_cores - cores_used;
    if (cores_available < 0)
        cores_available = 0;

    int cpu_util = (int)((cores_used * 100.0) / total_cores + 0.5); // Rounded

    std::cout << "CPU utilization: " << cpu_util << "%\n";
    std::cout << "Cores used: \033[1;34m" << cores_used << "\033[0m\n";
    std::cout << "Cores available: \033[38;5;208m" << cores_available << "\033[0m\n";
    std::cout << "\n---------------------------------------------\n";

    // Running processes
    std::cout << "Running processes:\n";
    auto runningQ = runningProcesses; // Make a copy to iterate

    while (!runningQ.empty())
    {
        const Process *proc = runningQ.front();
        runningQ.pop();

        std::cout << proc->getProcessName() << " (";
        auto tp = proc->getCreatedAt();
        std::time_t tt = std::chrono::system_clock::to_time_t(tp);
        std::tm *tm_ptr = std::localtime(&tt);
        std::cout << std::setfill('0')
                  << std::setw(2) << tm_ptr->tm_mon + 1 << "/"
                  << std::setw(2) << tm_ptr->tm_mday << "/"
                  << tm_ptr->tm_year + 1900 << " "
                  << std::setw(2) << tm_ptr->tm_hour << ":"
                  << std::setw(2) << tm_ptr->tm_min << ":"
                  << std::setw(2) << tm_ptr->tm_sec << "AM) ";

        std::cout << "Core: \033[1;35m" << proc->getCurrentCore() << "\033[0m  ";
        /* std::cout << " | " << scr->getCurrentInstructionString();
        if (scr->getInstructionState().sleepTicks > 0)
        {
            std::cout << " [SLEEPING: " << scr->getInstructionState().sleepTicks << "]";
        } */
        std::cout << "\033[1;34m" << proc->getCurrentLine() << "\033[0m / \033[1;34m" << proc->getTotalLines() << "\033[0m\n";
    }

    // Ready processes
    std::cout << "\nReady processes:\n";
    auto readyQ = readyProcesses; // Make a copy to iterate
    while (!readyQ.empty())
    {
        const Process *proc = readyQ.front();
        readyQ.pop();

        std::cout << proc->getProcessName() << " (";
        auto tp = proc->getCreatedAt();
        std::time_t tt = std::chrono::system_clock::to_time_t(tp);
        std::tm *tm_ptr = std::localtime(&tt);
        std::cout << std::setfill('0')
                  << std::setw(2) << tm_ptr->tm_mon + 1 << "/"
                  << std::setw(2) << tm_ptr->tm_mday << "/"
                  << tm_ptr->tm_year + 1900 << " "
                  << std::setw(2) << tm_ptr->tm_hour << ":"
                  << std::setw(2) << tm_ptr->tm_min << ":"
                  << std::setw(2) << tm_ptr->tm_sec << "AM) ";

        std::cout << "Ready   ";
        std::cout << "\033[1;34m" << proc->getCurrentLine() << "\033[0m / \033[1;34m" << proc->getTotalLines() << "\033[0m\n";
    }

    // Finished processes (in order of completion)
    std::cout << "\nFinished processes:\n";
    for (const auto &proc : finishedProcesses)
    {
        std::cout << proc->getProcessName() << " (";
        auto tp = proc->getCreatedAt();
        std::time_t tt = std::chrono::system_clock::to_time_t(tp);
        std::tm *tm_ptr = std::localtime(&tt);
        std::cout << std::setfill('0')
                  << std::setw(2) << tm_ptr->tm_mon + 1 << "/"
                  << std::setw(2) << tm_ptr->tm_mday << "/"
                  << tm_ptr->tm_year + 1900 << " "
                  << std::setw(2) << tm_ptr->tm_hour << ":"
                  << std::setw(2) << tm_ptr->tm_min << ":"
                  << std::setw(2) << tm_ptr->tm_sec << "AM) ";

        std::cout << "Finished   \033[1;34m" << proc->getTotalLines() << "\033[0m / \033[1;34m" << proc->getTotalLines() << "\033[0m\n";
    }
    std::cout << "---------------------------------------------\n";
}

void ProcessManager::saveReportFile(const std::string &filename, const ControlState &cState) const
{
    std::ofstream ofs(filename);

    int total_cores = cState.num_cpu;
    int cores_used = runningProcesses.size();
    int cores_available = total_cores - cores_used;
    if (cores_available < 0)
        cores_available = 0;

    int cpu_util = (int)((cores_used * 100.0) / total_cores + 0.5); // Rounded

    ofs << "CPU utilization: " << cpu_util << "%\n";
    ofs << "Cores used: " << cores_used << "\n";
    ofs << "Cores available: " << cores_available << "\n";
    ofs << "\n---------------------------------------------\n";

    // Running processes
    ofs << "Running processes:\n";
    auto runningQ = runningProcesses; // Make a copy to iterate
    while (!runningQ.empty())
    {
        const Process *proc = runningQ.front();
        runningQ.pop();

        ofs << proc->getProcessName() << " (";
        auto tp = proc->getCreatedAt();
        std::time_t tt = std::chrono::system_clock::to_time_t(tp);
        std::tm *tm_ptr = std::localtime(&tt);
        ofs << std::setfill('0')
            << std::setw(2) << tm_ptr->tm_mon + 1 << "/"
            << std::setw(2) << tm_ptr->tm_mday << "/"
            << tm_ptr->tm_year + 1900 << " "
            << std::setw(2) << tm_ptr->tm_hour << ":"
            << std::setw(2) << tm_ptr->tm_min << ":"
            << std::setw(2) << tm_ptr->tm_sec << "AM) ";

        ofs << "Core: " << proc->getCurrentCore() << "  ";
        ofs << proc->getCurrentLine() << " / " << proc->getTotalLines() << "\n";
    }

    // Ready processes
    ofs << "\nReady processes:\n";
    auto readyQ = readyProcesses; // Make a copy to iterate
    while (!readyQ.empty())
    {
        const Process *proc = readyQ.front();
        readyQ.pop();

        ofs << proc->getProcessName() << " (";
        auto tp = proc->getCreatedAt();
        std::time_t tt = std::chrono::system_clock::to_time_t(tp);
        std::tm *tm_ptr = std::localtime(&tt);
        ofs << std::setfill('0')
            << std::setw(2) << tm_ptr->tm_mon + 1 << "/"
            << std::setw(2) << tm_ptr->tm_mday << "/"
            << tm_ptr->tm_year + 1900 << " "
            << std::setw(2) << tm_ptr->tm_hour << ":"
            << std::setw(2) << tm_ptr->tm_min << ":"
            << std::setw(2) << tm_ptr->tm_sec << "AM) ";

        ofs << "Waiting   ";
        ofs << proc->getCurrentLine() << " / " << proc->getTotalLines() << "\n";
    }

    // Finished processes (in order of completion)
    ofs << "\nFinished processes:\n";
    for (const auto &proc : finishedProcesses)
    {
        ofs << proc->getProcessName() << " (";
        auto tp = proc->getCreatedAt();
        std::time_t tt = std::chrono::system_clock::to_time_t(tp);
        std::tm *tm_ptr = std::localtime(&tt);
        ofs << std::setfill('0')
            << std::setw(2) << tm_ptr->tm_mon + 1 << "/"
            << std::setw(2) << tm_ptr->tm_mday << "/"
            << tm_ptr->tm_year + 1900 << " "
            << std::setw(2) << tm_ptr->tm_hour << ":"
            << std::setw(2) << tm_ptr->tm_min << ":"
            << std::setw(2) << tm_ptr->tm_sec << "AM) ";

        ofs << "Finished   " << proc->getTotalLines() << " / " << proc->getTotalLines() << "\n";
    }
    ofs << "---------------------------------------------\n";
    ofs.close();
}