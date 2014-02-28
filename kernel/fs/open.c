/*
 *  FILE: open.c
 *  AUTH: mcc | jal
 *  DESC:
 *  DATE: Mon Apr  6 19:27:49 1998
 */

#include "globals.h"
#include "errno.h"
#include "fs/fcntl.h"
#include "util/string.h"
#include "util/printf.h"
#include "fs/vfs.h"
#include "fs/vnode.h"
#include "fs/file.h"
#include "fs/vfs_syscall.h"
#include "fs/open.h"
#include "fs/stat.h"
#include "util/debug.h"

/* find empty index in p->p_files[] */
int
get_empty_fd(proc_t *p)
{
        int fd;

        for (fd = 0; fd < NFILES; fd++) {
                if (!p->p_files[fd])
                        return fd;
        }

        dbg(DBG_ERROR | DBG_VFS, "ERROR: get_empty_fd: out of file descriptors "
            "for pid %d\n", curproc->p_pid);
        return -EMFILE;
}

/*
 * There a number of steps to opening a file:
 *      1. Get the next empty file descriptor.
 *      2. Call fget to get a fresh file_t.
 *      3. Save the file_t in curproc's file descriptor table.
 *      4. Set file_t->f_mode to OR of FMODE_(READ|WRITE|APPEND) based on
 *         oflags, which can be O_RDONLY, O_WRONLY or O_RDWR, possibly OR'd with
 *         O_APEND.
 *      5. Use open_namev() to get the vnode for the file_t.
 *      6. Fill in the fields of the file_t.
 *      7. Return new fd.
 *
 * If anything goes wrong at any point (specifically if the call to open_namev
 * fails), be sure to remove the fd from curproc, fput the file_t and return an
 * error.
 *
 * Error cases you must handle for this function at the VFS level:
 *      o EINVAL (1)
 *        oflags is not valid.
 *      o EMFILE (2)
 *        The process already has the maximum number of files open.
 *      o ENOMEM (3)
 *        Insufficient kernel memory was available.
 *      o ENAMETOOLONG (4)
 *        A component of filename was too long.
 *      o ENOENT (5)
 *        O_CREAT is not set and the named file does not exist.  Or, a
 *        directory component in pathname does not exist.
 *      o EISDIR (6)
 *        pathname refers to a directory and the access requested involved
 *        writing (that is, O_WRONLY or O_RDWR is set).
 *      o ENXIO (7)
 *        pathname refers to a device special file and no corresponding device
 *        exists.
 */

int
do_open(const char *filename, int oflags)
{
    /* step 1: get next empty file descriptor */
    int fd = get_empty_fd(curproc);

    /* error case 2 */
    if (fd == -EMFILE){
        return -EMFILE;
    }

    /* step 2: Call fget to get a fresh file_t */
    file_t *f = fget(-1);

    /* error case 3 */
    if (f == NULL){
        fput(f);
        return -ENOMEM;
    }

    KASSERT(f != NULL);
    KASSERT(f->f_refcount == 1);

    /* step 3: Save file_t in curproc's file descriptor table */
    KASSERT(curproc->p_files[fd] == NULL);
    curproc->p_files[fd] = f;

    /* step 4: Set the file_t->f-mode */
    f->f_mode = 0;

    if (oflags & O_RDONLY){
        f->f_mode = FMODE_READ;
    } else if (oflags & O_WRONLY){
        f->f_mode = FMODE_WRITE;
    } else if (oflags & O_RDWR){
        f->f_mode = FMODE_READ | FMODE_WRITE;
    }

    if (oflags & O_APPEND){
        f->f_mode |= FMODE_APPEND;
    }

    /* make sure we have a valid mode */
    KASSERT(f->f_mode == FMODE_READ
            || f->f_mode == FMODE_WRITE
            || f->f_mode == (FMODE_READ | FMODE_WRITE)
            || f->f_mode == (FMODE_WRITE | FMODE_APPEND)
            || f->f_mode == (FMODE_READ | FMODE_WRITE | FMODE_APPEND));

    /* step 5: use open_namev() to get the vnode for the file_t */

        NOT_YET_IMPLEMENTED("VFS: do_open");

        panic("not yet implemented\n");
        return -1;
}
