#ifndef SCREEN_H
#define SCREEN_H

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <iomanip>
#include <sstream>
#include "ControlState.h"

struct ProcessLog
{
    std::chrono::system_clock::time_point timestamp;
    int core;
    std::string message;
};

enum class ProcessState
{
    Ready,
    Running,
    Finished
};

class Process
{
private:
    int id;
    static int nextId; // Id for the next process
    int quantumUsed;
    int core = -1;
    std::string processName;

    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point finishedAt;
    std::vector<ProcessLog> processLogs;

    int currentLine;
    int totalLines;

    ControlState &cState;
    ProcessState processState;

    /* InstructionHandler::InstructionState instructionState;

    bool hasBeenRecordedFinished = false;

    std::vector<Instruction> instructions; */

public:
    Process(const std::string &name, ControlState &cState);

    // Basic info getters
    int getId() const { return id; };
    std::string getProcessName() const { return processName; };
    std::chrono::system_clock::time_point getCreatedAt() const { return createdAt; };

    // Instruction lines get/set
    int getCurrentLine() const { return currentLine; };
    int getTotalLines() const { return totalLines; };
    void setCurrentLine(int line) { currentLine = line; };
    void setTotalLines(int lines) { totalLines = lines; };

    // Core get/set
    void setCurrentCore(int core) { this->core = core; }
    int getCurrentCore() const { return core; }

    // Quantum used
    int getQuantumUsed() const { return quantumUsed; }
    void resetQuantumUsed() { quantumUsed = 0; }
    void incrementQuantumUsed() { ++quantumUsed; }

    // Logs
    void addLog(int core, const std::string &message)
    {
        processLogs.push_back({std::chrono::system_clock::now(), core, message});
    }

    // State handler
    bool isFinished() const { return processState == ProcessState::Finished; }
    ProcessState getState() const { return processState; }
    void setState(ProcessState newState)
    {
        processState = newState;
        if (newState == ProcessState::Finished)
        {
            finishedAt = std::chrono::system_clock::now();
        }
    }
    std::chrono::system_clock::time_point getFinishedAt() const { return finishedAt; };
    void setFinishedAt(const std::chrono::system_clock::time_point &tp) { finishedAt = tp; }

    // Process execution
    void viewProcess() const;
    void executeStep();

    /* InstructionHandler::InstructionState getInstructionState() const { return instructionState; }

    std::string getCurrentInstructionString() const;

    std::vector<Instruction> &getInstructions() { return instructions; }
    void setInstructions(const std::vector<Instruction> &instrs)
    {
        instructions = instrs;
        totalLines = static_cast<int>(instrs.size());
    } */
};

#endif