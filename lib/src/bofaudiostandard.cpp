/*
 * Copyright (c) 2015-2025, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements audio standard class
 *
 * Name:        bofaudiostandard.cpp
 * Author:      Bernard HARMEL: onbings@dscloud.me
 * Web:			    onbings.dscloud.me
 * Revision:    1.0
 *
 * Rem:         Nothing
 *
 * History:
 *
 * V 1.00  May 26 2020  BHA : Initial release
 */
#define _USE_MATH_DEFINES
#include <cmath>
#include <regex>

#include <bofstd/bofaudiostandard.h>
#include <bofstd/bofenum.h>

BEGIN_BOF_NAMESPACE()
bool Bof_GenerateWaveform(BOF_WAVE_FORM_TYPE _WaveForm_E, float _Amplitude_f, float _Frequency_f,
                          float _SampleRate_f, uint32_t _ChunkSize_U32, float *_pDataY_f, float &_rPhase_f)
{
  bool Rts_B = false;
  uint32_t i_U32;
  float IncPhase_f, Phase_f, PeriodFactor_f;

  //_pDataX_f tested below !!
  if ((_WaveForm_E < BOF_WAVE_FORM_TYPE::BOF_WAVE_FORM_TYPE_MAX) && (_Amplitude_f > 0.0f) && (_Frequency_f > 0.0f) && (_SampleRate_f > 0.0f) && (_ChunkSize_U32) && (_pDataY_f))
  {
    Rts_B = true;
    // After _SampleRate_f / _Frequency_f we have covered 2 M_PI, so for Fr 440 Fe 44100 we roll over angle after 100 sample->So for a chunk of 1024 byte we have 10 sinus
    IncPhase_f = (2.0f * M_PI * _Frequency_f) / _SampleRate_f;
    /* PeriodFactor_f / 10.0f->the ajust factor is 0.628 ???
    Triangle	2.0f * M_PI / 2.0f (2PI for one cycle, divided by 2 for two triangles)
    Sawtooth	2.0f * M_PI * 0.25f (2PI for one cycle, compressed to 0-1 range, adjusted by 0.25)
    */
    PeriodFactor_f = 2.0f * M_PI / 10.0f; // *_Frequency_f / _SampleRate_f;
    Phase_f = _rPhase_f;

    switch (_WaveForm_E)
    {
      case BOF_WAVE_FORM_TYPE::BOF_WAVE_FORM_TYPE_SINUS:
        for (i_U32 = 0; i_U32 < _ChunkSize_U32; i_U32++)
        {
          _pDataY_f[i_U32] = _Amplitude_f * sin(Phase_f);
          Phase_f += IncPhase_f;
        }
        break;
      case BOF_WAVE_FORM_TYPE::BOF_WAVE_FORM_TYPE_SQUARE:
        for (i_U32 = 0; i_U32 < _ChunkSize_U32; i_U32++)
        {
          _pDataY_f[i_U32] = (sin(Phase_f) >= 0) ? _Amplitude_f : -_Amplitude_f;
          Phase_f += IncPhase_f;
        }
        break;
      case BOF_WAVE_FORM_TYPE::BOF_WAVE_FORM_TYPE_TRIANGLE:
        for (i_U32 = 0; i_U32 < _ChunkSize_U32; i_U32++)
        {
          _pDataY_f[i_U32] = (_Amplitude_f * fabs(fmod(Phase_f * PeriodFactor_f, 4.0f) - 2.0f)) - _Amplitude_f;
          Phase_f += IncPhase_f;
        }
        break;
      case BOF_WAVE_FORM_TYPE::BOF_WAVE_FORM_TYPE_SAW_TOOTH:
        for (i_U32 = 0; i_U32 < _ChunkSize_U32; i_U32++)
        {
          _pDataY_f[i_U32] = (_Amplitude_f * (2.0f * fmod(Phase_f * PeriodFactor_f * 0.25f, 1.0f)) - _Amplitude_f);
          Phase_f += IncPhase_f;
        }
        break;

      default:
        Rts_B = false;
        break;
    }

    if (Rts_B)
    {
      _rPhase_f = fmod(Phase_f, 2.0f * M_PI); // Keep phase within 0 to 2*PI range
    }
  }
  return Rts_B;
}

static BofEnum<BOF_AUDIO_SAMPLE_FORMAT> S_BofAudioStandardEnumConverter(
    {
        {BOF_AUDIO_SAMPLE_FORMAT::BOF_AUDIO_SAMPLE_FORMAT_UNKNOWN, "UNKNOW"},
        {BOF_AUDIO_SAMPLE_FORMAT::BOF_AUDIO_SAMPLE_FORMAT_S24L32, "S24L32"},
        {BOF_AUDIO_SAMPLE_FORMAT::BOF_AUDIO_SAMPLE_FORMAT_MAX, "MAX"},
    },
    BOF_AUDIO_SAMPLE_FORMAT::BOF_AUDIO_SAMPLE_FORMAT_UNKNOWN);

