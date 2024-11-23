#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find_files(const char *dir_path, const char *file_name)
{
    char buf[512];
    char *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(dir_path, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", dir_path);
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", dir_path);
        close(fd);
        return;
    }

    if (st.type != T_DIR)
    {
        close(fd);
        return;
    }

    strcpy(buf, dir_path);
    p = buf + strlen(buf);
    *p++ = '/';
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        {
            continue;
        }
        memmove(p, de.name, DIRSIZ);
        p[DIRSIZ] = 0;
        if (stat(buf, &st) < 0)
        {
            printf("find: cannot stat %s\n", buf);
            continue;
        }

        if (st.type == T_DIR)
        {
            // Recursively search in the subdirectory
            find_files(buf, file_name);
        }
        else if (st.type == T_FILE)
        {
            // Check if the file name matches
            if (strcmp(de.name, file_name) == 0)
            {
                printf("%s\n", buf);
            }
        }
    }
}
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(2, "Usage: %s <directory> <filename>\n", argv[0]);
        exit(1);
    }

    const char *dir_path = argv[1];
    const char *file_name = argv[2];

    // Start the search
    find_files(dir_path, file_name);

    exit(0);
}