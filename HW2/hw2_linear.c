#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

/*This function is called when the module is loaded. */
int simple_init(void){
    struct task_struct *task_iteration;
    printk(KERN_INFO "List all the tasks:\n");

    for_each_process(task_iteration){
        printk(KERN_INFO "pid: %d| pname: %s| state: %ld\n", task_iteration->pid, task_iteration->comm, task_iteration->state);
    }

    return 0;
}

/*This function is called when the module is removed. */
void simple_exit(void){
    printk(KERN_INFO "Remove module\n");
}

/*Macros for registering module entry and exit points.*/
module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");

