#include "Process.h"
#include "Handler.h"
#include <iostream>
#include <random>

int Process::nextId = 1;

Process::Process(const std::string &name, ControlState &cState)
    : id(nextId++), cState(cState), processName(name), currentLine(0), totalLines(0),
      createdAt(std::chrono::system_clock::now()) {}

/* std::string Screen::getCurrentInstructionString() const
{
    if (currentLine >= instructions.size())
        return "FINISHED";
    const Instruction &instr = instructions[currentLine];
    std::string typeStr;
    switch (instr.type)
    {
    case Instruction::Type::PRINT:
        typeStr = "PRINT";
        break;
    case Instruction::Type::DECLARE:
        typeStr = "DECLARE";
        break;
    case Instruction::Type::ADD:
        typeStr = "ADD";
        break;
    case Instruction::Type::SUBTRACT:
        typeStr = "SUBTRACT";
        break;
    case Instruction::Type::SLEEP:
        typeStr = "SLEEP";
        break;
    case Instruction::Type::FOR:
        typeStr = "FOR";
        break;
    default:
        typeStr = "UNKNOWN";
        break;
    }
    std::string argsStr;
    for (const auto &arg : instr.args)
    {
        if (!argsStr.empty())
            argsStr += ", ";
        argsStr += arg;
    }
    if (instr.type == Instruction::Type::FOR)
        argsStr += " [body size: " + std::to_string(instr.body.size()) + "] x" + std::to_string(instr.repeats);
    return typeStr + "(" + argsStr + ")";
} */

void Process::executeStep()
{
    if (processState == ProcessState::Finished)
    {
        setState(ProcessState::Finished);
        return;
    }

    currentLine++;

    /* // If sleeping, decrement and check if done
    if (instructionState.isSleeping)
    {
        if (instructionState.sleepTicks > 0)
        {
            instructionState.sleepTicks--;
            return;
        }
        else
        {
            instructionState.isSleeping = false;
            currentLine++; // Advance to next instruction after sleep
        }
    }

    // Only execute instruction if not sleeping
    if (!instructionState.isSleeping)
    {
        InstructionHandler::executeNextInstruction(
            *this, instructions, currentLine, instructionState, state);
    } */

    // Check if all instructions are done
    if (currentLine >= totalLines)
    {
        setState(ProcessState::Finished);
        setFinishedAt(std::chrono::system_clock::now());
        addLog(core, "Process finished!");
    }
}

void Process::viewProcess() const
{
    std::cout << "Process name: " << processName << "\n";
    std::cout << "ID: " << id << "\n";
    std::cout << "Logs:\n";
    for (const auto &log : processLogs)
    {
        std::time_t time = std::chrono::system_clock::to_time_t(log.timestamp);
        std::tm *tm_ptr = std::localtime(&time);
        std::ostringstream oss;
        oss << "("
            << std::setfill('0') << std::setw(2) << tm_ptr->tm_mon + 1 << "/"
            << std::setw(2) << tm_ptr->tm_mday << "/"
            << tm_ptr->tm_year + 1900 << " "
            << std::setw(2) << tm_ptr->tm_hour << ":"
            << std::setw(2) << tm_ptr->tm_min << ":"
            << std::setw(2) << tm_ptr->tm_sec << "AM)";

        std::cout << "\033[38;5;208m" << oss.str() << "\033[0m ";
        std::cout << "Core:\033[1;35m" << log.core << "\033[0m ";
        std::cout << "\"" << log.message << "\"\n";
    }
    if (processState == ProcessState::Finished)
    {
        std::cout << "Finished!\n";
    }
    else
    {
        std::cout << "\nCurrent instruction line: \033[38;5;208m" << getCurrentLine() << "\033[0m\n";
        std::cout << "Lines of code: \033[1;32m" << getTotalLines() << "\033[0m\n";
    }
}