#ifdef USE_LANDLOCK
#define _GNU_SOURCE
#endif

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

#ifdef USE_LANDLOCK
#include<linux/landlock.h>
#include<linux/prctl.h>
#include<stddef.h>
#include<sys/prctl.h>
#include<sys/stat.h>
#include<sys/syscall.h>
#include<sys/types.h>
#include<unistd.h>
#include<fcntl.h>
#endif

#ifdef USE_LANDLOCK
// functions to wrap landlock syscalls
int ll_create_ruleset(const struct landlock_ruleset_attr *const attr, const size_t size, const __u32 flags)
{
	return syscall(__NR_landlock_create_ruleset, attr, size, flags);
}

int ll_add_rule(const int ruleset, const enum landlock_rule_type type, const void *const attr, const __u32 flags)
{
    return syscall(__NR_landlock_add_rule, ruleset, type, attr, flags);
}

int ll_restrict(const int ruleset, const __u32 flags)
{
    return syscall(__NR_landlock_restrict_self, ruleset, flags);
}

int apply_sandbox_policy()
{
    int ruleset_fd;

    // general ruleset
    struct landlock_ruleset_attr ruleset_attrs = { .handled_access_fs = LANDLOCK_ACCESS_FS_READ_FILE | LANDLOCK_ACCESS_FS_READ_DIR };
    // ruleset for target directory
    struct landlock_path_beneath_attr paths_beneath = { .allowed_access = LANDLOCK_ACCESS_FS_READ_FILE | LANDLOCK_ACCESS_FS_READ_DIR };

    // create file descriptor that represents ruleset
    ruleset_fd = ll_create_ruleset(&ruleset_attrs, sizeof(ruleset_attrs), 0);

    // data structure that represents ruleset for user directory
    paths_beneath.parent_fd = open(getenv("HOME"), O_PATH | O_CLOEXEC);

    // add rules that restricts process to user home directory
    ll_add_rule(ruleset_fd, LANDLOCK_RULE_PATH_BENEATH, &paths_beneath, 0);
    prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0);

    // restrict this process
    ll_restrict(ruleset_fd, 0);

    close(ruleset_fd);
}
#endif

void usage()
{
    printf("Usage: printfile <FILE>\ne.g.: printfile somefile.txt\n");
}


int do_print_file(const char* path)
{
    FILE *file;
    char ch;

    file = fopen(path, "r");

    if(file == NULL) {
        printf("Failed to open: ");
        switch(errno)
        {
            case ENOENT:
                printf("file does not exist");
            break;
            case EACCES:
                printf("permission denied");
            break;
            default:
                printf("general error");
            break;
        }
        printf("\n");
        return -1;
    }

    printf("\n\n=============================\nPrinting: %s\n=============================\n\n", path);

    while((ch = fgetc(file)) != EOF) {
        printf("%c", ch);  
    }

    fclose(file);
    printf("\n");
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        usage();
        return -1;
    }

#ifdef USE_LANDLOCK
printf("------------  Landlock in use  ------------\n");
    // before going any further, we apply the sandboxing policy
    if(apply_sandbox_policy() < 0) {
        return EXIT_FAILURE;
    }
#endif

    // print all files!
    for(int filec = 1; filec < argc; filec++) 
    {
            do_print_file(argv[filec]);
    }
    return 0;

}