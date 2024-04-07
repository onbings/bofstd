/*
 * Copyright (c) 2015-2025, Onbings. All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module defines audio standard class manipulation
 *
 * Name:        bofaudiostandard.h
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
#pragma once
#include <bofstd/bofstring.h>

BEGIN_BOF_NAMESPACE()
enum class BOF_AUDIO_SAMPLE_FORMAT : uint32_t // See vlc_fourcc.h VLC_CODEC_S24L32
{
  BOF_AUDIO_SAMPLE_FORMAT_UNKNOWN = 0,
  BOF_AUDIO_SAMPLE_FORMAT_S24L32,
  BOF_AUDIO_SAMPLE_FORMAT_MAX,
};

//            "AudioStandard": "16xS24L32@48000",
#define BOF_AUDIO_STANDARD_ID(mono, freq, format) (((uint64_t)(mono & 0xFF) << 56) | (((uint64_t)((freq & 0xFFFFFFFF))) << 32) | (((uint64_t)((format & 0xFF))) << 24))

typedef uint64_t AudioStandardId;
static const AudioStandardId DefaultAudioStandard = BOF_AUDIO_STANDARD_ID(16, 48000, static_cast<uint32_t>(BOF_AUDIO_SAMPLE_FORMAT::BOF_AUDIO_SAMPLE_FORMAT_S24L32));

enum class BOF_WAVE_FORM_TYPE
{
  BOF_WAVE_FORM_TYPE_SINUS = 0,
  BOF_WAVE_FORM_TYPE_SQUARE,
  BOF_WAVE_FORM_TYPE_TRIANGLE,
  BOF_WAVE_FORM_TYPE_SAW_TOOTH,
  BOF_WAVE_FORM_TYPE_MAX
};

bool Bof_GenerateWaveform(BOF_WAVE_FORM_TYPE _WaveForm_E, float _Amplitude_f, float _Frequency_f, float _SampleRate_f, uint32_t _ChunkSize_U32, float *_pDataY_f, float &_rPhase_f);

class BOFSTD_EXPORT BofAudioStandard
{
public:
  BofAudioStandard();
  BofAudioStandard(const std::string &_rStandard_S);
  BofAudioStandard(uint32_t _NbMonoChannel_U32, uint32_t _SamplingRateInHz_U32, BOF_AUDIO_SAMPLE_FORMAT _SampleFormat_E);
  BofAudioStandard &operator=(const BofAudioStandard &_rStandard);
  bool operator==(const BofAudioStandard &_rStandard) const;
  std::string ToString() const;
  AudioStandardId Id() const;
  bool IsValid() const;
  uint32_t NbBitPerSample() const;
  uint32_t NbMonoChannel() const;
  uint32_t SamplingRateInHz() const;
  BOF_AUDIO_SAMPLE_FORMAT SampleFormat() const;
  static bool S_Parse(const std::string &_rStandard_S, uint32_t &_rNbMonoChannel_U32, uint32_t &_rSamplingRateInHz_U32, uint32_t &_rNbBitPerSample_U32, BOF_AUDIO_SAMPLE_FORMAT &_rSampleFormat_E);

private:
  uint32_t mNbBitPerSample_U32 = 0;
  uint32_t mNbMonoChannel_U32 = 0;
  uint32_t mSamplingRateInHz_U32 = 0;
  BOF_AUDIO_SAMPLE_FORMAT mSampleFormat_E = BOF_AUDIO_SAMPLE_FORMAT::BOF_AUDIO_SAMPLE_FORMAT_UNKNOWN;
};

END_BOF_NAMESPACE()