BofAudioStandard::BofAudioStandard()
{
}

BofAudioStandard::BofAudioStandard(const std::string &_rStandard_S)
{
  S_Parse(_rStandard_S.c_str(), mNbMonoChannel_U32, mSamplingRateInHz_U32, mNbBitPerSample_U32, mSampleFormat_E);
}

BofAudioStandard::BofAudioStandard(uint32_t _NbMonoChannel_U32, uint32_t _SamplingRateInHz_U32, BOF_AUDIO_SAMPLE_FORMAT _SampleFormat_E)
{
  switch (_SampleFormat_E)
  {
    case BOF_AUDIO_SAMPLE_FORMAT::BOF_AUDIO_SAMPLE_FORMAT_S24L32:
      mNbBitPerSample_U32 = 32;
      break;

    default:
      mNbBitPerSample_U32 = 0;
      break;
  }
  mNbMonoChannel_U32 = _NbMonoChannel_U32;
  mSamplingRateInHz_U32 = _SamplingRateInHz_U32;
  mSampleFormat_E = _SampleFormat_E;
}

BofAudioStandard &BofAudioStandard::operator=(const BofAudioStandard &_rStandard)
{
  mNbMonoChannel_U32 = _rStandard.NbMonoChannel();
  mSamplingRateInHz_U32 = _rStandard.SamplingRateInHz();
  mNbBitPerSample_U32 = _rStandard.NbBitPerSample();
  mSampleFormat_E = _rStandard.SampleFormat();

  return *this;
}
bool BofAudioStandard::operator==(const BofAudioStandard &_rStandard) const
{
  return ((mNbMonoChannel_U32 == _rStandard.NbMonoChannel()) && (mSamplingRateInHz_U32 == _rStandard.SamplingRateInHz()) && (mNbBitPerSample_U32 == _rStandard.NbBitPerSample()) && (mSampleFormat_E == _rStandard.SampleFormat()));
}

std::string BofAudioStandard::ToString() const
{
  return Bof_Sprintf("%dx%s@%d", mNbMonoChannel_U32, S_BofAudioStandardEnumConverter.ToString(mSampleFormat_E).c_str(), mSamplingRateInHz_U32);
}

AudioStandardId BofAudioStandard::Id() const
{
  return BOF_AUDIO_STANDARD_ID(mNbMonoChannel_U32, mSamplingRateInHz_U32, static_cast<uint32_t>(mSampleFormat_E));
}

bool BofAudioStandard::IsValid() const
{
  return (true); // it is valid even it is BOF_AUDIO_SAMPLE_FORMAT_UNKNOWN if  mSampleFormat_E != BOF_AUDIO_SAMPLE_FORMAT::BOF_AUDIO_SAMPLE_FORMAT_UNKNOWN) ? true : false;
}

uint32_t BofAudioStandard::NbBitPerSample() const
{
  return mNbBitPerSample_U32;
}

uint32_t BofAudioStandard::NbMonoChannel() const
{
  return mNbMonoChannel_U32;
}

uint32_t BofAudioStandard::SamplingRateInHz() const
{
  return mSamplingRateInHz_U32;
}

BOF_AUDIO_SAMPLE_FORMAT BofAudioStandard::SampleFormat() const
{
  return mSampleFormat_E;
}
//            "AudioStandard": "16x48000_S24L32",

bool BofAudioStandard::S_Parse(const std::string &_rStandard_S, uint32_t &_rNbMonoChannel_U32, uint32_t &_rSamplingRateInHz_U32, uint32_t &_rNbBitPerSample_U32, BOF_AUDIO_SAMPLE_FORMAT &_rSampleFormat_E)
{
  bool Rts_B = false;
  uint32_t ResolutionBit_U32;
  char SignedSample_c, LittleEndian_c;
  std::string Standard_S;
  std::smatch MatchString;
  static const std::regex S_RegExAudioStandard(R"(^(\d*)x(.)(\d*)(.)(\d*)@(\d*))");

  if (std::regex_search(_rStandard_S, MatchString, S_RegExAudioStandard))
  {
    if (MatchString.size() == 7)
    {
      _rNbMonoChannel_U32 = std::atol(MatchString[1].str().c_str());
      SignedSample_c = MatchString[2].str().c_str()[0];
      ResolutionBit_U32 = std::atol(MatchString[3].str().c_str());
      LittleEndian_c = MatchString[4].str().c_str()[0];
      _rNbBitPerSample_U32 = std::atol(MatchString[5].str().c_str());
      Standard_S = MatchString[2].str() + MatchString[3].str() + MatchString[4].str() + MatchString[5].str();
      _rSamplingRateInHz_U32 = std::atol(MatchString[6].str().c_str());
      _rSampleFormat_E = S_BofAudioStandardEnumConverter.ToEnum(Standard_S);
      Rts_B = true;
    }
  }
  return Rts_B;
}

END_BOF_NAMESPACE()