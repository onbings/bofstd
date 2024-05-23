/*
 * Copyright (c) 2024-2029, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines different audio interface
 *
 * Author:      Bernard HARMEL: onbings@gmail.com
 *
 * History:
 * V 1.00  Apr 11 2024  BHA : Initial release
 */
#pragma once
#include <string>
#include <vector>
#include <functional>
#include <limits>
#include <bofstd/bofstd.h>
#include <miniaudio.h>

BEGIN_BOF_NAMESPACE()
#pragma pack(1)
struct BOF_WAV_FILE_HEADER
{
  // RIFF Chunk Descriptor
  char pChunkId_c[4];     // "RIFF" (4 bytes)
  uint32_t ChunkSize_U32; // File size - 8 (4 bytes)
  char pFormat_c[4];      // "WAVE" (4 bytes)

  // Format Chunk
  char pSubchunk1Id_c[4];     // "fmt " (4 bytes)
  uint32_t SubChunk1Size_U32; // Size of the fmt chunk (16 or 18 bytes for PCM) (4 bytes)
  uint16_t AudioFormat_U16;   // Audio format (PCM = 1) (2 bytes)
  uint16_t NbChannel_U16;    // Number of channels (mono = 1, stereo = 2, etc.) (2 bytes)
  uint32_t SampleRate_U32;    // Sample rate (e.g., 44100 Hz) (4 bytes)
  uint32_t ByteRate_U32;      // Byte rate (sampleRate * numChannels * bitsPerSample / 8) (4 bytes)
  uint16_t BlockAlign_U16;    // Block align (numChannels * bitsPerSample / 8) (2 bytes)
  uint16_t BitPerSample_U16;  // Bits per sample (8, 16, etc.) (2 bytes)
  char pDataId_c[4];     // "data" (4 bytes)
  uint32_t SubChunk2Size_U32; // Si

};
#pragma pack()
enum class BOF_WAVE_FORM_TYPE
{
  BOF_WAVE_FORM_TYPE_LEVEL = 0,
  BOF_WAVE_FORM_TYPE_SINUS,
  BOF_WAVE_FORM_TYPE_COSINUS,
  BOF_WAVE_FORM_TYPE_SQUARE,
  BOF_WAVE_FORM_TYPE_TRIANGLE,
  BOF_WAVE_FORM_TYPE_SAW_TOOTH,
  BOF_WAVE_FORM_TYPE_MAX
};

template <class T>
class BofRampGenerator
{
public:
  BofRampGenerator(T _FromSample, T _ToSample, T _StepSample)
  {
    Reset(_FromSample, _ToSample, _StepSample);
  }
  ~BofRampGenerator()
  {
  }
  bool Reset()
  {
    return Reset(mFromSample, mToSample, mStepSample);
  }
  bool Reset(T _FromSample, T _ToSample, T _StepSample)
  {
    bool Rts_B = false;

    if (((_StepSample > static_cast<T>(0)) && (_ToSample >= _FromSample)) || ((_StepSample < static_cast<T>(0)) && (_ToSample <= _FromSample)))
    {
      Rts_B = true;
      mFromSample = _FromSample;
      mToSample = _ToSample;
      mStepSample = _StepSample;
      mCrtSample = mFromSample;
    }
    return Rts_B;
  }

  T *Generate(uint32_t _HeaderSizeInByte_U32, uint32_t _ChunkSize_U32, uint32_t _FooterSizeInByte_U32, T *_pData)
  {
    T *pRts=nullptr;
    uint32_t i_U32;

    if (_pData)
    {
      pRts = _pData;
      _pData = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(_pData) + _HeaderSizeInByte_U32);
      for (i_U32 = 0; i_U32 < _ChunkSize_U32; i_U32++)
      {
        _pData[i_U32] = mCrtSample;
        mCrtSample += mStepSample;
        if (mCrtSample > mToSample)
        {
          mCrtSample = mFromSample;
        }
      }
      _pData += i_U32;
      pRts = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(_pData) + _FooterSizeInByte_U32);
    }
    return pRts;
  }

private:
  T mFromSample = std::numeric_limits<T>::min();
  T mToSample = std::numeric_limits<T>::max();
  T mStepSample = 0;
  T mCrtSample = 0;
};

