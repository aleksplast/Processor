#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "processor.h"

int Processor(struct cpu* cpu)
{
    for (cpu->ip = 0; cpu->ip < cpu->info.numofel; cpu->ip++)
    {
        char cmd = cpu->code[cpu->ip];
        int val1 = 0, val2 = 0;

        switch(cmd & 31)
        {
#define DEF_CMD(name, num, arg, cod)                                    \
case CMD_##name:                                                        \
    cod                                                                 \
    break;
#include "C:\Users\USER\Documents\GitHub\Assembler\cmd.h"
#undef DEF_CMD
            default:
                printf("%c UNKNOWN COMMAND\n", *cpu->code);
                CpuDump(*cpu, __LINE__, __func__, __FILE__);
                break;
        }
    }
    return NOERR;
}

int CpuCtor(struct cpu* cpu)
{
    cpu->stk = {};
    StackCtor(&(cpu->stk), 5);
    struct cpuinfo info = {0, 0, 0};

    FILE* fp = fopen("out.txt", "r");

    long size = FileSize(fp);
    if ((cpu->code = (char*) calloc(size, sizeof(char)))== NULL)
        return MEMERR;

    fread(cpu->code, sizeof(char), size, fp);
    fclose(fp);

    CpuCheck(cpu);

    return NOERR;
}

int CpuDetor(struct cpu* cpu)
{
    free(cpu->code);
    StackDetor(&cpu->stk);
    cpu->ip = 0;

    return NOERR;
}

int CpuCheck(struct cpu* cpu)
{
    cpu->info.sign = *(int*) cpu->code;
    if (cpu->info.sign != 'C' + 256 * 'P')
        return SignErr;

    cpu->info.vers = *(int*) (cpu->code + sizeof(int));
    if (cpu->info.vers != version)
        return VerErr;

    cpu->info.numofel = *(int*) (cpu->code + 2 * sizeof(int));

    cpu->code += 3 * sizeof(int);

    return NOERR;
}

int CpuDump(struct cpu cpu, int line, const char* func, const char* file)
{
    FILE* logs = fopen("logs.txt", "a");
    fprintf(logs, "\n%s at ", func);
    fprintf(logs, "%s", file);
    fprintf(logs, "(%d)\n", line);
    for (int i = 0; i < cpu.info.numofel; i++)
    {
        fprintf(logs, "%2d ", i);
    }
    fprintf(logs, "\n");
    for(int i = 0; i < cpu.info.numofel; i++)
    {
        fprintf(logs, "%2d ", *(cpu.code + i));
    }
    fprintf(logs, "\n");
    for(int i = 0; i < cpu.ip + 1; i++)
    {
        fprintf(logs, "   ");
    }
    fprintf(logs, " ^\n");
    for(int i = 0; i < cpu.ip + 1; i++)
    {
        fprintf(logs, "   ");
    }
    fprintf(logs, " ip\n");

    fclose(logs);

    int errors = StackErr(&cpu.stk);
    StackDump(&cpu.stk, errors, line, func, file);
    UpdateHash(&cpu.stk);

    return NOERR;
}

int* GetAdr(struct cpu* cpu, char cmd)
{
    int arg = 0;
    int* argptr = NULL;

    if (cmd & ARG_IMMED)
    {
        arg += *(int*) (cpu->code + cpu->ip + 1);
        argptr = &arg;
        cpu->ip += sizeof(int);
    }
    if (cmd & ARG_REG)
    {
        cpu->ip += 1;
        arg += cpu->regs[cpu->code[cpu->ip]];
        argptr = cpu->regs + cpu->code[cpu->ip];
    }
    if (cmd & ARG_MEM)
    {
        argptr = cpu->ram + arg;
    }

    return argptr;
}

int SetArg(struct cpu* cpu, char cmd)
{
    int arg = 0;

    if (cmd & ARG_IMMED)
    {
        arg += *(int*) (cpu->code + cpu->ip + 1);
        cpu->ip += sizeof(int);
    }
    if (cmd & ARG_REG)
    {
        cpu->ip += 1;
        arg += cpu->regs[cpu->code[cpu->ip]];
    }
    if (cmd & ARG_MEM)
    {
        arg = cpu->ram[arg];
    }

    return arg;
}
