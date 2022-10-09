#include <stdio.h>
#include <stdlib.h>

#include "processor.h"

int Processor(const char* input)
{
    struct stack stk = {};
    struct cpu cpu = {};
    StackCtor(&stk, 5);
    const char* out = "out.txt";
    FILE* fp = fopen(input, "r");
    struct cpuinfo info = {0, 0, 0};
    long size = FileSize(fp);
    cpu.code = (char*) calloc(size, sizeof(char));

    fread(cpu.code, sizeof(char), size, fp);

    fclose(fp);

    CpuCheck(cpu.code, &info);

    for (cpu.ip = 12; cpu.ip < size; cpu.ip++)
    {
        char cmd = cpu.code[cpu.ip];
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
                printf("UNKNOWN COMMAND\n");
                break;
        }
    }
    return NOERR;
}

int CpuCheck(char* buffer, struct cpuinfo* info)
{
    info->sign = *(int*) buffer;
    if (info->sign != 'C' + 256 * 'P')
        return SignErr;

    info->vers = *(int*) (buffer + sizeof(int));
    if (info->vers != version)
        return VerErr;

    info->numofel = *(int*) (buffer + sizeof(int));

    return NOERR;
}

int CpuDump(struct stack* stk, int* buffer, int ip, int line, const char* func, const char* file)
{
    FILE* logs = fopen("logs.txt", "a");
    fprintf(logs, "\n%s at ", func);
    fprintf(logs, "%s", file);
    fprintf(logs, "(%d)\n", line);
    for (int i = 0; i < buffer[3]; i++)
    {
        fprintf(logs, "%2d ", i);
    }
    fprintf(logs, "\n");
    for(int i = 0; i < buffer[3]; i++)
    {
        fprintf(logs, "%2d ", buffer[i + 3]);
    }
    fprintf(logs, "\n");
    for(int i = 0; i < ip-3; i++)
    {
        fprintf(logs, "   ");
    }
    fprintf(logs, " ^\n");
    for(int i = 0; i < ip-3; i++)
    {
        fprintf(logs, "   ");
    }
    fprintf(logs, " ip\n");

    fclose(logs);

    int errors = StackErr(stk);
    StackDump(stk, errors, line, func, file);
    UpdateHash(stk);
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
