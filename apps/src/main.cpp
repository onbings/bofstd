#include <bofstd/boffs.h>
#include <filesystem>
#include <stdio.h>

#if defined(__EMSCRIPTEN__)
#include <emscripten.h>
#endif

#if defined(__EMSCRIPTEN__)
void game_loop_callback(void)
{
  int i, Len_i;
  FILE *pIo_X;
  char pTxt_c[0x100], pFn_c[] = "/offline/any_file.bha";

  if (!EM_ASM_INT({ return Module.fs_is_ready; }))
  {
    printf("wait\n");
    return;
  }

  pIo_X = fopen(pFn_c, "rb");
  if (pIo_X)
  {
    for (i = 0; i < 10; i++)
    {
      if (fgets(pTxt_c, sizeof(pTxt_c), pIo_X) == nullptr)
      {
        break;
      }
      else
      {
        printf("[%d] '%s'\n", i, pTxt_c);
      }
    }
    fclose(pIo_X);
  }
  else
  {
    pIo_X = fopen(pFn_c, "wb");
    if (pIo_X)
    {
      for (i = 0; i < 10; i++)
      {
        Len_i = sprintf(pTxt_c, "This is line %d\n", i);
        if (fwrite(pTxt_c, Len_i, 1, pIo_X) != 1)
        {
          break;
        }
      }
      fclose(pIo_X);
    }
  }
  // Don't forget to sync to make sure you store it to IndexedDB
  EM_ASM(
      FS.syncfs(
          false, function(err) {if (err) {
            console.error("Error syncing file system:", err);
            return 1; // Indicate failure
        } else {
            console.log("File system synced successfully.");
            return 0; // Indicate success
        } }););

  if (0) // game_is_running())
  {
    // game_run_frame();
  }
  else
  {
    emscripten_cancel_main_loop();
  }
}

BOFERR EmscriptenCallback(void *_pArg)
{
  BOFERR Rts_E = BOF_ERR_NO_ERROR;
  int i, Len_i;
  FILE *pIo_X;
  char pTxt_c[0x100], pFn_c[] = "/offline/any_file.bha";

  const std::filesystem::directory_iterator end;
  for (std::filesystem::directory_iterator it("/"); it != end; ++it)
  {
    const std::filesystem::path &entry = it->path();
    printf("root: %s\n", entry.c_str());
  }

  pIo_X = fopen(pFn_c, "rb");
  if (pIo_X)
  {
    for (i = 0; i < 10; i++)
    {
      if (fgets(pTxt_c, sizeof(pTxt_c), pIo_X) == nullptr)
      {
        break;
      }
      else
      {
        printf("[%d] '%s'\n", i, pTxt_c);
      }
    }
    fclose(pIo_X);
  }
  else
  {
    pIo_X = fopen(pFn_c, "wb");
    if (pIo_X)
    {
      for (i = 0; i < 10; i++)
      {
        Len_i = sprintf(pTxt_c, "This is line %d\n", i);
        if (fwrite(pTxt_c, Len_i, 1, pIo_X) != 1)
        {
          break;
        }
      }
      fclose(pIo_X);
    }
  }
  for (std::filesystem::directory_iterator it("/offline"); it != end; ++it)
  {
    const std::filesystem::path &entry = it->path();
    printf("offline: %s\n", entry.c_str());
  }
  Rts_E = BOF_ERR_FINISHED;
  return Rts_E;
}
#endif

int main(int argc, char *argv[])
{
  BOF::BOFSTDPARAM StdParam_X;
  std::string Cwd_S;

  StdParam_X.AssertInRelease_B = true;
  StdParam_X.AssertCallback = nullptr;
#if defined(__EMSCRIPTEN__)
  StdParam_X.EmscriptenCallback = EmscriptenCallback;
  StdParam_X.EmscriptenCallbackFps_U32 = 0;
  StdParam_X.pEmscriptenCallbackArg = (void *)0x12345678;
  StdParam_X.pPersistentRootDir_c = "/offline";
  StdParam_X.ExitOnBofShutdown_B = true;
#endif
  if (Bof_Initialize(StdParam_X) == BOF_ERR_NO_ERROR)
  {
    BOF::Bof_GetCurrentDirectory(Cwd_S);
    printf("\nPwd %s\nRunning BofStd V %s on %s under %s\n", Cwd_S.c_str(), StdParam_X.Version_S.c_str(), StdParam_X.ComputerName_S.c_str(), StdParam_X.OsName_S.c_str());
    BOF::Bof_Shutdown();
  }
  // emscripten_exit_with_live_runtime();
  return 0;
}
