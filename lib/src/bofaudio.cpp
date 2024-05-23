/*
 * Copyright (c) 2024-2044, EVS Broadcast Equipment S.A. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements different Audio interface
 *
 * Author:      Bernard HARMEL: b.harmel@evs.com
 *
 * History:
 * V 1.00  Apr 11 2024  BHA : Initial release
 */
#include "bofstd/bofaudio.h"

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

/*From doc
2.6. Emscripten
---------------
The Emscripten build emits Web Audio JavaScript directly and should compile cleanly out of the box.
You cannot use `-std=c*` compiler flags, nor `-ansi`.

You can enable the use of AudioWorkets by defining `MA_ENABLE_AUDIO_WORKLETS` and then compiling
with the following options:

    -sAUDIO_WORKLET=1 -sWASM_WORKERS=1 -sASYNCIFY

An example for compiling with AudioWorklet support might look like this:

    emcc program.c -o bin/program.html -DMA_ENABLE_AUDIO_WORKLETS -sAUDIO_WORKLET=1 -sWASM_WORKERS=1 -sASYNCIFY
*/

BEGIN_BOF_NAMESPACE()

void Cb_OnNeedAudioData(ma_device *_pDevice_X, void *_pOutput, const void *_pInput, ma_uint32 _FrameCount_U32)
{
  BOF_AUDIO_CB_PARAM *pAudioCbParam_X = reinterpret_cast<BOF_AUDIO_CB_PARAM *>(_pDevice_X->pUserData);
  if ((pAudioCbParam_X) && (pAudioCbParam_X->pAudioDevice))
  {
    pAudioCbParam_X->OnNeedAudioData(*pAudioCbParam_X, _pDevice_X, _pOutput, _pInput, _FrameCount_U32);
  }
}

BofAudioDevice::BofAudioDevice(const BOF_AUDIO_DEVICE_PARAM &_rAudioDeviceParam_X)
{
  mAudioDeviceParam_X = _rAudioDeviceParam_X;
  if (ma_context_init(nullptr, 0, nullptr, &mMaContext_X) == MA_SUCCESS)
  {
    ScanForDevice();
  }
}
BofAudioDevice::~BofAudioDevice()
{
  ma_context_uninit(&mMaContext_X);
}

