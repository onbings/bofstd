#include <bofstd/bofstd.h>

#include <stdio.h>

int main(int argc, char* argv[])
{
  printf("Hello world, I'm bofstd_tool version %s\n", BOF::Bof_GetVersion().c_str());
  return 0;
}

