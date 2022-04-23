#include <stdio.h>
#include <bofstd/bofstd.h>

int main(int argc, char* argv[])
{
  printf("Hello world, I'm bofstd version %s\n", Bof_GetVersion().c_str());
  return 0
}

