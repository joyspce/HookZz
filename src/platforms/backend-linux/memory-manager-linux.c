#include "memory-manager-linux.h"
#include "memory-helper-posix.h"

PLATFORM_API static bool memory_manager_cclass(is_support_allocate_rx_memory)(memory_manager_t *self) { return true; }

PLATFORM_API static int memory_manager_cclass(get_page_size)() { return 0x4000; }

PLATFORM_API void *memory_manager_cclass(allocate_page)(memory_manager_t *self, int prot, int n) {
    int page_size = posix_memory_helper_class(get_page_size)();

    void *mmap_page = mmap(0, 1, PROT_WRITE | PROT_READ, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    mprotect(mmap_page, (size_t)page_size, (PROT_READ | PROT_WRITE));
    return mmap_page;
}

PLATFORM_API void memory_manager_cclass(patch_code)(memory_manager_t *self, void *dest, void *src, int count) {
    posix_memory_helper_cclass(patch_code)(dest, src, count);
    return;
}

PLATFORM_API void memory_manager_cclass(get_process_memory_layout)(memory_manager_t *self) {
    char filename[64];
    char buf[256];
    FILE *fp;

    // given pid, open /proc/pid/maps; or not, open current maps.
    if (pid > 0) {
        sprintf(filename, "/proc/%d/maps", pid);
    } else {
        sprintf(filename, "/proc/self/maps");
    }

    fp = fopen(filename, "r");
    if (fp < 0) {
        return NULL;
    }

    while (fgets(buf, sizeof(buf), fp) != NULL) {
        zz_addr_t start, end;
        unsigned dev, sdev;
        unsigned long inode;
        unsigned long long offset;
        char prot[5];
        char path[64];
        int len;

        /* format in /proc/pid/maps is constructed as below in fs/proc/task_mmu.c
        167	seq_printf(m,
        168			   "%08lx-%08lx %c%c%c%c %08llx %02x:%02x %lu ",
        169			   vma->vm_start,
        170			   vma->vm_end,
        171			   flags & VM_READ ? 'r' : '-',
        172			   flags & VM_WRITE ? 'w' : '-',
        173			   flags & VM_EXEC ? 'x' : '-',
        174			   flags & VM_MAYSHARE ? flags & VM_SHARED ? 'S' : 's' : 'p',
        175			   pgoff,
        176			   MAJOR(dev), MINOR(dev), ino);
        177
        178		if (file) {
        179			seq_pad(m, ' ');
        180			seq_file_path(m, file, "");
        181		} else if (mm && is_stack(priv, vma)) {
        182			seq_pad(m, ' ');
        183			seq_printf(m, "[stack]");
        184		}
         */
        if (sscanf(buf, "%lx-%lx %s %llx %x:%x %lu %s", &start, &end, prot, &offset, &dev, &sdev, &inode, path) != 8)
            continue;

        MemoryBlock *mb = SAFE_MALLOC_TYPE(MemoryBlock);
        list_rpush(self->process_memory_layout, list_node_new(mb));
        tmp_addr += tmp_size;
        mb->address = start_addr;
        mb->size    = end_addr - start_addr;
        mb->prot = (prot[0] == 'r' ? (1 << 0) : 0) | (prot[1] == 'w' ? (1 << 1) : 0) | (prot[2] == 'x' ? (1 << 2) : 0);
    }
}
