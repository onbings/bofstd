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

BEGIN_BOF_NAMESPACE()

using AudioSampleFormatEnumConverter = BOF_NAMESPACE::BofEnum<BOF_AUDIO_SAMPLE_FORMAT>;
static AudioSampleFormatEnumConverter &S_AudioSampleFormatConverter()
{
	static AudioSampleFormatEnumConverter S_TheAudioSampleFormatConverter
	{
		{
			{BOF_AUDIO_SAMPLE_FORMAT::BOF_AUDIO_SAMPLE_FORMAT_S24L32, "S24L32"},
			{BOF_AUDIO_SAMPLE_FORMAT::BOF_AUDIO_SAMPLE_FORMAT_MAX, "FMT_MAX"},
			{BOF_AUDIO_SAMPLE_FORMAT::BOF_AUDIO_SAMPLE_FORMAT_UNKNOWN, "FMT_???"},
		},
//			COMMIT_IP_SPF_CONFIG_STATE::UNDEF
	};
	return S_TheAudioSampleFormatConverter;
}
BofAudioStandard::BofAudioStandard()
{
}

BofAudioStandard::BofAudioStandard(const char *_pStandard_c)
{
	S_Parse(_pStandard_c, mNbMonoChannel_U32, mSamplingRateInHz_U32, mNbBitPerSample_U32, mSampleFormat_E);
}

BofAudioStandard::BofAudioStandard(uint32_t _NbMonoChannel_U32, uint32_t _SamplingRateInHz_U32, BOF_AUDIO_SAMPLE_FORMAT _SampleFormat_E)
{
	switch(_SampleFormat_E)
	{
		case BOF_AUDIO_SAMPLE_FORMAT::BOF_AUDIO_SAMPLE_FORMAT_S24L32:
			mNbBitPerSample_U32=32;
			break;

		default:
			mNbBitPerSample_U32=0;
			break;
	}
	mNbMonoChannel_U32=_NbMonoChannel_U32;
	mSamplingRateInHz_U32=_SamplingRateInHz_U32;
	mSampleFormat_E=_SampleFormat_E;
}

BofAudioStandard &BofAudioStandard::operator=(const BofAudioStandard &_rStandard)
{
	mNbMonoChannel_U32=_rStandard.NbMonoChannel();
	mSamplingRateInHz_U32=_rStandard.SamplingRateInHz();
	mNbBitPerSample_U32=_rStandard.NbBitPerSample();
	mSampleFormat_E=_rStandard.SampleFormat();

	return *this;
}
bool BofAudioStandard::operator==(const BofAudioStandard &_rStandard) const
{
	return ((mNbMonoChannel_U32==_rStandard.NbMonoChannel()) && (mSamplingRateInHz_U32==_rStandard.SamplingRateInHz()) && (mNbBitPerSample_U32==_rStandard.NbBitPerSample()) && (mSampleFormat_E==_rStandard.SampleFormat()));
}

std::string BofAudioStandard::ToString() const
{
	return Bof_Sprintf("%dx%d_%s",mNbMonoChannel_U32, mSamplingRateInHz_U32, S_AudioSampleFormatConverter().ToString(mSampleFormat_E).c_str());
}

AudioStandardId BofAudioStandard::Id() const
{
	return BOF_AUDIO_STANDARD_ID(mNbMonoChannel_U32,mSamplingRateInHz_U32,static_cast<uint32_t>(mSampleFormat_E));
}

bool BofAudioStandard::Valid() const
{
	return (mSampleFormat_E!=BOF_AUDIO_SAMPLE_FORMAT::BOF_AUDIO_SAMPLE_FORMAT_UNKNOWN) ? true:false;
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

bool BofAudioStandard::S_Parse(const char *_pStandard_c, uint32_t &_rNbMonoChannel_U32, uint32_t &_rSamplingRateInHz_U32, uint32_t &_rNbBitPerSample_U32, BOF_AUDIO_SAMPLE_FORMAT &_rSampleFormat_E)
{
	bool Rts_B=false;
	char pAudioFormat_c[0x400];
	int NbMonoChannel_i,SamplingRateInHz_i,BitPerSample_i,ResolutionBit_i;
	char SignedSample_c,LittleEndian_c;

	if ((_pStandard_c) && (sscanf(_pStandard_c,"%dx%d_%s",&NbMonoChannel_i,&SamplingRateInHz_i,pAudioFormat_c)==3))
	{
		_rNbMonoChannel_U32=NbMonoChannel_i;
		_rSamplingRateInHz_U32=SamplingRateInHz_i;
		if (sscanf(_pStandard_c,"%c%d%c%d",&SignedSample_c,&ResolutionBit_i,&LittleEndian_c,&BitPerSample_i)==4)
		{
			_rNbBitPerSample_U32=BitPerSample_i;
			_rSampleFormat_E = S_AudioSampleFormatConverter().ToEnum(pAudioFormat_c);
			Rts_B=true;
		}
	}
	return Rts_B;
}

END_BOF_NAMESPACE()