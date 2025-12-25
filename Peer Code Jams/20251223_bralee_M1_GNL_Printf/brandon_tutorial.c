// HOW TO NOT CHANGE LOGIC (USING INDEXING TABLE?)

// 4 ways to do it (as far deep into the rabbithole I dug):
// 1. If else if else...
// 2. Indexing table (here, efficient, bottleneck is syscall)
// 3. Paired list
// 4. Mapping table/ perfect hash (OVERKILL)

// 2. vs 3. is paired list is clearer but 25 CPU cycles instead of 10
// counting branch predictions too. (BTW bottleneck is write syscall so-)

// Anw, first I will make a handler typedef for dispatching
typedef void (*t_handler)(void);

/ *************************************** /
// dispatcher function reading keys mapped from ascii
static void dispatch_key(int key)
{
    t_handler fn;

    if (key < 0 || key > 255)
        return;
    fn = g_handlers[(unsigned char)key];
    if (fn)
        fn();
} // This is NON-changing LOGIC FOR DISPATCHING

/ *************************************** /
// EXAMPLES NOT PRINTF
static void do_quit(void) { write(1, "quit\n", 5); }
static void do_help(void) { write(1, "help\n", 5); }
static void do_save(void) { write(1, "save\n", 5); }
static void do_load(void) { write(1, "load\n", 5); }

/ *************************************** /
// Add additional functions for keys that can be mapped from ascii key
// E.G. for "r" (say reload),
// do not modify dispatch_key.

// Add:
static void do_reload(void) { ... };
g_handlers['r'] = do_reload;

// Discussion
// if else if else NOT changing LOGIC/ ridiculous to fail someone during 
// live coding because "do you expect (them) to redo their whole printf if 
// they didn't use indexing table method?"