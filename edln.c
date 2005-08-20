/* edln - a simple readline-based utility for interactively editing
 *        the target of a symbolic link.
 *
 * Written by Jeremy Lin <jjlin@OCF.Berkeley.EDU>.
 * Placed in the public domain.
 *
 * Revision history:
 * 
 * 12/30/2002: Initial revision.
 * 05/01/2004: Added check for empty symlink name.
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <readline/readline.h>

#define BUFLEN 512

char buf[BUFLEN];

int init_rl_line_buffer(void)
{
    rl_replace_line(buf, 0);
    rl_point = rl_end; /* move cursor to end of line */
    rl_redisplay();
    return 0;
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

    /* read original target of symlink */
    if (readlink(link_name, buf, sizeof(buf)) < 0)
	goto error;

    if (new_target == NULL) {
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

    /* remove original symlink */
    if (unlink(link_name) < 0)
	goto error;

    /* write out new symlink */
    if (symlink(new_target, link_name) < 0)
	goto error;

    return EXIT_SUCCESS;

 error:
    perror(argv[0]);
    return EXIT_FAILURE;    
}
