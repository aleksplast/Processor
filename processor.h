#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "text-sort.h"
#include "stack.h"

const int version = 1;

int Processor(struct cpu* cpu);

int CpuCtor(struct cpu* cpu);

int CpuDetor(struct cpu* cpu);

int CpuCheck(struct cpu* cpu);

int CpuDump(struct cpu cpu, int line, const char* func, const char* file);

int* GetAdr(struct cpu* cpu, char cmd);

int SetArg(struct cpu* cpu, char cmd);

struct cpuinfo
{
    int sign;
    int vers;
    int numofel;
};

struct cpu
{
    struct cpuinfo info;
    char* code;
    int regs[4];
    int ip;
    int ram[100];
    struct stack stk;
};

enum ComArgs{
    ARG_MEM     = 128,
    ARG_REG     = 64,
    ARG_IMMED   = 32
};

enum ProcessorErrors
{
    SignErr,
    VerErr,
};


#define DEF_CMD(name, num, arg, code) \
    CMD_##name = num,
enum Commands
{
#include "C:\Users\USER\Documents\GitHub\Assembler\cmd.h"
    NOCMD = 31
};
#undef DEF_CMD

#endif //PROCESSOR_H
