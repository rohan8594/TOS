
#include <kernel.h>

#include "disptable.c"


PROCESS         active_proc;


/* 
 * Ready queues for all eight priorities.
 */
PCB            *ready_queue[MAX_READY_QUEUES];

unsigned		ready_procs; // bits in ready_procs tell you which ready queue is empty


/* 
 * add_ready_queue
 *----------------------------------------------------------------------------
 * The process pointed to by p is put the ready queue.
 * The appropiate ready queue is determined by p->priority.
 */

void add_ready_queue(PROCESS proc)
{
	int prio;
	assert(proc->magic == MAGIC_PCB); // error/consistency check
	prio = proc->priority;

	if (ready_queue[prio] == NULL) {

		ready_queue[prio] = proc;
		proc->next = proc;
		proc->prev = proc;
		ready_procs |= 1 << prio;

	} else {

		proc->next = ready_queue[prio];
		proc->prev = ready_queue[prio]->prev;
		ready_queue[prio]->prev->next = proc; // prev_tail->next points to new process (new tail)
		ready_queue[prio]->prev = proc; // head->prev points to new process

	}

	proc->state = STATE_READY;
}


/* 
 * remove_ready_queue
 *----------------------------------------------------------------------------
 * The process pointed to by p is dequeued from the ready
 * queue.
 */

void remove_ready_queue(PROCESS proc)
{
	int prio;
	assert(proc->magic == MAGIC_PCB);
	prio = proc->priority;

	if (proc->next == proc) {

		// single element in list
		ready_queue[prio] = NULL;
		ready_procs &= ~(1 << prio); // switch particular ready_procs bit to 0

	} else {

		ready_queue[prio] = proc->next;
		proc->next->prev = proc->prev;
		proc->prev->next = proc->next;

	}
}


/* 
 * become_zombie
 *----------------------------------------------------------------------------
 * Turns the calling process into a zombie. It will be removed from the ready
 * queue and marked as being in STATE_ZOMBIE.
 */

void become_zombie()
{
    active_proc->state = STATE_ZOMBIE;
    while (1);
}


/* 
 * dispatcher
 *----------------------------------------------------------------------------
 * Determines a new process to be dispatched. The process
 * with the highest priority is taken. Within one priority
 * level round robin is used.
 */


PROCESS dispatcher()
{
	PROCESS new_proc;
	unsigned i;

	
	i = table[ready_procs]; // gives queue with highest priority that isn't empty.
	assert(i != -1); // TOS assumes ready_queue is always non empty

	if (i == active_proc->priority) {
		// round-robin to next proc on same priority level
		new_proc = active_proc->next;
	} else {
		new_proc = ready_queue[i];
	}

	return new_proc;
}


/* 
 * resign
 *----------------------------------------------------------------------------
 * The current process gives up the CPU voluntarily. The
 * next running process is determined via dispatcher().
 * The stack of the calling process is setup such that it
 * looks like an interrupt.
 */
void resign()
{
}



/* 
 * init_dispatcher
 *----------------------------------------------------------------------------
 * Initializes the necessary data structures.
 */

void init_dispatcher()
{
	for (int i = 0; i < MAX_READY_QUEUES; i++) {
		ready_queue[i] = NULL;
	}

	ready_procs = 0;
	add_ready_queue(active_proc); // setup first process
}
