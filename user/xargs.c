#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

#define MAX_LINE_LENGTH 128

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(2, "Usage: %s command [args...]\n", argv[0]);
        exit(1);
    }

    char buff[128];
    int n = 0;

    // read from the standard input
    while (1)
    {
        char c;
        int read_bytes = read(0, &c, 1);
        if (read_bytes <= 0)
        {
            break;
        }

        if (c == '\n' || n >= MAX_LINE_LENGTH - 1)
        {
            buff[n] = '\0';

            // construct the param array
            char *cmd_args[MAXARG];
            int i;

            for (i = 1; i < argc && i < MAXARG - 1; i++)
            {
                cmd_args[i - 1] = argv[i];
            }
            cmd_args[i - 1] = buff;
            cmd_args[i] = 0;

            if (fork() == 0)
            {
                exec(cmd_args[0], cmd_args);
                fprintf(2, "exec %s failed\n", cmd_args[0]);
                exit(1);
            }
            else
            {
                wait(0);
            }

            // reset n
            n = 0;
        }
        else
        {
            buff[n++] = c;
        }
    }

    exit(0);
}