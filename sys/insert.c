/* insert.c  -  insert */

#include <conf.h>
#include <kernel.h>
#include <q.h>
#include <sched.h>

extern int def_sched_var;
/*------------------------------------------------------------------------
 * insert.c  --  insert an process into a q list in key order
 *------------------------------------------------------------------------
 */
int insert(int proc, int head, int key)
{
	int	next;			/* runs through list		*/
	int	prev;

	next = q[head].qnext;
	if(def_sched_var != LINUXSCHED && def_sched_var != EXPDISTSCHED)
	{
		while (q[next].qkey < key)	/* tail has maxint as key	*/
			next = q[next].qnext;
	}
	else
	{
		while (q[next].qkey <= key)	/* tail has maxint as key	*/
			next = q[next].qnext;
	}
	q[proc].qnext = next;
	q[proc].qprev = prev = q[next].qprev;
	q[proc].qkey  = key;
	q[prev].qnext = proc;
	q[next].qprev = proc;
	return(OK);
}
