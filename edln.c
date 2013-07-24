/*
** edln - a simple readline-based utility for interactively editing the
** target of a symbolic link.
**
** Author: Jeremy Lin <jjlin@cs.stanford.edu>
**
** Placed in the public domain.
*/

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <readline/readline.h>

#define BUFLEN 4096

char buf[BUFLEN];

int init_rl_line_buffer(void)
{
    rl_replace_line(buf, 0);
    rl_point = rl_end; /* Move cursor to end of line. */
    rl_redisplay();
    return 0;
}

char* edln_rl_filename_completion_function(const char* text, int state)
{
    /*
    ** N.B. There are two ways to make this space-suppression customization.
    ** The original method is to set 'rl_completion_append_character' to the
    ** NUL character. The alternate method, which was introduced in Readline
    ** 4.3, is to set 'rl_completion_suppress_append' to a nonzero value.
    ** Digging through the Readline source code reveals that, surprisingly,
    ** 'rl_completion_suppress_append' has always been completely redundant.
    ** There was a time when it seemed to have been intended to also suppress
    ** appending a '/' character to directories, but this was never actually
    ** put into effect.
    **
    ** In any case, Readline resets both variables to their default values
    ** before every call to the completion function, hence the need for this
    ** wrapper function, which sets one of them back to our desired value.
    */
    rl_completion_append_character = '\0';

    /* Call out to Readline's built-in filename completion function. */
    return rl_filename_completion_function(text, state);
}

void customize_rl_completion_behavior(void)
{
    /*
    ** Don't break on any of the typical word break characters. This would
    ** cause filename completion to fail on filenames that contain spaces or
    ** other "unusual" characters.
    */
    rl_completer_word_break_characters = "";

    /*
    ** Don't append a space character to a fully-completed filename.
    ** This would just result in a broken symlink.
    **
    ** This customization requires use of a custom completion function.
    */
    rl_completion_entry_function = &edln_rl_filename_completion_function;
}

/*
** Change to the directory containing the symlink, so that filename
** completions are performed relative to that directory.
*/
void change_to_symlink_dir(char* link_path)
{
    /* Find the last slash in the symlink path, if any. */
    int i;
    for (i = strlen(link_path) - 1; i > 0; --i) {
        if (link_path[i] == '/') {
            link_path[i] = '\0'; /* Temporarily truncate at the slash. */
            if (chdir(link_path) < 0) {
                fprintf(stderr, "Warning: Couldn't chdir() to '%s': ", link_path);
                perror(NULL);
            }
            link_path[i] = '/'; /* Restore the slash. */
            break;
        }
    }
}

void fatal_error(const char* fmt, ...)
{
    char msg[BUFLEN];
    va_list ap;

    if (fmt) {
        /* Format and print the error message. */
        va_start(ap, fmt);
        vsnprintf(msg, sizeof(msg), fmt, ap);
        va_end(ap);
        fprintf(stderr, "%s\n", msg);
    } else {
        perror(NULL);
    }
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
    char* link_path = NULL;
    char* new_target = NULL;

    switch (argc) {
    case 3:
        new_target = argv[2];
        /* FALL THROUGH */
    case 2:
        link_path = argv[1];
        break;
    default:
        fprintf(stderr, "usage: %s symlink_to_edit [new_target]\n", argv[0]);
        return EXIT_SUCCESS;
    }

    /*
    ** If there is a trailing slash in the symlink path, remove it.
    **
    ** When performing filename completion on a symlink, shells often add a
    ** trailing slash when the symlink points to a directory. However, this
    ** trailing slash will cause readlink() to fail.
    */
    {
        char* last_char = link_path + strlen(link_path) - 1;
        if (last_char > link_path && *last_char == '/') {
            *last_char = '\0';
        }
    }

    /* Read the original symlink target. 'buf' is already zero-filled. */
    if (readlink(link_path, buf, sizeof(buf)-1) < 0) {
        /* Print less-cryptic messages for the most common errors. */
        switch (errno) {
        case ENOENT:
            fatal_error("'%s' does not exist.", link_path);
            break;
        case EINVAL:
            fatal_error("'%s' is not a symlink.", link_path);
            break;
        default:
            fatal_error(NULL); /* Behave like perror(). */
            break;
        }
    }

    if (new_target == NULL) {
        change_to_symlink_dir(link_path);
        customize_rl_completion_behavior();
        rl_extend_line_buffer(BUFLEN+1);
        rl_pre_input_hook = &init_rl_line_buffer;

        new_target = readline("New target: ");
        if (new_target == NULL)
            fatal_error("No input received, aborting.");

        if (!strcmp(new_target, buf)) {
            /* New target same as old target, so don't make any changes. */
            exit(EXIT_SUCCESS);
        } else if (*new_target == '\0') {
            fatal_error("Can't symlink to an empty target.");
        }
    }

    /* Remove the original symlink. */
    if (unlink(link_path) < 0)
        fatal_error(NULL);

    /* Write out the updated symlink. */
    if (symlink(new_target, link_path) < 0)
        fatal_error(NULL);

    return EXIT_SUCCESS;
}
