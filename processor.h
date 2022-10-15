#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "..\Assembler\Common.h"
#include "text-sort.h"
#include "stack.h"

const int RAMSIZE = 400;

int ProcessorMain(struct cpu* cpu);

int CpuCtor(struct cpu* cpu);

int CpuDetor(struct cpu* cpu);

int CpuInfoCheck(struct cpu* cpu);

int CpuDump(struct cpu cpu, int errors, int line, const char* func, const char* file);

int CpuVerr(struct cpu* cpu);

int SetLabel(struct cpu* cpu);

int RamWrite(struct cpu* cpu);

int GetArg(struct cpu* cpu, char cmd);

int compare(const elem_t a, const elem_t b);

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
    elem_t regs[5];
    int ip;
    elem_t ram[RAMSIZE];
    struct stack commands;
    struct stack returns;
};

enum ProcessorErrors
{
    SignErr =   1 << 1,
    VerErr =    1 << 2,
    ARITHERR =  1 << 3,
    IPERR =     1 << 4,
};

enum Colors
{
    WHITE = 1, ///<Inficating white color
    GREEN = 2, ///<indicating green color
    RED   = 3, ///<Indicating red color
};

int SetColor(enum Colors color);

#endif //PROCESSOR_H