template <class T>
class BofWaveformGenerator
{
public:
  BofWaveformGenerator(BOF_WAVE_FORM_TYPE _WaveForm_E, T _Amplitude, uint32_t _Frequency_U32, uint32_t _SampleRate_U32, float _InitialPhase_f)
  {
    Reset(_WaveForm_E, _Amplitude, _Frequency_U32, _SampleRate_U32, _InitialPhase_f);
  }
  ~BofWaveformGenerator()
  {
  }
  bool Reset(float _InitialPhase_f)
  {
    return Reset(mWaveForm_E, mAmplitude, mFrequency_U32, mSampleRate_U32, _InitialPhase_f);
  }
  bool Reset(float _InitialPhase_f, T _Amplitude)
  {
    return Reset(mWaveForm_E, _Amplitude, mFrequency_U32, mSampleRate_U32, _InitialPhase_f);
  }
  bool Reset(float _InitialPhase_f, T _Amplitude, uint32_t _Frequency_U32)
  {
    return Reset(mWaveForm_E, _Amplitude, _Frequency_U32, mSampleRate_U32, _InitialPhase_f);
  }
  bool Reset(BOF_WAVE_FORM_TYPE _WaveForm_E, T _Amplitude, uint32_t _Frequency_U32, uint32_t _SampleRate_U32, float _InitialPhase_f)
  {
    bool Rts_B = false;

    // Amplitude can be < 0 for level if ((_WaveForm_E < BOF_WAVE_FORM_TYPE::BOF_WAVE_FORM_TYPE_MAX) && (_Amplitude > 0.0f) && (_Frequency_U32) && (_SampleRate_U32))
    if ((_WaveForm_E < BOF_WAVE_FORM_TYPE::BOF_WAVE_FORM_TYPE_MAX) && (_Frequency_U32) && (_SampleRate_U32))
    {
      Rts_B = true;
      mWaveForm_E = _WaveForm_E;
      mAmplitude = _Amplitude;
      mFrequency_U32 = _Frequency_U32;
      mSampleRate_U32 = _SampleRate_U32;
      mCrtPhase_lf = fmodf(_InitialPhase_f, 2.0L * static_cast<double>(M_PI)); // Keep phase within 0 to 2*PI range
      mLastPhase_lf = mCrtPhase_lf;
    }
    return Rts_B;
  }
  float CurrentPhase()
  {
    return mCrtPhase_lf;
  }
  float LastPhase()
  {
    return mLastPhase_lf;
  }
  T *Generate(uint32_t _HeaderSizeInByte_U32, uint32_t _ChunkSize_U32, uint32_t _FooterSizeInByte_U32, T *_pDataY)
  {
    T *pRts = nullptr;
    uint32_t i_U32;
    double IncPhase_lf; // , PeriodFactor_lf;

    if ((mWaveForm_E < BOF_WAVE_FORM_TYPE::BOF_WAVE_FORM_TYPE_MAX) && (_ChunkSize_U32) && (_pDataY))
    {
      pRts = _pDataY;
      // After _SampleRate_f / _Frequency_f we have covered 2 M_PI, so for Fr 440 Fe 44100 we roll over angle after 100 sample->So for a chunk of 1024 byte we have 10 sinus
      IncPhase_lf = (2.0L * static_cast<double>(M_PI) * static_cast<double>(mFrequency_U32)) / static_cast<double>(mSampleRate_U32);
      mLastPhase_lf = mCrtPhase_lf;
      /* PeriodFactor_f / 10.0f->the ajust factor is 0.628 ???
      Triangle	2.0f * M_PI / 2.0f (2PI for one cycle, divided by 2 for two triangles)
      Sawtooth	2.0f * M_PI * 0.25f (2PI for one cycle, compressed to 0-1 range, adjusted by 0.25)
      */
      // PeriodFactor_lf = 2.0L * static_cast<double>(M_PI) / 10.0f; // *_Frequency_f / _SampleRate_f;
      _pDataY = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(_pDataY) + _HeaderSizeInByte_U32);
      switch (mWaveForm_E)
      {
        case BOF_WAVE_FORM_TYPE::BOF_WAVE_FORM_TYPE_LEVEL:
          for (i_U32 = 0; i_U32 < _ChunkSize_U32; i_U32++)
          {
            _pDataY[i_U32] = static_cast<T>(mAmplitude);
          }
          break;
        case BOF_WAVE_FORM_TYPE::BOF_WAVE_FORM_TYPE_SINUS:
          for (i_U32 = 0; i_U32 < _ChunkSize_U32; i_U32++)
          {
            _pDataY[i_U32] = static_cast<T>(static_cast<double>(mAmplitude) * sin(mCrtPhase_lf));
            if constexpr (std::is_same_v<T, int32_t>)
            {
              //            printf("i %d Ph %f Amp %f sin %f v %d %08X\n", i_U32, mPhase_f * 360.0f / M_PI, static_cast<float>(_Amplitude), sin(mPhase_f), _pDataY[i_U32], _pDataY[i_U32]);
            }
            mCrtPhase_lf += IncPhase_lf;
          }
          break;
        case BOF_WAVE_FORM_TYPE::BOF_WAVE_FORM_TYPE_COSINUS:
          for (i_U32 = 0; i_U32 < _ChunkSize_U32; i_U32++)
          {
            _pDataY[i_U32] = static_cast<T>(static_cast<float>(mAmplitude) * cos(mCrtPhase_lf));
            mCrtPhase_lf += IncPhase_lf;
          }
          break;
        case BOF_WAVE_FORM_TYPE::BOF_WAVE_FORM_TYPE_SQUARE:
          for (i_U32 = 0; i_U32 < _ChunkSize_U32; i_U32++)
          {
            _pDataY[i_U32] = (sin(mCrtPhase_lf) >= 0) ? static_cast<T>(mAmplitude) : static_cast<T>(-mAmplitude);
            mCrtPhase_lf += IncPhase_lf;
          }
          break;
        case BOF_WAVE_FORM_TYPE::BOF_WAVE_FORM_TYPE_TRIANGLE:
          for (i_U32 = 0; i_U32 < _ChunkSize_U32; i_U32++)
          {
            _pDataY[i_U32] = static_cast<T>(static_cast<float>(mAmplitude) * (acos(sin(mCrtPhase_lf)) / static_cast<double>(M_PI_2) - 1.0f));
            //            _pDataY[i_U32] = static_cast<T>((static_cast<float>(mAmplitude) * fabs(fmod(mCrtPhase_lf * PeriodFactor_lf, 4.0f) - 2.0f)) - static_cast<float>(mAmplitude));
            mCrtPhase_lf += IncPhase_lf;
          }
          break;
        case BOF_WAVE_FORM_TYPE::BOF_WAVE_FORM_TYPE_SAW_TOOTH:
          for (i_U32 = 0; i_U32 < _ChunkSize_U32; i_U32++)
          {
            //_pDataY[i_U32] = static_cast<T>((static_cast<float>(mAmplitude) * (2.0f * fmod(mCrtPhase_lf * PeriodFactor_lf * 0.25f, 1.0f)) - static_cast<float>(mAmplitude)));
            // Calculate sawtooth value using linear function
            _pDataY[i_U32] = static_cast<T>(static_cast<float>(mAmplitude) * (((mCrtPhase_lf / (M_PI))) - 1.0f));
            mCrtPhase_lf += IncPhase_lf;
            mCrtPhase_lf = fmodf(mCrtPhase_lf, 2.0L * static_cast<double>(M_PI)); // Keep phase within 0 to 2*PI range
          }
          break;

        default:
          pRts = nullptr;
          break;
      }

      if (pRts)
      {
        mCrtPhase_lf = fmodf(mCrtPhase_lf, 2.0L * static_cast<double>(M_PI)); // Keep phase within 0 to 2*PI range
        _pDataY += i_U32;
        pRts = reinterpret_cast<T *>(reinterpret_cast<uint8_t *>(_pDataY) + _FooterSizeInByte_U32);
      }
    }
    return pRts;
  }