uint32_t BofAudioDevice::ScanForDevice()
{
  uint32_t Rts_U32, i_U32, j_U32;
  BOF_AUDIO_DEVICE_INFO AudioDeviceInfo_X;
  ma_device_info *pPlaybackInfo_X;
  ma_uint32 PlaybackCount_U32;
  ma_device_info *pCaptureInfo_X;
  ma_uint32 CaptureCount_U32;
  BOF_AUDIO_DEVICE_NATIVE_FORMAT AudioDeviceNativeFormat_X;

  Rts_U32 = 0;
  mpAudioEngine_X[BOF_AUDIO_ENGINE_ID_PLAYBACK].Index_S32 = -1;
  mpAudioEngine_X[BOF_AUDIO_ENGINE_ID_PLAYBACK].AudioDeviceInfoCol.clear();
  mpAudioEngine_X[BOF_AUDIO_ENGINE_ID_CAPTURE].Index_S32 = -1;
  mpAudioEngine_X[BOF_AUDIO_ENGINE_ID_CAPTURE].AudioDeviceInfoCol.clear();
  if (ma_context_get_devices(&mMaContext_X, &pPlaybackInfo_X, &PlaybackCount_U32, &pCaptureInfo_X, &CaptureCount_U32) == MA_SUCCESS)
  {
    Rts_U32 += (PlaybackCount_U32+ CaptureCount_U32);
    // Loop over each device info and do something with it. Here we just print the name with their index. You may want
    // to give the user the opportunity to choose which device they'd prefer.
    for (i_U32 = 0; i_U32 < PlaybackCount_U32; i_U32++)
    {
      memcpy(AudioDeviceInfo_X.pId_c, pPlaybackInfo_X[i_U32].id.custom.s, 256);
      AudioDeviceInfo_X.Name_S = pPlaybackInfo_X[i_U32].name;
      AudioDeviceInfo_X.IsDefault_B = pPlaybackInfo_X[i_U32].isDefault;
      for (j_U32 = 0; j_U32 < pPlaybackInfo_X[i_U32].nativeDataFormatCount; j_U32++)
      {
        AudioDeviceNativeFormat_X.Format_E = static_cast<BOF_AUDIO_FORMAT>(pPlaybackInfo_X[i_U32].nativeDataFormats[j_U32].format);
        AudioDeviceNativeFormat_X.NbChannel_U32 = pPlaybackInfo_X[i_U32].nativeDataFormats[j_U32].channels;
        AudioDeviceNativeFormat_X.SampleRate_32 = pPlaybackInfo_X[i_U32].nativeDataFormats[j_U32].sampleRate;
        AudioDeviceNativeFormat_X.Flag_U32 = pPlaybackInfo_X[i_U32].nativeDataFormats[j_U32].flags;
        AudioDeviceInfo_X.NativeFormatCol.push_back(AudioDeviceNativeFormat_X);
      }
      mpAudioEngine_X[BOF_AUDIO_ENGINE_ID_PLAYBACK].AudioDeviceInfoCol.push_back(AudioDeviceInfo_X);
      printf("out %d - %s\n", i_U32, AudioDeviceInfo_X.Name_S.c_str());
    }

    for (i_U32 = 0; i_U32 < CaptureCount_U32; i_U32++)
    {
      memcpy(AudioDeviceInfo_X.pId_c, pCaptureInfo_X[i_U32].id.custom.s, 256);
      AudioDeviceInfo_X.Name_S = pCaptureInfo_X[i_U32].name;
      AudioDeviceInfo_X.IsDefault_B = pCaptureInfo_X[i_U32].isDefault;
      for (j_U32 = 0; j_U32 < pCaptureInfo_X[i_U32].nativeDataFormatCount; j_U32++)
      {
        AudioDeviceNativeFormat_X.Format_E = static_cast<BOF_AUDIO_FORMAT>(pCaptureInfo_X[i_U32].nativeDataFormats[j_U32].format);
        AudioDeviceNativeFormat_X.NbChannel_U32 = pCaptureInfo_X[i_U32].nativeDataFormats[j_U32].channels;
        AudioDeviceNativeFormat_X.SampleRate_32 = pCaptureInfo_X[i_U32].nativeDataFormats[j_U32].sampleRate;
        AudioDeviceNativeFormat_X.Flag_U32 = pCaptureInfo_X[i_U32].nativeDataFormats[j_U32].flags;
        AudioDeviceInfo_X.NativeFormatCol.push_back(AudioDeviceNativeFormat_X);
      }
      mpAudioEngine_X[BOF_AUDIO_ENGINE_ID_PLAYBACK].AudioDeviceInfoCol.push_back(AudioDeviceInfo_X);
      printf("out %d - %s\n", i_U32, AudioDeviceInfo_X.Name_S.c_str());
    }
  }
  return Rts_U32;
}

std::vector<BOF_AUDIO_DEVICE_INFO> BofAudioDevice::GetDeviceList(BOF_AUDIO_ENGINE_ID _AudioEngineId_E)
{
  std::vector<BOF_AUDIO_DEVICE_INFO> Rts;
  if (_AudioEngineId_E < BOF_AUDIO_ENGINE_ID_MAX)
  {
    Rts = mpAudioEngine_X[_AudioEngineId_E].AudioDeviceInfoCol;
  }
  return Rts;
}

