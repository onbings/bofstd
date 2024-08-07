#pragma once

// #if USE_XALLOCATOR
#include "xallocator.h"
/// #endif

/// @brief A class to hold a platform-specific thread messsage that will be passed
/// through the OS message queue.
class ThreadMsg
{
  // #if USE_XALLOCATOR
  XALLOCATOR
  // #endif
public:
  /// Constructor
  /// @param[in] id - a unique identifier for the thread messsage
  /// @param[in] data - a pointer to the messsage data to be typecast
  ///		by the receiving task based on the id value.
  /// @pre The data pointer argument *must* be created on the heap.
  /// @port The destination thread will delete the heap allocated data once the
  ///		callback is complete.
  ThreadMsg(int32_t id, void *data) : m_id(id), m_data(data)
  {
  }

  int32_t GetId() const
  {
    return m_id;
  }
  void *GetData() const
  {
    return m_data;
  }

private:
  int32_t m_id;
  void *m_data;
};
