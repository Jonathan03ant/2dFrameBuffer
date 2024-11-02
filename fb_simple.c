#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fb.h>
#include <linux/vmalloc.h>
#include <linux/uaccess.h>
#include <linux/mman.h>
#include <linux/fs.h>

#define FB_WIDTH  1024    // Set width of the framebuffer
#define FB_HEIGHT 768     // Set height of the framebuffer
#define FB_BPP    32      // 32 bits per pixel (RGBA)

struct fb_info *fb_info;       // Framebuffer information structure
void *fb_memory;               // Framebuffer memory pointer
unsigned long fb_size;         // Framebuffer memory size

// File operations: open, mmap, and release (close)
static int fb_open(struct inode *inode, struct file *file) {
    return 0;
}

static int fb_release(struct inode *inode, struct file *file) {
    return 0;
}

// Memory mapping for user space access
static int fb_mmap(struct file *file, struct vm_area_struct *vma) {
    int ret;
    // Map framebuffer memory to user space
    ret = remap_vmalloc_range(vma, fb_memory, vma->vm_pgoff);
    if (ret) {
        pr_err("Failed to mmap framebuffer memory\n");
        return ret;
    }
    return 0;
}

// Define the file operations
static struct file_operations fb_fops = {
    .owner = THIS_MODULE,
    .open = fb_open,
    .mmap = fb_mmap,
    .release = fb_release,
};

// Framebuffer initialization
static int __init fb_init(void) {
    // Allocate the fb_info structure
    fb_info = framebuffer_alloc(0, NULL);
    if (!fb_info) {
        pr_err("Failed to allocate framebuffer info\n");
        return -ENOMEM;
    }

    // Set up basic framebuffer information (resolution, color depth)
    fb_info->var.xres = FB_WIDTH;
    fb_info->var.yres = FB_HEIGHT;
    fb_info->var.bits_per_pixel = FB_BPP;
    
    // Calculate the framebuffer size
    fb_size = FB_WIDTH * FB_HEIGHT * (FB_BPP / 8);

    // Allocate memory for the framebuffer
    fb_memory = vmalloc(fb_size);
    if (!fb_memory) {
        pr_err("Failed to allocate framebuffer memory\n");
        framebuffer_release(fb_info);
        return -ENOMEM;
    }

    // Initialize the framebuffer memory (optional: clear it to a color)
    memset(fb_memory, 0, fb_size);

    // Register framebuffer device
    if (register_framebuffer(fb_info) < 0) {
        pr_err("Failed to register framebuffer\n");
        vfree(fb_memory);
        framebuffer_release(fb_info);
        return -EINVAL;
    }

    pr_info("Framebuffer driver loaded successfully\n");
    return 0;
}

// Framebuffer cleanup
static void __exit fb_exit(void) {
    unregister_framebuffer(fb_info);  // Unregister framebuffer
    vfree(fb_memory);                 // Free framebuffer memory
    framebuffer_release(fb_info);     // Release fb_info
    pr_info("Framebuffer driver unloaded\n");
}

// Register init and exit functions
module_init(fb_init);
module_exit(fb_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Simple 2D Framebuffer Driver");

