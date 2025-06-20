#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H

#include "Process.h"
#include <vector>
#include <memory>
#include <queue>

// Refactored: finishedScreens now stores finished processes in order of completion
class ProcessManager
{
    std::vector<std::unique_ptr<Process>> allProcesses; // master list
    std::queue<Process *> runningProcesses;             // processes on CPU
    std::queue<Process *> readyProcesses;               // processes waiting for CPU
    std::vector<Process *> finishedProcesses;           // finished processes in order of finishing

public:
    // Process management
    Process *createProcess(const std::string &name, ControlState &cState);
    Process *getProcessByName(const std::string &name, ControlState &cState)
    {
        for (const auto &process : allProcesses)
        {
            if (process->getProcessName() == name)
            {
                return process.get();
            }
        }
        return nullptr;
    }

    // Get Process Lists
    const std::vector<std::unique_ptr<Process>> &getAllProcesses() const { return allProcesses; }
    std::queue<Process *> &getRunningQueue() { return runningProcesses; }
    std::queue<Process *> &getReadyQueue() { return readyProcesses; }
    const std::vector<Process *> &getFinishedProcesses() const { return finishedProcesses; }

    // Moving Processes
    void moveToReady(Process *process)
    {
        process->setState(ProcessState::Ready);
        readyProcesses.push(process);
    }
    void moveToRunning(Process *process)
    {
        process->setState(ProcessState::Running);
        runningProcesses.push(process);
        removeFromQueue(readyProcesses, process);
    }
    void moveToFinished(Process *process)
    {
        process->setState(ProcessState::Finished);
        process->setFinishedAt(std::chrono::system_clock::now());
        finishedProcesses.push_back(process);
        removeFromQueue(runningProcesses, process);
    }

    // Printing and Saving
    void printProcessList(const ControlState &cState) const;
    void saveReportFile(const std::string &filename, const ControlState &cState) const;

private:
    // This is a helper function to remove a specific process from a queue
    void removeFromQueue(std::queue<Process *> &queue, Process *process)
    {
        std::queue<Process *> tempQueue;
        while (!queue.empty())
        {
            Process *p = queue.front();
            queue.pop();
            if (p != process)
            {
                tempQueue.push(p);
            }
        }
        queue = std::move(tempQueue);
    }
};

#endif