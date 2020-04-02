#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

//using DFS to traverse all the processes.
void DFS_Traverse(struct task_struct *task_iteration){
    struct task_struct *child_cursor = NULL;
    printk(KERN_INFO "pid: %d| pname: %s| state: %ld\n", task_iteration->pid, task_iteration->comm, task_iteration->state);
//    printk(KERN_INFO "pid: %d| ppid: %d| pname: %s| state: %ld\n", task_iteration->pid, task_iteration->parent->pid, task_iteration->comm, task_iteration->state);

    list_for_each_entry(child_cursor, &task_iteration->children, sibling) {
        DFS_Traverse(child_cursor);
    }
}

/*This function is called when the module is loaded. */
int simple_init(void){
    printk(KERN_INFO "List all the tasks by DFS:\n");
    DFS_Traverse(&init_task);
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

