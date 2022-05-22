#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define LINELEN (80)

char *mygets(char *buf, int len);
int mygeti();

int main(int argc, char *argv[])
{
	char *cmpstr[] = {"lexcmp", "lencmp"};
	char str1[LINELEN + 1];
	char str2[LINELEN + 1];
	int index;
	int veclen = sizeof(cmpstr) / sizeof(char *);
	int writeLex[2], readLex[2], writeLen[2], readLen[2];
	int lexID, lenID;
	if (pipe(writeLex) == -1 || pipe(readLex) == -1 || pipe(writeLen) == -1 || pipe(readLen) == -1)
		return -2;

	if ((lexID = fork()) == -1)
		return -2;

	if (lexID == 0)
	{
		if (dup2(writeLex[0], STDIN_FILENO) == -1)
			return -2;
		if (close(writeLex[0]) != 0 || close(writeLex[1]) != 0)
			return -2;
		if (dup2(readLex[1], STDOUT_FILENO) == -1)
			return -2;
		if (close(readLex[0]) != 0 || close(readLex[1]) != 0)
			return -2;
		char *argsForLex[] = {"loopcmp", "lexcmp", NULL};
		if (execvp("./loopcmp", argsForLex) == -1)
			return -2;
	}
	if (close(writeLex[0]) != 0)
		return -2;
	if (close(readLex[1]) != 0)
		return -2;

	// setup lencmp child

	if ((lenID = fork()) == -1)
		return -2;
	if (lenID == 0)
	{
		if (close(writeLex[1]) != 0 || close(readLex[0]) != 0)
			return -2;
		if (dup2(writeLen[0], STDIN_FILENO) == -1)
			return -2;
		if (close(writeLen[0]) != 0 || close(writeLen[1]) != 0)
			return -2;

		if (dup2(readLen[1], STDOUT_FILENO) == -1)
			return -2;
		if (close(readLen[0]) != 0 || close(readLen[1]) != 0)
			return -2;
		char *argsForLen[] = {"loopcmp", "lencmp", NULL};

		if (execvp("./loopcmp", argsForLen) == -1)
			return -2;
	}

	if (close(writeLen[0]) != 0 || close(readLen[1]) != 0)
		return -2;
	while (1)
	{
		printf("Please enter first string:\n");

		if (mygets(str1, LINELEN) == NULL)
			break;

		printf("Please enter second string:\n");

		if (mygets(str2, LINELEN) == NULL)
			break;

		do
		{
			printf("Please choose:\n");
			for (int i = 0; i < veclen; i++)
				printf("%d - %s\n", i, cmpstr[i]);
			index = mygeti();
		} while ((index < 0) || (index >= veclen));

		char result[2];

		if (index == 0)
		{
			
			if (write(writeLex[1], str1, strlen(str1)) != strlen(str1))
				return -2;
			if (write(writeLex[1], "\n", 1) != 1)
				return -2;
			if (write(writeLex[1], str2, strlen(str2)) != strlen(str2))
				return -2;
			if (write(writeLex[1], "\n", 1) != 1)
				return -2;

			if (read(readLex[0], result, 2) != 2)
				return -2;
		}
		if (index == 1)
		{
			if (write(writeLen[1], str1, strlen(str1)) != strlen(str1))
				return -2;
			if (write(writeLen[1], "\n", 1) != 1)
				return -2;
			if (write(writeLen[1], str2, strlen(str2)) != strlen(str2))
				return -2;
			if (write(writeLen[1], "\n", 1) != 1)
				return -2;

			if (read(readLen[0], result, 2) != 2)
				return -2;
		}

		printf("%s(%s, %s) == %c\n", cmpstr[index], str1, str2, result[0]);
		fflush(stdout);
	}

	// close lexcmp

	if (close(writeLex[1]) != 0)
		return -2;
	if (close(readLex[0]) != 0)
		return -2;

	int wstatus;
	if (waitpid(lexID, &wstatus, 0) == -1)
		return -2;

	if (!WIFEXITED(wstatus))
		return -2;

	int child_exit_status = WEXITSTATUS(wstatus);

	// close lencmp

	if (close(writeLen[1]) != 0)
		return -2;
	if (close(readLen[0]) != 0)
		return -2;

	if (waitpid(lenID, &wstatus, 0) == -1)
		return -2;

	if (!WIFEXITED(wstatus))
		return -2;

	child_exit_status = WEXITSTATUS(wstatus);

	return 0;
}

char *mygets(char *buf, int len)
{
	char *retval;

	retval = fgets(buf, len, stdin);
	buf[len] = '\0';
	if (buf[strlen(buf) - 1] == 10) /* trim \r */
		buf[strlen(buf) - 1] = '\0';
	else if (retval)
		while (getchar() != '\n')
			; /* get to eol */

	return retval;
}

int mygeti()
{
	int ch;
	int retval = 0;

	while (isspace(ch = getchar()))
		;
	while (isdigit(ch))
	{
		retval = retval * 10 + ch - '0';
		ch = getchar();
	}
	while (ch != '\n')
		ch = getchar();
	return retval;
}
