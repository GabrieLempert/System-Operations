#ifdef USE_MAP_ANON
#define _BSD_SOURCE /* Get MAP_ANONYMOUS definition */
#endif
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

void parseargs(char *argv[], int argc, int *lval, int *uval, int *nval, int *pval);
int isprime(int n);

int main(int argc, char **argv)
{
  int lval = 1;
  int uval = 100;
  int nval = 10;
  int pval = 4;
  char *flagarr = NULL;
  int num;
  int count = 0;

  // Parse arguments
  parseargs(argv, argc, &lval, &uval, &nval, &pval);
  if (uval < lval)
  {
    fprintf(stderr, "Upper bound should not be smaller then lower bound\n");
    exit(1);
  }
  if (lval < 2)
  {
    lval = 2;
    uval = (uval > 1) ? uval : 1;
  }
  if (pval <= 0)
  {
    fprintf(stderr, "You need to use at least one proccessor\n");
    exit(1);
  }
  if (nval <= 0)
  {
    fprintf(stderr, "You need to use at least one proccessor\n");
    exit(1);
  }
  // Allocate flags
  flagarr = mmap(NULL, (uval - lval + 1), PROT_READ | PROT_WRITE,
                 MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if (flagarr == MAP_FAILED)
    exit(1);
  for (size_t i = 0; i < pval; i++)
  {
    pid_t pId = fork();
    if (pId == 0)
    {
      for (size_t num = lval + i; num <= uval; (num = num + pval))
      {
        if (isprime(num))
        {
          flagarr[num - lval] = 1;
        }
        else
        {
          flagarr[num - lval] = 0;
        }
      }
      exit(EXIT_SUCCESS);
    }
  }

  for (size_t j = 0; j < pval; j++)
  {
    if (wait(NULL) == -1)
      exit(1);
  }
  // Set flagarr
  for (num = lval; num <= uval; num++)
  {
    if (flagarr[num - lval] == 1)
    {
      count++;
    }
  }
  // Print results
  printf("Found %d primes you asked to print %d of them%c\n", count,nval, count ? ':' : '.');
  int temp = nval;
  for (num = lval; num <= uval; num++)
    if (flagarr[num - lval])
    {
      temp--;
      if (temp >= 0)
        printf("%d%c", num, temp <= 0 ? '\n' : ',');
    }

  if (munmap(flagarr, sizeof(char) * (uval - lval + 1)) == -1)
    exit(1);

  return 0;
}

// NOTE : use 'man 3 getopt' to learn about getopt(), opterr, optarg and optopt
void parseargs(char *argv[], int argc, int *lval, int *uval, int *nval, int *pval)
{
  int ch;

  opterr = 0;
  while ((ch = getopt(argc, argv, "l:u:n:p:")) != -1)
    switch (ch)
    {
    case 'l': // Lower bound flag
      *lval = atoi(optarg);
      break;
    case 'u': // Upper bound flag
      *uval = atoi(optarg);
      break;
    case 'n':
      *nval = atoi(optarg);
      break;
    case 'p':
      *pval = atoi(optarg);
      break;
    case '?':
      if ((optopt == 'l') || (optopt == 'u') || (optopt == 'n') || (optopt == 'p'))
        fprintf(stderr, "Option -%c requires an argument.\n", optopt);
      else if (isprint(optopt))
        fprintf(stderr, "Unknown option `-%c'.\n", optopt);
      else
        fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
      exit(1);
    default:
      exit(1);
    }
}

int isprime(int n)
{
  static int *primes = NULL; // NOTE: static !
  static int size = 0;       // NOTE: static !
  static int maxprime;       // NOTE: static !
  int root;
  int i;

  // Init primes array (executed on first call)
  if (primes == NULL)
  {
    primes = (int *)malloc(2 * sizeof(int));
    if (primes == NULL)
      exit(1);
    size = 2;
    primes[0] = 2;
    primes[1] = 3;
    maxprime = 3;
  }

  root = (int)(sqrt(n));

  // Update primes array, if needed
  while (root > maxprime)
    for (i = maxprime + 2;; i += 2)
      if (isprime(i))
      {
        size++;
        primes = (int *)realloc(primes, size * sizeof(int));
        if (primes == NULL)
          exit(1);
        primes[size - 1] = i;
        maxprime = i;
        break;
      }

  // Check 'special' cases
  if (n <= 0)
    return -1;
  if (n == 1)
    return 0;

  // Check prime
  for (i = 0; ((i < size) && (root >= primes[i])); i++)
    if ((n % primes[i]) == 0)
      return 0;
  return 1;
}