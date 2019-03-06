#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
#include <argp.h>
#include <sys/resource.h>

char commandline[]= "NowcastToWDSS2 [input_filename] [output_directory]";
int main(int argc, char* argv[])
{
  const rlim_t kStackSize = 64L * 1024L * 1024L;   // min stack size = 64 Mb
  struct rlimit rl;
  int result;

  result = getrlimit(RLIMIT_STACK, &rl);
  if (result == 0)
    {
      if (rl.rlim_cur < kStackSize)
        {
	  rl.rlim_cur = kStackSize;
	  result = setrlimit(RLIMIT_STACK, &rl);
	  if (result != 0)
            {
	      fprintf(stderr, "setrlimit returned result = %d\n", result);
            }
        }
    }
  
  char *i_filename, *o_directory;
  if(argc != 3) {
    printf("Usage: %s\n", commandline);
    exit(-1);
  }

  i_filename = argv[1];
  o_directory = argv[2];

  /*Function Declaration */
  void NowcastToWDSS2(char i_filename[], char o_directory[]);
  
  /* Function call */
  NowcastToWDSS2(i_filename, o_directory);

  return 0;
}






