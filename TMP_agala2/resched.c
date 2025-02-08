/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sched.h>
#include <math.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
extern int def_sched_var;
extern double expdev(double lambda);
// int last_run[NPROC];

/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int resched()
{
	register struct	pentry	*nptr;	/* pointer to new process entry */
	register struct	pentry	*optr;	/* pointer to old process entry */

	if(def_sched_var == EXPDISTSCHED)
	{
		int pid, lowerBound, randp;

		randp = (int) expdev(0.1);

		// kprintf("\nThe rand num is: %d", randp);
		optr = &proctab[currpid];

		// old process to ready queue
    	if (optr->pstate == PRCURR) {
    	    optr->pstate = PRREADY;
    	    insert(currpid, rdyhead, optr->pprio);
    	}

		pid = q[rdyhead].qnext;
		lowerBound = pid; //lower bound for round robin

		while(pid != rdytail)
		{
			// kprintf("\n Process %d in ready queue with priority %d", pid, q[pid].qkey);
			if(q[pid].qkey > randp && pid != NULLPROC)
			{
				break;
			}
			else if(q[lowerBound].qkey < q[pid].qkey)
			{
				lowerBound = pid;
			}

			pid = q[pid].qnext;
		}

		//schedule round robin
		if(pid == rdytail)
		{
			pid = lowerBound;
		}

		// kprintf("\nProcess scheduled is pid: %d and its priority is: %d", pid, q[pid].qkey);
		
		//new process as current process
		currpid = pid;
		nptr = &proctab[currpid];
    	nptr->pstate = PRCURR;
		dequeue(pid);
	#ifdef RTCLOCK
	    preempt = QUANTUM;
	#endif

	    ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
		return OK;
	}
	else if(def_sched_var == LINUXSCHED)
	{
		// kprintf("\nScheduler called!");
		int pid, maxGoodness, chosenProc;

		optr = &proctab[currpid];

		optr->goodness += preempt - optr->counter;
		optr->counter = preempt;

		if (preempt <=0 || currpid == NULLPROC)
		{
			optr->goodness = 0;
		}

		//putting currprocess back in ready queue
		if(optr->pstate == PRCURR)
		{
			optr->pstate = PRREADY;
        	insert(currpid,rdyhead,optr->goodness);
		}

		pid = q[rdyhead].qnext;
		maxGoodness = 0;
		chosenProc = 0;

		//finding good-est process
		while(pid != rdytail)
		{
			if(proctab[pid].goodness > maxGoodness)
			{
				maxGoodness = proctab[pid].goodness;
				chosenProc = pid;
			}

			pid = q[pid].qnext;
		}

		if(maxGoodness>0)
		{
			nptr = &proctab[(currpid = dequeue(chosenProc))];
			nptr->pstate = PRCURR;

			preempt = nptr->counter;
			ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
            return OK;
		}
		else
		{
			epoch_init();
			preempt = optr->counter;

			if(currpid == NULLPROC)
			{
				optr->pstate = PRCURR;
				currpid = dequeue(NULLPROC);
				return OK;
			}
			else
			{
				nptr = &proctab[(currpid = dequeue(NULLPROC)) ];
                nptr->pstate = PRCURR;
					
				#ifdef RTCLOCK
                	preempt = QUANTUM;
                #endif

                ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
                return OK;	

			}
		}
	}
	//default xinu scheduler
	else
	{
		/* no switch needed if current process priority higher than next*/

		if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
		   (lastkey(rdytail)<optr->pprio)) {
			return(OK);
		}

		/* force context switch */

		if (optr->pstate == PRCURR) {
			optr->pstate = PRREADY;
			insert(currpid,rdyhead,optr->pprio);
		}

		/* remove highest priority process at end of ready list */

		nptr = &proctab[ (currpid = getlast(rdytail)) ];
		nptr->pstate = PRCURR;		/* mark it currently running	*/
	#ifdef	RTCLOCK
		preempt = QUANTUM;		/* reset preemption counter	*/
	#endif

		ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

		/* The OLD process returns here when resumed. */
		return OK;
	}

	return OK;
}