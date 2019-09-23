/*
 * bufpool.hpp
 *
 *  Created on: 30-July-2019
 *      Author: bineesh
 */

/*
 * Allocates buffer from pool instead doing it on heap.
 * Returns shared pointer to buffer
 */

#ifndef _BUFPOOL_HPP_
#define _BUFPOOL_HPP_

#include <algorithm>
#include <array>
#include <memory>
#include <mutex>

#include "external/MemoryPool/C-11/MemoryPool.h"

namespace utils {

/* Create a buffer pool
 *
 * @elem type of underlying data
 * @elem_size size of single buffer element
 * @block_size size of one page in pool. It should at least hold two buffers
 */
template<class elem, size_t elem_size = 1, size_t block_size = 4096>
class bufpool
{
  typedef std::array<elem, elem_size> buf_t;

  std::mutex m_mutex;
  MemoryPool<buf_t, block_size> m_pool;

  bufpool(const bufpool&) = delete;
  bufpool& operator=(const bufpool&) = delete;

public:
  bufpool(){};

  /*
   * Returns shared pointer to type.
   */
  std::shared_ptr<elem> alloc()
  {
    std::unique_lock<std::mutex> lock(m_mutex);
    buf_t* ptr = m_pool.allocate();
    std::fill(ptr->begin(), ptr->end(), 0);
    return std::shared_ptr<elem>(ptr->data(), [this, ptr](elem*) {
      std::unique_lock<std::mutex> lock(this->m_mutex);
      this->m_pool.deallocate(ptr);
    });
  }
};

}

#endif /* _BUFPOOL_HPP_ */
