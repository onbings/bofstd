BOFERR IBofIoConnection::OnReadyRead(uint32_t _NbNewByte_U32)
{
	BOFERR Rts_E = BOFERR_NO_ERROR, Sts_E;
	uint32_t Nb_U32, NbRemainingByte_U32, StartIndex_U32, LastIndex_U32, i_U32, NbByteInDelimitedPacket_U32, NbByteToMove_U32;

	if (_NbNewByte_U32)
	{
		NbRemainingByte_U32 = mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32 - mWriteIndex_U32;
		Nb_U32 = (_NbNewByte_U32 <= NbRemainingByte_U32) ? _NbNewByte_U32 : NbRemainingByte_U32;

		if (Nb_U32)
		{
			//       memcpy(&mpDataBuffer_U8[mWriteIndex_U32], _rBuffer_X.pData_U8, Nb_U32);
			StartIndex_U32 = mWriteIndex_U32;
			LastIndex_U32 = StartIndex_U32 + Nb_U32;
			mWriteIndex_U32 += Nb_U32;

			if (mIBofIoConnectionParam_X.NotifyType_E == BOF_IO_NOTIFY_TYPE::ASAP)
			{
				Sts_E = V_DataRead(Nb_U32, mpDataBuffer_U8);
				mWriteIndex_U32 = 0;
				mDelimiterStartIndex_U32 = 0;
			}
			else if (mIBofIoConnectionParam_X.NotifyType_E == BOF_IO_NOTIFY_TYPE::WHEN_FULL)
			{
				if (mWriteIndex_U32 == mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32)
				{
					Sts_E = V_DataRead(mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32, mpDataBuffer_U8);
					mWriteIndex_U32 = 0;
					mDelimiterStartIndex_U32 = 0;
				}
			}
			else if (mIBofIoConnectionParam_X.NotifyType_E == BOF_IO_NOTIFY_TYPE::WHEN_FULL_OR_DELIMITER_FOUND)
			{
				//We look for Delimiter_U8 in mpDataBuffer_U8 data chunk per data chunk
				BOF_ASSERT(mWriteIndex_U32 <= mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32);
				BOF_ASSERT(mDelimiterStartIndex_U32 < mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32);
				BOF_ASSERT(LastIndex_U32 <= mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32);

				do
				{
					for (i_U32 = StartIndex_U32; i_U32 < LastIndex_U32; i_U32++)
					{
						if (mpDataBuffer_U8[i_U32] == mIBofIoConnectionParam_X.Delimiter_U8)
						{
							NbByteInDelimitedPacket_U32 = i_U32 - mDelimiterStartIndex_U32 + 1;
							BOF_ASSERT(NbByteInDelimitedPacket_U32 <= mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32);

							Sts_E = V_DataRead(NbByteInDelimitedPacket_U32, &mpDataBuffer_U8[mDelimiterStartIndex_U32]);

							StartIndex_U32 = i_U32 + 1;
							BOF_ASSERT(StartIndex_U32 <= mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32);

							mDelimiterStartIndex_U32 = i_U32 + 1;
							BOF_ASSERT(mDelimiterStartIndex_U32 <= mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32);
							break;
						}
					}
				} while (i_U32 < LastIndex_U32);

				if (mWriteIndex_U32 >= mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32)  //can happens if last byte of buffer is equal to Delimiter_U8 or no Delimiter_U8 byte found before the end
				{
					if (mDelimiterStartIndex_U32 == 0)
					{
						Sts_E = V_DataRead(mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32, mpDataBuffer_U8);
						mWriteIndex_U32 = 0;
						mDelimiterStartIndex_U32 = 0;
					}
					else
					{
						NbByteToMove_U32 = mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32 - mDelimiterStartIndex_U32;

						BOF_ASSERT(NbByteToMove_U32 < mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32);
						if (NbByteToMove_U32)
						{
							memcpy(mpDataBuffer_U8, &mpDataBuffer_U8[mDelimiterStartIndex_U32], NbByteToMove_U32);
						}
						mWriteIndex_U32 = NbByteToMove_U32;
						mDelimiterStartIndex_U32 = 0;
					}
				}
				BOF_ASSERT(mWriteIndex_U32 <= mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32);
				BOF_ASSERT(mDelimiterStartIndex_U32 <= mIBofIoConnectionParam_X.NotifyRcvBufferSize_U32);
			}
		}
	}
	return Rts_E;
}











