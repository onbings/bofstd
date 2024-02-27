#include <bofstd/bofbuffer.h>
#include <bofstd/bofbit.h>

BEGIN_BOF_NAMESPACE()
// https://stackoverflow.com/questions/32652833/how-to-allocate-huge-pages-for-c-application-on-linux
// constexpr char MMGW_HUGE_PAGE_PATH[]="/sys/kernel/mm/hugepages/hugepages-2048kB/page_%08X";  //"/var/lib/hugetlbfs/global/pagesize-2MB/page_%08X";
constexpr char BOF_HUGE_PAGE_PATH[] = "/tmp/hugepages/page_%08X";
constexpr uint32_t BOF_HUGE_PAGE_SIZE = 2 * 1024 * 1024;
//_OsAdvice_i mainly for MADV_DONTFORK option in madvise
BOFERR Bof_AlignedMemAlloc(BOF_BUFFER_ALLOCATE_ZONE _AllocateZone_E, uint32_t _AligmentInByte_U32, uint32_t _SizeInByte_U32, bool _LockIt_B, int _OsAdvice_i, bool _ClearIt_B, BOF_BUFFER &_rAllocatedBuffer_X) //, uint32_t _Offset_U32)
{
  BOFERR Rts_E = BOF_ERR_EINVAL;
  BOF_BUFFER_ALLOCATE_HEADER AllocateBuffer_X;
  static std::atomic<uint32_t> S_HugePageId(0);

  _rAllocatedBuffer_X.Reset();
  if (Bof_IsAPowerOf2(_AligmentInByte_U32))
  {
    Rts_E = BOF_ERR_ENOMEM;

    _rAllocatedBuffer_X.Deleter_E = BOF_BUFFER_DELETER_ALIGNED_FREE;
    _rAllocatedBuffer_X.Capacity_U64 = _SizeInByte_U32;
    _rAllocatedBuffer_X.Size_U64 = 0;
    _rAllocatedBuffer_X.pData_U8 = nullptr;
    AllocateBuffer_X.AllocateZone_E = _AllocateZone_E;
    switch (_AllocateZone_E)
    {
      default:
      case BOF_BUFFER_ALLOCATE_ZONE::BOF_BUFFER_ALLOCATE_ZONE_RAM:
#if defined(_WIN32)
        _rAllocatedBuffer_X.pData_U8 = reinterpret_cast<uint8_t *>(_aligned_malloc(_SizeInByte_U32, _AligmentInByte_U32)); // malloc(size);   // TODO pChannel->getBoard()->getNUMANode() !!!
#else
        _rAllocatedBuffer_X.pData_U8 = reinterpret_cast<uint8_t *>(aligned_alloc(_AligmentInByte_U32, _SizeInByte_U32)); // malloc(size);   // TODO pChannel->getBoard()->getNUMANode() !!!
#endif
        break;

        /*
         * On a NUMA platform, the kernel will attempt to distribute the huge page pool
  over all the set of allowed nodes specified by the NUMA memory policy of the
  task that modifies nr_hugepages. https://www.kernel.org/doc/Documentation/vm/hugetlbpage.txt
         */
      case BOF_BUFFER_ALLOCATE_ZONE::BOF_BUFFER_ALLOCATE_ZONE_HUGE_PAGE:
#if defined(_WIN32)
#else
        void *pBuffer;

        S_HugePageId++;
        snprintf(AllocateBuffer_X.pHugePath_c, sizeof(AllocateBuffer_X.pHugePath_c), BOF_HUGE_PAGE_PATH, S_HugePageId.load());
        AllocateBuffer_X.Io_i = open(AllocateBuffer_X.pHugePath_c, O_CREAT | O_RDWR, 0755);
        if (AllocateBuffer_X.Io_i >= 0)
        {
          //          BOFERR Bof_OpenSharedMemory(const std::string &_rName_S, uint32_t _SizeInByte_U32, BOF_SHARED_MEMORY &_rSharedMemory_X)
          pBuffer = ::mmap(0, _SizeInByte_U32 < BOF_HUGE_PAGE_SIZE ? BOF_HUGE_PAGE_SIZE : _SizeInByte_U32, PROT_READ | PROT_WRITE, MAP_SHARED, AllocateBuffer_X.Io_i, 0);
          if (pBuffer == MAP_FAILED)
          {
            ::close(AllocateBuffer_X.Io_i);
            unlink(AllocateBuffer_X.pHugePath_c);
          }
          else
          {
            _rAllocatedBuffer_X.pData_U8 = reinterpret_cast<uint8_t *>(pBuffer);
          }
        }
#endif
        break;
    }
    if (_rAllocatedBuffer_X.pData_U8)
    {
      _rAllocatedBuffer_X.pUser = new BOF_BUFFER_ALLOCATE_HEADER;
      if (_rAllocatedBuffer_X.pUser)
      {
        Rts_E = BOF_ERR_NO_ERROR;
        *reinterpret_cast<BOF_BUFFER_ALLOCATE_HEADER *>(_rAllocatedBuffer_X.pUser) = AllocateBuffer_X;
        if (_LockIt_B)
        {
          Rts_E = Bof_LockMem(_OsAdvice_i, _SizeInByte_U32, _rAllocatedBuffer_X.pData_U8);
          if (Rts_E != BOF_ERR_NO_ERROR)
          {
            Bof_AlignedMemFree(_rAllocatedBuffer_X);
          }
        }
        else
        {
#if defined(_WIN32)
#else
          if (_OsAdvice_i)
          {
            Rts_E = (madvise(_rAllocatedBuffer_X.pData_U8, _SizeInByte_U32, _OsAdvice_i) == 0) ? BOF_ERR_NO_ERROR : BOF_ERR_SET;
          }
#endif
        }
        if (Rts_E == BOF_ERR_NO_ERROR)
        {
          if (_ClearIt_B)
          {
            memset(_rAllocatedBuffer_X.pData_U8, 0, _SizeInByte_U32);
          }
        }
      }
      else
      {
        _rAllocatedBuffer_X.Deleter_E = BOF_BUFFER_DELETER_NONE;
        _rAllocatedBuffer_X.pUser = &AllocateBuffer_X;
        Bof_AlignedMemFree(_rAllocatedBuffer_X);
      }
    }
  }
  // printf("=======> alloc Zone %d Must %d handle %x:%p data %x:%p\n",AllocateBuffer_X.AllocateZone_E, _rAllocatedBuffer_X.MustBeDeleted_B, sizeof(BOF_BUFFER_ALLOCATE_HEADER), _rAllocatedBuffer_X.pUser, _SizeInByte_U32, _rAllocatedBuffer_X.pData_U8);
  return Rts_E;
}

