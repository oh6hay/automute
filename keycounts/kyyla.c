#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/keyboard.h>
#include <linux/debugfs.h>
#include <linux/input.h>

/*
 Simple kernel module to provide keyboard event count in a file in debugfs

 Credits to the author of https://github.com/jarun/spy
 I used the "spy" module as a template, removed key logging, added maintaining keyboard event count
 */


#define KEYCOUNTS_VERSION "0.1"

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("osku <osku@helsec.fi>");
MODULE_VERSION(KEYCOUNTS_VERSION);
MODULE_DESCRIPTION("Provide keypress count in debugfs");

static char keycountbuffer[32];
static int keycount;

static struct dentry *keycountfile;

static ssize_t keycountfile_read(struct file *filp,
				 char *buffer,
				 size_t len,
				 loff_t *offset);

static int keycount_cb(struct notifier_block *nblock,
		       unsigned long code,
		       void *_param);

const struct file_operations keycount_fops = {
					      .owner = THIS_MODULE,
					      .read = keycountfile_read,
};

static ssize_t keycountfile_read(struct file *filp,
				 char *buffer,
				 size_t len,
				 loff_t *offset)
{
  snprintf(&keycountbuffer[0], 32, "%d\n", keycount);
  return simple_read_from_buffer(buffer, len, offset, keycountbuffer, strlen(keycountbuffer));
}


static struct notifier_block keycount_blk = {
					     .notifier_call = keycount_cb,
};

int keycount_cb(struct notifier_block *nblock,
		unsigned long code,
		void *_param)
{
  keycount++; // increase key count on every keyboard event. they all make noise

  return NOTIFY_OK;
}

static int __init keycount_init(void)
{
  keycountfile = debugfs_create_file("keypresscount", 0444, NULL, NULL, &keycount_fops);
  if (!keycountfile) {
    return -ENOENT;
  }

  register_keyboard_notifier(&keycount_blk);
  return 0;
}

static void __exit keycount_exit(void)
{
  unregister_keyboard_notifier(&keycount_blk);
}

module_init(keycount_init);
module_exit(keycount_exit);