BOFERR BofSocketChannel::OnReadyRead(uint32_t _NbNewByte_U32, bool _Partial_B, const BOF_SOCKET_PEER_INFO &_rPeerInfo_X)
{
	BOFERR Rts_E = BOFERR_NO_ERROR, Sts_E;
	uint32_t Nb_U32, NbRemainingByte_U32, StartIndex_U32, LastIndex_U32, i_U32, NbByteInDelimitedPacket_U32, NbByteToMove_U32;

	if (_NbNewByte_U32)
	{
		NbRemainingByte_U32 = mBofSocketChannelParam_X.NotifyRcvBufferSize_U32 - mWriteIndex_U32;
		Nb_U32 = (_NbNewByte_U32 <= NbRemainingByte_U32) ? _NbNewByte_U32 : NbRemainingByte_U32;

		if (Nb_U32)
		{
			//       memcpy(&mpDataBuffer_U8[mWriteIndex_U32], _rBuffer_X.pData_U8, Nb_U32);
			StartIndex_U32 = mWriteIndex_U32;
			LastIndex_U32 = StartIndex_U32 + Nb_U32;
			mWriteIndex_U32 += Nb_U32;

			if (mBofSocketChannelParam_X.NotifyType_E == BOF_SOCKET_NOTIFY_TYPE::NEVER)
			{
				mWriteIndex_U32 = 0;
				mDelimiterStartIndex_U32 = 0;
			}
			else if (mBofSocketChannelParam_X.NotifyType_E == BOF_SOCKET_NOTIFY_TYPE::ASAP)
			{
				Sts_E = OnDataRead(Nb_U32, mpDataBuffer_U8, _Partial_B, _rPeerInfo_X);
				if (Sts_E == BOFERR_NO_ERROR)
				{
					mBofSocketChannelStatistic_X.NbOpRead_U32++;
					mBofSocketChannelStatistic_X.NbDataByteRead_U64 += Nb_U32;
				}
				else
				{
					mBofSocketChannelStatistic_X.NbOpReadError_U32++;
				}
				mWriteIndex_U32 = 0;
				mDelimiterStartIndex_U32 = 0;
			}
			else if (mBofSocketChannelParam_X.NotifyType_E == BOF_SOCKET_NOTIFY_TYPE::WHEN_FULL)
			{
				if (mWriteIndex_U32 == mBofSocketChannelParam_X.NotifyRcvBufferSize_U32)
				{
					Sts_E = OnDataRead(mBofSocketChannelParam_X.NotifyRcvBufferSize_U32, mpDataBuffer_U8, _Partial_B, _rPeerInfo_X);
					if (Sts_E == BOFERR_NO_ERROR)
					{
						mBofSocketChannelStatistic_X.NbOpRead_U32++;
						mBofSocketChannelStatistic_X.NbDataByteRead_U64 += mBofSocketChannelParam_X.NotifyRcvBufferSize_U32;
					}
					else
					{
						mBofSocketChannelStatistic_X.NbOpReadError_U32++;
					}
					mWriteIndex_U32 = 0;
					mDelimiterStartIndex_U32 = 0;
				}
			}
			else if (mBofSocketChannelParam_X.NotifyType_E == BOF_SOCKET_NOTIFY_TYPE::WHEN_FULL_OR_DELIMITER_FOUND)
			{
				//We look for Delimiter_U8 in mpDataBuffer_U8 data chunk per data chunk
				BOF_ASSERT(mWriteIndex_U32 <= mBofSocketChannelParam_X.NotifyRcvBufferSize_U32);
				BOF_ASSERT(mDelimiterStartIndex_U32 < mBofSocketChannelParam_X.NotifyRcvBufferSize_U32);
				BOF_ASSERT(LastIndex_U32 <= mBofSocketChannelParam_X.NotifyRcvBufferSize_U32);

				do
				{
					for (i_U32 = StartIndex_U32; i_U32 < LastIndex_U32; i_U32++)
					{
						if (mpDataBuffer_U8[i_U32] == mBofSocketChannelParam_X.Delimiter_U8)
						{
							NbByteInDelimitedPacket_U32 = i_U32 - mDelimiterStartIndex_U32 + 1;
							BOF_ASSERT(NbByteInDelimitedPacket_U32 <= mBofSocketChannelParam_X.NotifyRcvBufferSize_U32);

							Sts_E = OnDataRead(NbByteInDelimitedPacket_U32, &mpDataBuffer_U8[mDelimiterStartIndex_U32], _Partial_B, _rPeerInfo_X);
							if (Sts_E == BOFERR_NO_ERROR)
							{
								mBofSocketChannelStatistic_X.NbOpRead_U32++;
								mBofSocketChannelStatistic_X.NbDataByteRead_U64 += NbByteInDelimitedPacket_U32;
							}
							else
							{
								mBofSocketChannelStatistic_X.NbOpReadError_U32++;
							}
							StartIndex_U32 = i_U32 + 1;
							BOF_ASSERT(StartIndex_U32 <= mBofSocketChannelParam_X.NotifyRcvBufferSize_U32);

							mDelimiterStartIndex_U32 = i_U32 + 1;
							BOF_ASSERT(mDelimiterStartIndex_U32 <= mBofSocketChannelParam_X.NotifyRcvBufferSize_U32);
							break;
						}
					}
				} while (i_U32 < LastIndex_U32);

				if (mWriteIndex_U32 >= mBofSocketChannelParam_X.NotifyRcvBufferSize_U32)  //can happens if last byte of buffer is equal to Delimiter_U8 or no Delimiter_U8 byte found before the end
				{
					if (mDelimiterStartIndex_U32 == 0)
					{
						Sts_E = OnDataRead(mBofSocketChannelParam_X.NotifyRcvBufferSize_U32, mpDataBuffer_U8, _Partial_B, _rPeerInfo_X);
						if (Sts_E == BOFERR_NO_ERROR)
						{
							mBofSocketChannelStatistic_X.NbOpRead_U32++;
							mBofSocketChannelStatistic_X.NbDataByteRead_U64 += mBofSocketChannelParam_X.NotifyRcvBufferSize_U32;
						}
						else
						{
							mBofSocketChannelStatistic_X.NbOpReadError_U32++;
						}
						mWriteIndex_U32 = 0;
						mDelimiterStartIndex_U32 = 0;
					}
					else
					{
						NbByteToMove_U32 = mBofSocketChannelParam_X.NotifyRcvBufferSize_U32 - mDelimiterStartIndex_U32;

						BOF_ASSERT(NbByteToMove_U32 < mBofSocketChannelParam_X.NotifyRcvBufferSize_U32);
						if (NbByteToMove_U32)
						{
							memcpy(mpDataBuffer_U8, &mpDataBuffer_U8[mDelimiterStartIndex_U32], NbByteToMove_U32);
						}
						mWriteIndex_U32 = NbByteToMove_U32;
						mDelimiterStartIndex_U32 = 0;
					}
				}
				BOF_ASSERT(mWriteIndex_U32 <= mBofSocketChannelParam_X.NotifyRcvBufferSize_U32);
				BOF_ASSERT(mDelimiterStartIndex_U32 <= mBofSocketChannelParam_X.NotifyRcvBufferSize_U32);
			}
		}
	}
	return Rts_E;
}