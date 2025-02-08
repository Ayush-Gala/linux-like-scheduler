#ifndef _SCHED_H_
#define _SCHED_H_

#define EXPDISTSCHED 1
#define LINUXSCHED 2

extern int def_sched_var;
void setschedclass(int sched_class);
int getschedclass();
void epoch_init();
#endif