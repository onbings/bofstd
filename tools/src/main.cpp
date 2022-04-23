#include <stdio.h>
#include <bofstd/bofstd.h>

int main(int argc, char* argv[])
{
  printf("Hello world, I'm bofstd_tool version %s\n", BOF_NAMESPACE::Bof_GetVersion().c_str());
  return 0;
}

