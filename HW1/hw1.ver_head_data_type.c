#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/slab.h>

typedef struct My_Student {
    int student_id;
    int day;
    int month;
    int year;
    struct list_head node_data;
} My_Student_T;

struct list_head head_list_stu;
My_Student_T list_head_stu;

void ListAddTail(const int student_id, const int day, const int month, const int year, struct list_head *head_list_stu){
    My_Student_T *student = NULL;

    if((student = kmalloc(sizeof(struct My_Student), GFP_KERNEL)) == NULL){
        printk(KERN_INFO "Error: Failed to allocate kernel memory with kmalloc.\n");
        return;
    }

    student->student_id = student_id;
    student->day        = day;
    student->month      = month;
    student->year       = year;
    list_add_tail(&student->node_data, head_list_stu);
}

void TraverseList(struct list_head *head_list_stu){
    My_Student_T *student_ptr = NULL;
    list_for_each_entry(student_ptr, head_list_stu, node_data) {
        printk(KERN_INFO "%d, %d-%d-%d.\n", student_ptr->student_id, student_ptr->day, student_ptr->month, student_ptr->year);
    }
}

void DeleteList(struct list_head *head_list_stu){
    My_Student_T *student = NULL;
    My_Student_T *tmp     = NULL;

    list_for_each_entry_safe(student, tmp, head_list_stu, node_data){
        printk(KERN_INFO "Deleting: %d, %d-%d-%d.\n", student->student_id, student->day, student->month, student->year);
        list_del(&student->node_data);
        kfree(student);
    }
}

/*This function is called when the module is loaded. */
int simple_init(void){
    printk(KERN_INFO "Loading Module\n");
    INIT_LIST_HEAD(&list_head_stu.node_data);

    ListAddTail(106062541, 15, 7, 1976, &list_head_stu.node_data);
    ListAddTail(105062841, 25, 2, 1973, &list_head_stu.node_data);
    ListAddTail(104052142,  3, 8, 1542, &list_head_stu.node_data);
    ListAddTail(103543212, 30, 2, 1912, &list_head_stu.node_data);
    ListAddTail(101021242,  9, 2, 1938, &list_head_stu.node_data);
    /*
    ListAddTail(106062541, 15, 7, 1976, &head_list_stu);
    ListAddTail(105062841, 25, 2, 1973, &head_list_stu);
    ListAddTail(104052142,  3, 8, 1542, &head_list_stu);
    ListAddTail(103543212, 30, 2, 1912, &head_list_stu);
    ListAddTail(101021242,  9, 2, 1938, &head_list_stu);
    */

    //TraverseList(&head_list_stu);
    TraverseList(&list_head_stu.node_data);
    printk(KERN_INFO "Success!\n");
    return 0;
}

/*This function is called when the module is removed. */
void simple_exit(void){
    printk(KERN_INFO "Removing Module\n");
    //DeleteList(&head_list_stu);
    DeleteList(&list_head_stu.node_data);
    printk(KERN_INFO "Success!\n");
}

/*Macros for registering module entry and exit points.*/
module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");