private:
  BOF_WAVE_FORM_TYPE mWaveForm_E = BOF_WAVE_FORM_TYPE::BOF_WAVE_FORM_TYPE_MAX;
  T mAmplitude = 0;
  uint32_t mFrequency_U32 = 0;
  uint32_t mSampleRate_U32 = 0;
  double mCrtPhase_lf = 0.0f;
  double mLastPhase_lf = 0.0f;
};

struct BOF_AUDIO_DEVICE_PARAM
{
  uint32_t Dummy_U32;
  BOF_AUDIO_DEVICE_PARAM()
  {
    Reset();
  }
  void Reset()
  {
    Dummy_U32 = 0;
  }
};
enum BOF_AUDIO_FORMAT : uint32_t
{
  BOF_AUDIO_FORMAT_UNKNOWN = 0,
  BOF_AUDIO_FORMAT_U8,
  BOF_AUDIO_FORMAT_S16,
  BOF_AUDIO_FORMAT_S24,
  BOF_AUDIO_FORMAT_S32,
  BOF_AUDIO_FORMAT_F32,
  BOF_AUDIO_FORMAT_MAX
};

struct BOF_AUDIO_DEVICE_NATIVE_FORMAT
{
  BOF_AUDIO_FORMAT Format_E;
  uint32_t NbChannel_U32; /* If set to 0, all channels are supported. */
  uint32_t SampleRate_32; /* If set to 0, all sample rates are supported. */
  uint32_t Flag_U32;      /* A combination of MA_DATA_FORMAT_FLAG_* flags. */
  BOF_AUDIO_DEVICE_NATIVE_FORMAT()
  {
    Reset();
  }
  void Reset()
  {
    Format_E = BOF_AUDIO_FORMAT::BOF_AUDIO_FORMAT_UNKNOWN;
    NbChannel_U32 = 0;
    SampleRate_32 = 0;
    Flag_U32 = 0;
  }
};

