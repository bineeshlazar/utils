/*
 * mempool.hpp
 *
 *  Created on: 02-Jun-2019
 *      Author: bineesh
 */

/*
 * Allocates memory from pool instead doing it on heap.
 * Returns shared pointer to type
 */

#ifndef MEMPOOL_HPP_
#define MEMPOOL_HPP_

#include <mutex>
#include <memory>
#include "external/MemoryPool/C-11/MemoryPool.h"

namespace utils {

template <class T, size_t BlockSize = 4096>
class mempool {

    std::mutex m_mutex;
    MemoryPool<T, BlockSize> m_pool;

public:

    /*
     * Returns shared pointer to type.
     * Pass constructor arguments of type
     */
     template <class... Args> std::shared_ptr<T> alloc(Args&&... args)
     {
         std::unique_lock<std::mutex> lock(m_mutex);
         T *ptr = m_pool.newElement(std::forward<Args>(args)...);
         return std::shared_ptr<T>(ptr, [this](T *t){
             std::unique_lock<std::mutex> lock(this->m_mutex);
             this->m_pool.deleteElement(t);
             });
     }
};

}

#endif /* MEMPOOL_HPP_ */
