/*  -*- LPC -*-  */
//
// queue.h: Queue Object
// A FILO (First In Last Out) data structure.
//
// Make sure all conditions are satisfied before calling these functions!

/*
 * POST: is_empty() == "TRUE"
 */
void create();

/*
 * PRE: 'filename' is a fully qualified path (less .o extension)
 * POST: returns true if save succeeded
 */
int save(string filename);

/*
 * PRE:  'filename' is a fully qualified path to an existing 
 *       file (less .o extension)
 * POST: returns true if restore succeeded
 */
int restore(string filename);

/*
 * POST: returns "TRUE" if queue is empty
 */
boolean is_empty();

/*
 * MODIFIES: the queue
 * POST: 'item' is now the last item in the queue
 */
void queue(mixed item);

/*
 * POST: returns "TRUE" if 'item' is in the queue
 */
boolean queued(mixed item);

/*
 * PRE: queued(item) == "TRUE"
 * MODIFIES: the queue
 * POST: 'item' is removed from the queue.  
 */
void dequeue(mixed item);

/*
 * PRE: is_empty() == "FALSE"
 * MODIFIES: the queue
 * POST: first item is pulled from the queue and returned
 */
mixed pull();
