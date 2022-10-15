#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <TXLib.h>

#include "processor.h"

static elem_t arg = 0;
static elem_t* argptr = NULL;
const double EPSILON = 1e-10;

#define CPUCHECK if (int errors = CpuVerr(cpu))                                 \
                    DBG CpuDump(*cpu, errors, __LINE__, __func__, __FILE__);

int ProcessorMain(struct cpu* cpu)
{
    for (cpu->ip = 0; cpu->ip < cpu->info.numofel; cpu->ip++)
    {
        char cmd = cpu->code[cpu->ip];
        elem_t val1 = 0, val2 = 0;

        switch(cmd & 31)
        {
#define DEF_CMD(name, num, arg, cod)                                    \
case CMD_##name:                                                        \
    cod                                                                 \
    break;
#include "..\Assembler\cmd.h"
#undef DEF_CMD
            default:
                CPUCHECK
                printf("%c UNKNOWN COMMAND\n", *cpu->code);
                break;
        }
    }
    return NOERR;
}

int CpuCtor(struct cpu* cpu)
{
    cpu->commands = {};

    StackCtor(&cpu->commands, 5);
    StackCtor(&cpu->returns, 5);

    FILE* fp = fopen("out.txt", "r");

    long size = FileSize(fp);
    if ((cpu->code = (char*) calloc(size, sizeof(char)))== NULL)
        return MEMERR;

    fread(cpu->code, sizeof(char), size, fp);
    fclose(fp);

    CpuInfoCheck(cpu);

    CPUCHECK

    return NOERR;
}

int CpuDetor(struct cpu* cpu)
{
    CPUCHECK

    free(cpu->code);
    StackDetor(&cpu->commands);
    StackDetor(&cpu->returns);
    cpu->ip = 0;

    return NOERR;
}

int CpuInfoCheck(struct cpu* cpu)
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

int CpuDump(struct cpu cpu, int errors, int line, const char* func, const char* file)
{
    FILE* logs = fopen("logs.txt", "a");
    fprintf(logs, "\n%s at ", func);
    fprintf(logs, "%s", file);
    fprintf(logs, "(%d)\n", line);
    for (int i = 0; i < cpu.info.numofel; i++)
    {
        fprintf(logs, "%3d ", i);
    }
    fprintf(logs, "\n");
    for(int i = 0; i < cpu.info.numofel; i++)
    {
        fprintf(logs, "%3d ", *(cpu.code + i));
    }
    fprintf(logs, "\n");
    for(int i = 0; i < cpu.ip + 1; i++)
    {
        fprintf(logs, "    ");
    }
    fprintf(logs, " ^\n");
    for(int i = 0; i < cpu.ip + 1; i++)
    {
        fprintf(logs, "    ");
    }
    fprintf(logs, " ip\n");

    fclose(logs);

    errors = StackErr(&cpu.commands);
    StackDump(&cpu.commands, errors, line, func, file);

    errors = StackErr(&cpu.returns);
    StackDump(&cpu.returns, errors, line, func, file);

    UpdateHash(&cpu.commands);
    UpdateHash(&cpu.returns);

    return NOERR;
}

int CpuVerr(struct cpu* cpu)
{
    int errors = 0;

    assert(cpu != NULL);

    if (cpu->code == NULL)
        errors |= DATAERR;
    if (cpu->ip > cpu->info.numofel)
        errors |= IPERR;

    return errors;
}

int GetArg(struct cpu* cpu, char cmd)
{
    arg = 0;
    argptr = NULL;

    if (cmd & ARG_IMMED)
    {
        arg += *(elem_t*) (cpu->code + cpu->ip + 1);
        argptr = &arg;

        cpu->ip += sizeof(elem_t);
    }
    if (cmd & ARG_REG)
    {
        cpu->ip += 1;

        arg += cpu->regs[(int) cpu->code[cpu->ip]];
        argptr = cpu->regs + cpu->code[cpu->ip];
    }
    if (cmd & ARG_MEM)
    {
        argptr = cpu->ram + (int)arg;
        arg = cpu->ram[(int)arg];
    }

    return NOERR;
}

int SetLabel(struct cpu* cpu)
{
    int label = *(int*)(cpu->code + cpu->ip + 1);

    cpu->ip = label - 1;

    return NOERR;
}

int RamWrite(struct cpu* cpu)
{
    for (int i = 0; i < 20; i++)
    {
        for (int j = 0; j < 20; j++)
        {
            if (compare(cpu->ram[20 * i + j], 0) != 0)
            {
                SetColor(GREEN);
                printf(" *");
                SetColor(WHITE);
            }
            else
            {
                SetColor(RED);
                printf(" .");
                SetColor(WHITE);
            }
        }
        printf("\n");
    }

    return NOERR;
}

int SetColor(enum Colors color)
{
    if (color == WHITE)
        txSetConsoleAttr(FOREGROUND_WHITE);
    else if (color == GREEN)
        txSetConsoleAttr(FOREGROUND_GREEN);
    else if (color == RED)
        txSetConsoleAttr(FOREGROUND_RED);

    return NOERR;
}

int compare(const elem_t a, const elem_t b)
{
    assert(isfinite(a));
    assert(isfinite(b));

    if (fabs(a-b) < EPSILON)
        return 0;
    if ((a-b) > EPSILON)
        return 1;
    if ((a-b) < EPSILON)
        return -1;

    return NOERR;
}
