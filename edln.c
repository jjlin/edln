/* edln - a simple readline-based program that lets you edit the target of
 *        a symbolic link.
 *
 * Written by Jeremy Lin <jjlin@OCF.Berkeley.EDU>, Dec. 2002.
 * Placed in the public domain.
 */

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <readline/readline.h>

#define BUFLEN 512

char buf[BUFLEN];

int init_rl_line_buffer (void)
{
    rl_replace_line(buf, 0);
    rl_point = rl_end; /* move cursor to end of line */
    rl_redisplay();
    return 0;
}

int main (int argc, char** argv)
{
    char* link_name = NULL;
    char* new_target = NULL;
    char* line = NULL;

    switch (argc) {
    case 3:
	new_target = argv[2];
	/* fall through */
    case 2:
	link_name = argv[1];
	break;
    default:
	fprintf(stderr, "usage: %s symlink_to_edit [new_target]\n", argv[0]);
	return EXIT_SUCCESS;
    }

    /* read original target of symlink */
    if (readlink(link_name, buf, sizeof(buf)) == -1)
	goto error;

    if (new_target == NULL) {
	rl_extend_line_buffer(BUFLEN+1);
	rl_pre_input_hook = &init_rl_line_buffer;
	if ( (line = readline("New target: ")) == NULL) {
	    fprintf(stderr, "No input received, aborting.\n");
	    return EXIT_FAILURE;
	}
	/* new target == old target */
	if (!strcmp(line, buf)) return EXIT_SUCCESS;
	new_target = line;
    }

    /* remove original symlink */
    if (unlink(link_name) == -1)
	goto error;

    /* write out new symlink */
    if (symlink(new_target, link_name) == -1)
	goto error;

    return EXIT_SUCCESS;

 error:
    perror(argv[0]);
    return EXIT_FAILURE;    
}
