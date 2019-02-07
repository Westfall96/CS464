/*
 * sh.cpp
 *
 *  Created on: Feb 7, 2019
 *      Author: Austin Westfall
 *      With assistance from https://brennan.io/2015/01/16/write-a-shell-in-c/
 */

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SH_BUFFERSIZE 1024

char *sh_read_line(void)
{
	char *line = NULL;
	ssize_t buffersize = 0;
	getline(&line, &buffersize, stdin);
	return line;
}

#define SH_TOK_BUFFERSIZE 64
#define SH_TOK_DELIMITER "\t\r\n\a"

char **sh_split_line(char *line)
{
	int buffersize = SH_TOK_BUFFERSIZE, position = 0;
	char **tokens = malloc(buffersize * sizeof(char*));
	char *token;

	if(!tokens)
	{
		fprintf(stderr, "sh: buffer not large enough\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, SH_TOK_DELIMITER);
	  while (token != NULL) {
	    tokens[position] = token;
	    position++;

	    if (position >= buffersize) {
	      buffersize += SH_TOK_BUFFERSIZE;
	      tokens = realloc(tokens, buffersize * sizeof(char*));
	      if (!tokens) {
	        fprintf(stderr, "sh: buffer not large enough\n");
	        exit(EXIT_FAILURE);
	      }
	    }

	    token = strtok(NULL, SH_TOK_DELIMITER);
	  }
	  tokens[position] = NULL;
	  return tokens;
}
int sh_open(char **args)
{
	pid_t pid, wpid;
	int stat;

	pid = fork();
	if(pid == 0)
	{
		if (execvp(args[0], args) == -1)
		{
			perror ("sh: failed to execute\n");
		}
		exit(EXIT_FAILURE);
	}
	else if(pid < 0)
	{
		perror("sh: failed to execute\n");
	}
	else
	{
	do
	{
	wpid = waitpid(pid, &stat, WUNTRACED);
	} while(!WIFEXITED(stat) && !WIFSIGNALED(stat));
}
	return 1;
}
int sh_help(char **args);
int sh_exit(char **args);

char *builtin_str[] =
{
	"help",
	"exit"
};

int (*builtin_func[])(char **) =
{
	&sh_help,
	&sh_exit
};

int sh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

int sh_help(char **args)
{
	int i;
	printf("Basic sh(shell)\n");
	printf("Requirements: Basic commands and complete with enter.\n");
	printf("Built in:\n");

	for(i=0;i<sh_num_builtins();i++)
	{
		printf(" %s\n",builtin_str[i]);
	}
	return 1;
}
int sh_exit(char **args)
{
	return 0;
}

int sh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < sh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }

  return sh_open(args);
}

void sh_loop(void)
{
	char *line;
	char **args;
	int status;

	do {
    	printf("in~> ");
    	line = sh_read_line();
    	args = sh_split_line(line);
    	status = sh_execute(args);

    	free(line);
    	free(args);
  } while (status);
}

int main(int argc, char **argv)
{
	sh_loop();

	return EXIT_SUCCESS;
}