BOFERR Bof_AlignedMemFree(BOF_BUFFER &_rBuffer_X)
{
  BOF_BUFFER_ALLOCATE_HEADER *pAllocateBuffer_X;
  BOFERR Rts_E = BOF_ERR_EINVAL;

  pAllocateBuffer_X = reinterpret_cast<BOF_BUFFER_ALLOCATE_HEADER *>(_rBuffer_X.pUser);
  if (pAllocateBuffer_X)
  {
    Rts_E = BOF_ERR_NO_ERROR;
    if (pAllocateBuffer_X->Locked_B)
    {
      Rts_E = Bof_UnlockMem(_rBuffer_X.Capacity_U64, _rBuffer_X.pData_U8);
    }
    switch (pAllocateBuffer_X->AllocateZone_E)
    {
      default:
      case BOF_BUFFER_ALLOCATE_ZONE::BOF_BUFFER_ALLOCATE_ZONE_RAM:
#if defined(_WIN32)
        _aligned_free(_rBuffer_X.pData_U8);
        //			VirtualFree(_rBuffer_X.pData_U8, _rBuffer_X.Capacity_U64, MEM_RELEASE);
#else
        free(_rBuffer_X.pData_U8);
#endif
        break;

      case BOF_BUFFER_ALLOCATE_ZONE::BOF_BUFFER_ALLOCATE_ZONE_HUGE_PAGE:
#if defined(_WIN32)
#else
        ::munmap(_rBuffer_X.pData_U8, _rBuffer_X.Size_U64);
        ::close(pAllocateBuffer_X->Io_i);
        unlink(pAllocateBuffer_X->pHugePath_c);
#endif
        break;
    }
    //	printf("=======> DELETE Zone %d Must %d handle %x:%p data %lx:%p\n",pAllocateBuffer_X->AllocateZone_E, _rBuffer_X.MustBeDeleted_B, sizeof(BOF_BUFFER_ALLOCATE_HEADER), _rBuffer_X.pUser, _rBuffer_X.SizeInByte_U64, _rBuffer_X.pData_U8);

    _rBuffer_X.Deleter_E = BOF_BUFFER_DELETER_NONE; // Done by free or _aligned_free
    BOF_SAFE_DELETE(pAllocateBuffer_X);
    _rBuffer_X.Reset();
  }
  return Rts_E;
}
END_BOF_NAMESPACE()