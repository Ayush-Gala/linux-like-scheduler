#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include<sched.h>

int def_sched_var = 0;
struct pentry *process;

void setschedclass(int sched_class)
{
    if(sched_class == EXPDISTSCHED)
    {
        def_sched_var = 1;
    }
    else if(sched_class == LINUXSCHED)
    {
        def_sched_var = 2;
    }
    else
    {
        def_sched_var = 0;
    }
}

int getschedclass()
{
    return def_sched_var;
}

void epoch_init()
{
    // kprintf("\n\nEpoch reset WOOOOOOOHHHHHHHOOOOOOOOOO!");
    int pid;
    for(pid=0;pid<NPROC;pid++)
    {
        process=&proctab[pid];
        if(process->pstate != PRFREE)
        {
            //unsused quantum or did not run
            if(process->counter == 0 || process->counter == process->tQuant)
            {
                process->tQuant = process->pprio;
            }
            else
            {
                process->tQuant = process->pprio + (int) (process->counter / 2);
            }

            process->counter = process->tQuant;
            process->goodness = process->pprio + process->counter;
        }

    }
}