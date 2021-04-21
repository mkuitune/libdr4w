#pragma once

#include <vector>
#include <list>
#include <stdint.h>
#include <mutex>
#include <memory>

#include <parallel_hashmap/phmap.h>

namespace dr4 {
	using namespace std;

	template<class KEY, class VAL>
	using HashMap = phmap::flat_hash_map<KEY, VAL>;

	template<class T>
	class SafeHandleBuffer {
	public:

		struct Handle {
			uint32_t handle;
			bool valid() const { return handle != 0; }
			static Handle Invalid() { return { 0 }; }
		};

	private:
		typedef uint32_t handle_t;

		typedef HashMap<handle_t, std::shared_ptr<T>> storage_t;

		storage_t m_storage;
		std::mutex m_storage_mutex;
		handle_t m_handle_seed = 1;

		// in the worst case scenario if we run out of 
		// ids it probably means lot of them have been erased already
		// instead of keeping a 'freed' list just collect unused numbers to this buffer
		static const s_fastsize = 1024;
		uint32_t tryFastStart = 1;
		handle_t m_reserveBuffer[m_fastsize];
		size_t m_fastused = 0;

		Handle newHandle() { 
			if (m_handle_seed < std::numeric_limits<uint32_t>::max()){
				return { m_handle_seed++ };
			}
			else {
				if (m_fastused == 0) {
					// 'rescue' indices from the missing indices of the hashmap
					while (m_fastused < m_fastsize) {
						if (m_storage.count(tryFastStart) == 0) {
							m_reserveBuffer[m_fastused++] = tryFastStart++;
						}
					}
				}
				m_fastused--;
				return { m_reserveBuffer[m_fastUsed] };
			}
		}

	public:	

		bool recordFailedRelease() {
			m_nFailedReleased++;
			return false;
		}

		bool recordFailedAccess() {
			m_nFailedAccesses++;
			return false;
		}

	public:

		SafeHandleBuffer(){}
		~SafeHandleBuffer() {}
		
		bool release(Handle handle) {
			std::lock_guard<std::mutex> guard(m_storage_mutex);

			handle_t h = handle.handle();
			auto iter = m_storage.find(h);
			if (iter == m_storage.end()) {
				recordFailedRelease();
				return false;
			}
			m_storage.erase(iter);
		}

		Handle create() {
			std::lock_guard<std::mutex> guard(m_storage_mutex);
			Handle h = newHandle();
			m_storage[h.handle()] = std::make_shared<T>(std::forward<Args>(args)...):
			return h;
		}
		
		template<typename ... Args>
		Handle create(Args&&...args)  {
			std::lock_guard<std::mutex> guard(m_storage_mutex);
			Handle h = newHandle();
			m_storage[h.handle()] = std::make_shared<T>(std::forward<Args>(args)...):
			return h;
		}

		std::weak_ptr<T> get(Handle h) {
			std::lock_guard<std::mutex> guard(m_storage_mutex);
			auto iter = m_storage.find(h.handle());
			if (iter == m_storage.end()) {
				recordFailedAccess();
				return nullptr;
			}
			return iter->second;
		}
	};

#define DECL_SAFE_HANDLE_STORAGE(type_param_, local_name_)\
    SafeHandleBuffer<type_param_> local_name_;\
    std::weak_ptr<type_param_> local_name_##Get(SafeHandleBuffer<type_param_>::Handle h){ return local_name_.get(h);} \
    bool local_name_##Delete(SafeHandleBuffer<type_param_>::Handle h){ return local_name_.release(h);} \
    SafeHandleBuffer<type_param_>::Handle local_name_##Create(){return local_name_.create();} \
	template<typename ... Args> \
	SafeHandleBuffer<type_param_>::Handle local_name_##Create(Args&&...args)  { return local_name_.create(std::forward<Args>(args)...);}


}
