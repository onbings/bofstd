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
#include <bofstd/bofaudiostandard.h>
#include <bofstd/bofenum.h>

#include <regex>

BEGIN_BOF_NAMESPACE()

static BofEnum<BOF_AUDIO_SAMPLE_FORMAT> S_BofAudioStandardEnumConverter({
      {BOF_AUDIO_SAMPLE_FORMAT::BOF_AUDIO_SAMPLE_FORMAT_UNKNOWN, "UNKNOW"},
      {BOF_AUDIO_SAMPLE_FORMAT::BOF_AUDIO_SAMPLE_FORMAT_S24L32, "S24L32"},
      {BOF_AUDIO_SAMPLE_FORMAT::BOF_AUDIO_SAMPLE_FORMAT_MAX, "MAX"},
                                                                        }, BOF_AUDIO_SAMPLE_FORMAT::BOF_AUDIO_SAMPLE_FORMAT_UNKNOWN);

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
  return (true);  //it is valid even it is BOF_AUDIO_SAMPLE_FORMAT_UNKNOWN if  mSampleFormat_E != BOF_AUDIO_SAMPLE_FORMAT::BOF_AUDIO_SAMPLE_FORMAT_UNKNOWN) ? true : false;
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