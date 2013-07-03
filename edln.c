/* edln - a simple readline-based utility for interactively editing the
 * target of a symbolic link.
 *
 * Author: Jeremy Lin <jjlin@cs.stanford.edu>
 *
 * Placed in the public domain.
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
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

void customize_rl_completion_behavior(void)
{
    /* Don't break on any of the typical word break characters. This would
     * cause filename completion to fail on filenames that contain spaces or
     * other "unusual" characters.
     */
    rl_completer_word_break_characters = "";

    /* Don't append a space character to a fully-completed filename.
     * This would just result in a broken symlink.
     */
    rl_completion_append_character = '\0';
}

void error(const char* msg)
{
    fprintf(stderr, "%s\n", msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv)
{
    char* link_name = NULL;
    char* new_target = NULL;

    switch (argc) {
    case 3:
        new_target = argv[2];
        /* FALL THROUGH */
    case 2:
        link_name = argv[1];
        break;
    default:
        fprintf(stderr, "usage: %s symlink_to_edit [new_target]\n", argv[0]);
        return EXIT_SUCCESS;
    }

    /* If there is a trailing slash in the symlink path, remove it.
     *
     * When performing filename completion on a symlink, shells often add a
     * trailing slash when the symlink points to a directory. However, this
     * trailing slash will cause readlink() to fail.
     */
    {
        char* last_char = link_name + strlen(link_name) - 1;
        if (last_char > link_name && *last_char == '/') {
            *last_char = '\0';
        }
    }

    /* Read the original symlink target. */
    if (readlink(link_name, buf, sizeof(buf)) < 0)
        goto error;

    if (new_target == NULL) {
        customize_rl_completion_behavior();
        rl_extend_line_buffer(BUFLEN+1);
        rl_pre_input_hook = &init_rl_line_buffer;

        if ( (new_target = readline("New target: ")) == NULL)
            error("No input received, aborting.");

        if (!strcmp(new_target, buf)) {
            /* new target same as old target, so don't make any changes */
            return EXIT_SUCCESS;
        } else if (*new_target == '\0') {
            /* empty target */
            error("Can't symlink to empty filename.");
        }
    }

    /* Remove the original symlink. */
    if (unlink(link_name) < 0)
        goto error;

    /* Write out the updated symlink. */
    if (symlink(new_target, link_name) < 0)
        goto error;

    return EXIT_SUCCESS;

 error:
    perror(argv[0]);
    return EXIT_FAILURE;    
}
