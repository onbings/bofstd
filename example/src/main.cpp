#include <stdio.h>
#include <bofstd/bofstd.h>

int main(int argc, char* argv[])
{
  printf("Hello world, I'm bofstd_example version %s\n", BOF::Bof_GetVersion().c_str());
  return 0;
}