struct BOF_AUDIO_DEVICE_INFO
{
  char pId_c[256]; // This is an opaque union type (cf ma_device_id)
  std::string Name_S;
  bool IsDefault_B;
  std::vector<BOF_AUDIO_DEVICE_NATIVE_FORMAT> NativeFormatCol;
  BOF_AUDIO_DEVICE_INFO()
  {
    Reset();
  }
  void Reset()
  {
    memset(pId_c, 0, sizeof(pId_c));
    Name_S = "";
    IsDefault_B = false;
    NativeFormatCol.clear();
  }
};
enum BOF_AUDIO_ENGINE_ID : uint32_t
{
  BOF_AUDIO_ENGINE_ID_PLAYBACK = 0,
  BOF_AUDIO_ENGINE_ID_CAPTURE,
  BOF_AUDIO_ENGINE_ID_MAX
};
class BofAudioDevice;
struct BOF_AUDIO_CB_PARAM;
using BOF_AUDIO_NEED_DATA_CALLBACK = std::function<void(BOF_AUDIO_CB_PARAM &_rAudioCbParam_X, void *_pDevice, void *_pOutput, const void *_pInput, uint32_t _FrameCount_U32)>;

struct BOF_AUDIO_CB_PARAM
{
  BOF_AUDIO_NEED_DATA_CALLBACK OnNeedAudioData;
  BOF_AUDIO_ENGINE_ID AudioEngineId_E;
  BofAudioDevice *pAudioDevice;
  uint32_t NbChannel_U32;
  uint32_t SampleRate_U32;
  BOF_AUDIO_FORMAT AudioFormat_E;

  //uint32_t RemainingAudioSample_U32;
  //int32_t *pAudioData_S32;
  BOF_AUDIO_CB_PARAM()
  {
    Reset();
  }
  void Reset()
  {
    OnNeedAudioData = nullptr;
    AudioEngineId_E = BOF_AUDIO_ENGINE_ID::BOF_AUDIO_ENGINE_ID_MAX;
    pAudioDevice = nullptr;
    NbChannel_U32 = 0;
    SampleRate_U32 = 0;
    AudioFormat_E = BOF_AUDIO_FORMAT::BOF_AUDIO_FORMAT_UNKNOWN;

    //RemainingAudioSample_U32 = 0;
    //pAudioData_S32 = nullptr;
  }
};
struct BOF_AUDIO_ENGINE
{
  BOF_AUDIO_CB_PARAM AudioCbParam_X;
  ma_device MaDevice_X;
  ma_device_config MaConfig_X;
  int32_t Index_S32;
  bool Started_B;
  std::vector<BOF_AUDIO_DEVICE_INFO> AudioDeviceInfoCol;

  BOF_AUDIO_ENGINE()
  {
    Reset();
  }
  void Reset()
  {
    AudioCbParam_X.Reset();
    Index_S32 = 0;
    Started_B = false;
    AudioDeviceInfoCol.clear();
  }
};

class BofAudioDevice
{
public:
  BofAudioDevice(const BOF_AUDIO_DEVICE_PARAM &_rAudioDeviceParam_X);
  ~BofAudioDevice();
  uint32_t ScanForDevice();
  std::vector<BOF_AUDIO_DEVICE_INFO> GetDeviceList(BOF_AUDIO_ENGINE_ID _AudioEngineId_E);
  bool InitEngineDevice(BOF_AUDIO_ENGINE_ID _AudioEngineId_E, const std::string &_rName_S, uint32_t _NbChannel_U32,uint32_t _SampleRate_U32, BOF_AUDIO_FORMAT _AudioFormat_E);

  bool Start(BOF_AUDIO_ENGINE_ID _AudioEngineId_E, BOF_AUDIO_NEED_DATA_CALLBACK _OnNeedAudioData);
  bool Stop(BOF_AUDIO_ENGINE_ID _AudioEngineId_E);
  bool ShutdownEngineDevice(BOF_AUDIO_ENGINE_ID _AudioEngineId_E);

//Internal
//  void OnNeedAudioData(BOF_AUDIO_CB_PARAM *_pAudioCbParam_X, ma_device *_pDevice_X, void *_pOutput, const void *_pInput, ma_uint32 _FrameCount_U32);

private:

  BOF_AUDIO_DEVICE_PARAM mAudioDeviceParam_X;
  ma_context mMaContext_X;
  BOF_AUDIO_ENGINE mpAudioEngine_X[BOF_AUDIO_ENGINE_ID_MAX];
};
END_BOF_NAMESPACE()
