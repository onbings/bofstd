#include <bofstd/boffs.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
  BOF::BOFSTDPARAM StdParam_X;
  std::string Cwd_S;

  StdParam_X.AssertInRelease_B = true;
  StdParam_X.AssertCallback = nullptr;
  if (Bof_Initialize(StdParam_X) == BOF_ERR_NO_ERROR)
  {
    BOF::Bof_GetCurrentDirectory(Cwd_S);
    printf("\nPwd %s\nRunning BofStd V %s on %s under %s\n", Cwd_S.c_str(), StdParam_X.Version_S.c_str(), StdParam_X.ComputerName_S.c_str(), StdParam_X.OsName_S.c_str());
    BOF::Bof_Shutdown();
  }
  return 0;
}
