#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/string.h>

struct val_node {
	struct list_head list;
	int val;
};

static struct list_head val_list = LIST_HEAD_INIT(val_list);

static struct dentry *stack_dentry;
static struct dentry *push_dentry;
static struct dentry *pop_dentry;
static struct dentry *show_dentry;

static struct val_node *val_node_alloc(int val)
{
	struct val_node *node;

	node = kmalloc(sizeof(*node), GFP_KERNEL);
	INIT_LIST_HEAD(&node->list);
	node->val = val;

	return node;
}

static void val_list_push(int val)
{
	struct val_node *node = val_node_alloc(val);

	list_add(&node->list, &val_list);
}

static void val_list_pop(void)
{
	struct list_head *list = val_list.next;
	struct val_node *node = list_entry(list, struct val_node, list);

	list_del(list);
	kfree(node);
}

static int val_list_show(struct seq_file *m, void *v)
{
	struct val_node *node;

	list_for_each_entry(node, &val_list, list) {
		seq_printf(m, "%d ", node->val);
	}
	seq_printf(m, "\n");

	return 0;
}

static void val_list_release(void)
{
	struct val_node *node, *n;

	list_for_each_entry_safe(node, n, &val_list, list)
		kfree(node);
}

static ssize_t push_store(struct file *file, const char __user *buf,
		size_t size, loff_t *ppos)
{
	int n;

	if (sscanf(buf, "%d", &n) != 1)
		return -EINVAL;

	val_list_push(n);

	return size;
}

static ssize_t pop_store(struct file *file, const char __user *buf,
		size_t size, loff_t *ppos)
{
	bool val;
	int ret;

	ret = strtobool(buf, &val);
	if (ret < 0)
		return ret;

	if (val == false)
		return -EINVAL;

	if (list_empty(&val_list))
		return -ENOSPC;

	val_list_pop();

	return size;
}

static const struct file_operations push_fops = {
	.write = push_store,
};

static const struct file_operations pop_fops = {
	.write = pop_store,
};

static int show_open(struct inode *inode, struct file *file)
{
	return single_open(file, val_list_show, inode->i_private);
}

static const struct file_operations show_fops = {
	.open = show_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

static int __init stack_init(void)
{
	stack_dentry = debugfs_create_dir("stack", NULL);

	if (!stack_dentry) {
		pr_err("failed to create /sys/kernel/debug/stack\n");
		return -ENOMEM;
	}

	push_dentry = debugfs_create_file("push", S_IWUGO, stack_dentry, NULL, &push_fops);

	if (!push_dentry) {
		pr_err("failed to create /sys/kernel/debug/stack/push\n");
		return -ENOMEM;
	}

	pop_dentry = debugfs_create_file("pop", S_IWUGO, stack_dentry, NULL, &pop_fops);

	if (!pop_dentry) {
		pr_err("failed to create /sys/kernel/debug/stack/pop\n");
		return -ENOMEM;
	}

	show_dentry = debugfs_create_file("show", S_IRUGO, stack_dentry, NULL, &show_fops);

	if (!show_dentry) {
		pr_err("failed to create /sys/kernel/debug/stack/show\n");
		return -ENOMEM;
	}

	return 0;
}

static void __exit stack_exit(void)
{
	debugfs_remove(show_dentry);
	debugfs_remove(push_dentry);
	debugfs_remove(pop_dentry);
	debugfs_remove(stack_dentry);
	val_list_release();
}

module_init(stack_init);
module_exit(stack_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("chengyihe");
