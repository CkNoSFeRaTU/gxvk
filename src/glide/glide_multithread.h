#pragma once

#include "glide_include.h"

namespace dxvk {

  class GlideMultithread;

  /**
   * \brief Scoped device lock
   */
  class GlideDeviceLock {

  public:

    GlideDeviceLock() { }

    GlideDeviceLock(GlideMultithread& mutex)
    : m_mutex(&mutex) { }

    GlideDeviceLock(GlideDeviceLock&& other)
    : m_mutex(other.m_mutex) {
      other.m_mutex = nullptr;
    }

    GlideDeviceLock& operator = (GlideDeviceLock&& other) {
      Unlock();

      m_mutex = other.m_mutex;
      other.m_mutex = nullptr;
      return *this;
    }

    ~GlideDeviceLock() {
      Unlock();
    }

  private:

    GlideMultithread* m_mutex = nullptr;

    void Unlock();

  };


  /**
   * \brief Glide context lock
   */
  class GlideMultithread {
    static constexpr uint32_t InvalidTid = -1u;

    friend GlideDeviceLock;
  public:

    GlideMultithread(FxBool Protected)
    : m_protected(Protected) { }

    /**
     * \brief Acquires lock
     *
     * If the calling thread already owns the lock, this will return
     * an empty lock guard and rely entirely on proper scoping.
     * \returns Lock guard
     */
    GlideDeviceLock AcquireLock() {
      if (likely(!m_protected))
        return GlideDeviceLock();

      uint32_t expected = 0u;
      uint32_t threadId = dxvk::this_thread::get_id();

      if (likely(m_owner.compare_exchange_weak(expected, threadId, std::memory_order_acquire)))
        return GlideDeviceLock(*this);

      if (expected == threadId)
        return GlideDeviceLock();

      return LockContested(threadId);
    }

  private:

    alignas(CACHE_LINE_SIZE)
    std::atomic<uint32_t>   m_owner     = { 0u };
    BOOL                    m_protected = false;

    GlideDeviceLock LockContested(uint32_t threadId);

    void Unlock() {
      m_owner.store(0u, std::memory_order_release);
    }

  };

  inline void GlideDeviceLock::Unlock() {
    if (m_mutex)
      m_mutex->Unlock();
  }

}