bool BofAudioDevice::InitEngineDevice(BOF_AUDIO_ENGINE_ID _AudioEngineId_E, const std::string &_rName_S, uint32_t _NbChannel_U32, uint32_t _SampleRate_U32, BOF_AUDIO_FORMAT _AudioFormat_E)
{
  bool Rts_B = false;
  uint32_t i_U32;

  if (_AudioEngineId_E < BOF_AUDIO_ENGINE_ID_MAX)
  {
    if (mpAudioEngine_X[_AudioEngineId_E].Index_S32 < 0)
    {
      for (i_U32 = 0; i_U32 < mpAudioEngine_X[_AudioEngineId_E].AudioDeviceInfoCol.size(); i_U32++)
      {
        if (_rName_S == mpAudioEngine_X[_AudioEngineId_E].AudioDeviceInfoCol[i_U32].Name_S)
        {
          mpAudioEngine_X[_AudioEngineId_E].AudioCbParam_X.NbChannel_U32 = _NbChannel_U32;
          mpAudioEngine_X[_AudioEngineId_E].AudioCbParam_X.SampleRate_U32 = _SampleRate_U32;
          mpAudioEngine_X[_AudioEngineId_E].AudioCbParam_X.AudioFormat_E = _AudioFormat_E;
          mpAudioEngine_X[_AudioEngineId_E].MaConfig_X = ma_device_config_init(ma_device_type_playback);

          mpAudioEngine_X[_AudioEngineId_E].MaConfig_X.playback.format = static_cast<ma_format>(mpAudioEngine_X[_AudioEngineId_E].AudioCbParam_X.AudioFormat_E); // ma_format_s32;
          mpAudioEngine_X[_AudioEngineId_E].MaConfig_X.playback.channels = mpAudioEngine_X[_AudioEngineId_E].AudioCbParam_X.NbChannel_U32;
          mpAudioEngine_X[_AudioEngineId_E].MaConfig_X.sampleRate = mpAudioEngine_X[_AudioEngineId_E].AudioCbParam_X.SampleRate_U32;// APE::AUDIO_SAMPLE_RATE;
          mpAudioEngine_X[_AudioEngineId_E].MaConfig_X.dataCallback = Cb_OnNeedAudioData;                                            // This function will be called when miniaudio needs more data.
          mpAudioEngine_X[BOF_AUDIO_ENGINE_ID_PLAYBACK].AudioCbParam_X.AudioEngineId_E = _AudioEngineId_E;
          mpAudioEngine_X[BOF_AUDIO_ENGINE_ID_PLAYBACK].AudioCbParam_X.pAudioDevice = this;
          mpAudioEngine_X[_AudioEngineId_E].MaConfig_X.pUserData = &mpAudioEngine_X[BOF_AUDIO_ENGINE_ID_PLAYBACK].AudioCbParam_X; // Can be accessed from the device object (device.pUserData).
          mpAudioEngine_X[_AudioEngineId_E].MaConfig_X.playback.pDeviceID = reinterpret_cast<const ma_device_id *>(mpAudioEngine_X[_AudioEngineId_E].AudioDeviceInfoCol[i_U32].pId_c);

          if (ma_device_init(nullptr, &mpAudioEngine_X[_AudioEngineId_E].MaConfig_X, &mpAudioEngine_X[_AudioEngineId_E].MaDevice_X) == MA_SUCCESS)
          {
            mpAudioEngine_X[_AudioEngineId_E].Index_S32 = i_U32;
            Rts_B = true;
          }
          break;
        }
      }
    }
  }
  return Rts_B;
}
bool BofAudioDevice::Start(BOF_AUDIO_ENGINE_ID _AudioEngineId_E, BOF_AUDIO_NEED_DATA_CALLBACK _OnNeedAudioData)
{
  bool Rts_B = false;

  if (_AudioEngineId_E < BOF_AUDIO_ENGINE_ID_MAX)
  {
    if (mpAudioEngine_X[_AudioEngineId_E].Index_S32 >= 0)
    {
      if (!mpAudioEngine_X[_AudioEngineId_E].Started_B)
      {
        mpAudioEngine_X[_AudioEngineId_E].AudioCbParam_X.OnNeedAudioData = _OnNeedAudioData;
        if (ma_device_start(&mpAudioEngine_X[_AudioEngineId_E].MaDevice_X) == MA_SUCCESS)
        {
          mpAudioEngine_X[_AudioEngineId_E].Started_B = true;
          Rts_B = true;
        }
      }
    }
  }
  return Rts_B;
}
bool BofAudioDevice::Stop(BOF_AUDIO_ENGINE_ID _AudioEngineId_E)
{
  bool Rts_B = false;

  if (_AudioEngineId_E < BOF_AUDIO_ENGINE_ID_MAX)
  {
    if (mpAudioEngine_X[_AudioEngineId_E].Index_S32 >= 0)
    {
      if (mpAudioEngine_X[_AudioEngineId_E].Started_B)
      {
        if (ma_device_stop(&mpAudioEngine_X[_AudioEngineId_E].MaDevice_X) == MA_SUCCESS)
        {
          mpAudioEngine_X[_AudioEngineId_E].Started_B = false;
          Rts_B = true;
        }
      }
    }
  }
  return Rts_B;
}
bool BofAudioDevice::ShutdownEngineDevice(BOF_AUDIO_ENGINE_ID _AudioEngineId_E)
{
  bool Rts_B = false;

  if (_AudioEngineId_E < BOF_AUDIO_ENGINE_ID_MAX)
  {
    if (mpAudioEngine_X[_AudioEngineId_E].Index_S32 >= 0)
    {
      Stop(_AudioEngineId_E);

      ma_device_uninit(&mpAudioEngine_X[_AudioEngineId_E].MaDevice_X);
      mpAudioEngine_X[_AudioEngineId_E].Index_S32 = -1;
      Rts_B = true;
    }
  }
  return Rts_B;
}

END_BOF_NAMESPACE()