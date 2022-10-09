#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "text-sort.h"
#include "stack.h"

const int version = 1;

int Processor(const char* input);

int CpuCheck(char* buffer, struct cpuinfo* info);

int CpuDump(struct stack* stk, int* buffer, int ip, int line, const char* func, const char* file);

int* GetAdr(struct cpu* cpu, char cmd);

int SetArg(struct cpu* cpu, char cmd);

struct cpu
{
    char* code;
    int regs[4];
    int ip;
    int ram[100];
};

enum ComArgs{
    ARG_MEM     = 128,
    ARG_REG     = 64,
    ARG_IMMED   = 32
};

struct cpuinfo
{
    int sign;
    int vers;
    int numofel;
};

enum ProcessorErrors
{
    SignErr,
    VerErr
};


#define DEF_CMD(name, num, arg, code) \
    CMD_##name = num,
enum Commands
{
#include "cmd.h"
    NOCMD = 18
};
#undef DEF_CMD

#endif //PROCESSOR_H
