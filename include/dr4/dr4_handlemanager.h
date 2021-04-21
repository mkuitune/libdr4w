#pragma once

#include <vector>
#include <list>
#include <stdint.h>
#include <mutex>

namespace dr4 {
	using namespace std;

	template<class T>
	class HandleBuffer {
	public:
		static const uint16_t TAG_UNUSED = 0;
		static const uint16_t TAG_TO_BE_COLLECTED = 1;
	public:
		struct Handle {
			uint32_t index : 32;  // index to buffer - 4.2G object indices
			uint16_t tag : 16;  // pseudo-unique marker to make sure that the entity in index is the same as requested
			uint16_t meta : 16;    //  usable for domain specific stuff if needed

			uint16_t getTag() const { return tag; }
			uint8_t getMeta() const { return meta; }
			uint32_t getIndex() const { return index; }

			void setMeta(uint8_t m) { meta = m; }

			bool equal(const Handle& h) const {
				// meta reserved for domain application specific use
				return index == h.index && tag == h.tag;
			}

			bool equal(uint64_t i)const {
				return equal(*((Handle*)&i));
			}

			bool operator==(const Handle& h) const { return equal(h); }
			bool operator!=(const Handle& h) const { return !equal(h); }
			bool operator==(uint64_t h) const { return equal(h); }
			bool operator!=(uint64_t h) const { return !equal(h); }

			uint64_t toUint() const {
				return *((uint64_t*)(this));
			}

			static Handle fromUint(uint64_t h) {
				return *((Handle*)(&h));
			}

			static Handle Create(uint16_t tag, uint32_t index) {
				Handle h;
				h.index = index;
				h.tag = tag;
				h.meta = 0;
				return h;
			}

			static Handle Create(uint16_t tag, uint32_t index, uint8_t meta) {
				Handle h;
				h.index = index;
				h.tag = tag;
				h.meta = meta;
				return h;
			}

			static Handle Invalid() {
				return { 0xffffffff,TAG_UNUSED,0};
			}

		};

		struct TagGenerator {
			uint16_t seed = 2;
			uint16_t next() {
				uint16_t v = seed++;
				if (v == TAG_UNUSED) {
					seed = TAG_TO_BE_COLLECTED + 1;
					v = seed++;
				}
				return v;
			}
		};
	private:
		typedef uint16_t tag_t;

		struct storage_type_t {
			T data;
			Handle handle;

			bool hasSameHandle(Handle h) const { return handle.equal(h); }

			void markForCollection() {
				handle = Handle::Invalid();
				handle.tag = TAG_TO_BE_COLLECTED;// this invalidates the handle
			}

			bool isMarkedForCollection() const {
				return handle.tag == TAG_TO_BE_COLLECTED;
			}

			void callDeallocator() {
				data.~T();
				memset(&data, 0, sizeof(data));
				handle = Handle::Invalid();
			}

			template<typename ... Args>
			void callConstructor(Args&&...args) {
				void* addr = (void*)(&data);
				new(addr)T(std::forward<Args>(args)...);
			}

			void callConstructor() {
				void* addr = (void*)(&data);
				new(addr)T();
			}
		};


		struct storage_t {
			storage_type_t* databuffer;
			const size_t size = 0;
			const uint32_t idxOffset;
			uint32_t lastUnused;

			std::list<uint32_t> freed;

			TagGenerator m_tags;

			storage_t(uint32_t bufferSize, uint32_t offset)
				: size(bufferSize), idxOffset(offset), lastUnused(0) {
				 databuffer = (storage_type_t *)malloc(sizeof(storage_type_t) * size);

				if (databuffer == nullptr)
					return;

				memset(databuffer, 0, sizeof(storage_type_t) * size);
				for (storage_type_t* iter = databuffer; iter != databuffer + size; iter++) {
					iter->handle = Handle::Invalid();
				}
			}

			// no deconstructor, the idea is that the move operation will deal with this
			void freeAll() {
				if (!databuffer)
					return;

				storage_type_t* last = databuffer + size;
				for (storage_type_t* s = databuffer; s != last; s++) {
					if (s->handle.tag != TAG_UNUSED)
						s->callDeallocator();
				}
				free(databuffer);
			}

			bool isFull() const {
				return lastUnused >= size && freed.empty();
			}

			uint32_t actualIndex(Handle h) const {
				return h.index - idxOffset;
			}

			bool isInRange(Handle h) const {
				if (h.index < idxOffset)
					return false;
				uint32_t localStorageIndex = actualIndex(h);
				return localStorageIndex < size;
			}

			storage_type_t* getSlot(Handle h) {
				if (!isInRange(h))
					return nullptr;

				size_t actual = actualIndex(h);
				storage_type_t* stored = databuffer + actual;

				if (!stored->hasSameHandle(h))
					return nullptr;

				return stored;
			}

			T* get(Handle h) {
				storage_type_t* stored = getSlot(h);
				if (stored)
					return &stored->data;
				else
					return nullptr;
			}

			bool deallocate(Handle h) {
				auto s = getSlot(h);
				if (!s)
					return false;
				freed.push_back(actualIndex(h));
				s->callDeallocator();
				return true;
			}

			bool markForCollection(Handle h) {
				auto s = getSlot(h);
				if (!s)
					return false;

				s->markForCollection();
				return true;
			}

			void collect() {
				for (size_t i = 0; i < size; i++) {
					if (databuffer[i].isMarkedForColletion()) {
						databuffer[i].callDeallocator();
						freed.push_back(i);
					}
				}
			}

			// does not call constructor, need to call that in callsite
			storage_type_t* allocateStorage(Handle& handleOut) {
				if (isFull())
					return nullptr;

				uint32_t idx;
				if (!freed.empty()) {
					idx = freed.back();
					freed.pop_back();
				}
				else {
					idx = lastUnused++;
				}

				uint32_t fullIdx = idxOffset + idx;
				handleOut = Handle::Create(m_tags.next(), fullIdx);
				
				storage_type_t* stored = databuffer + idx;
				stored->handle = handleOut;

				return stored;
			}

		}; // Storage_t (single chunk) ends

		const uint32_t m_buffersize;

		std::vector<storage_t> m_storage;

		uint64_t m_nFailedReleased = 0;
		uint64_t m_nFailedAccesses = 0;

		std::mutex m_storage_mutex;

		bool recordFailedRelease() {
			m_nFailedReleased++;
			return false;
		}

		bool recordFailedAccess() {
			m_nFailedAccesses++;
			return false;
		}

		storage_type_t* getFreeSlot(Handle& h) {
			std::lock_guard<std::mutex> guard(m_storage_mutex);
			storage_type_t* stor;
			for (auto& s : m_storage) {
				stor = s.allocateStorage(h);
				if (stor != nullptr)
					return stor;
			}
			uint32_t newOffset = (uint32_t) m_storage.size() * m_buffersize;
			m_storage.emplace_back(m_buffersize, newOffset);
			auto& s = m_storage.back();

			return s.allocateStorage(h);
		}

		storage_t* getStorageOfHandle(Handle h) {
			std::lock_guard<std::mutex> guard(m_storage_mutex);
			uint32_t idx = h.index / m_buffersize;
			if (idx >= m_storage.size())
				return nullptr;
			return	&m_storage[idx];
		}

		storage_type_t* getSlotForHandle(Handle h) {
			auto storage = getStorageOfHandle(h);
			if (!storage)
				return nullptr;
			return storage->getSlot(h);
		}

		T* getStoredTypeForHandle(Handle h) {
			auto slot = getSlotForHandle(h);
			if (!slot) {
				return nullptr;
			}
			return &slot->data;
		}

	public:

		HandleBuffer(uint32_t bufsize) :m_buffersize(bufsize) {
			m_storage.reserve(32);
			m_storage.emplace_back(m_buffersize, (uint32_t)0);
		}

		~HandleBuffer() {
			for (auto& stor : m_storage) {
				stor.freeAll();
			}
		}
		
		Handle create() {
			Handle h;
			storage_type_t* storage = getFreeSlot(h);
			storage->callConstructor();
			return h;
		}

#if 0// TODO REMOVE or figure out way to parametrize usage of deallocator
		// does not call constructor
		Handle createPOD() {
			Handle h;
			storage_type_t* storage = getFreeSlot(h);
			return h;
		}
#endif
		
		template<typename ... Args>
		Handle create(Args&&...args)  {
			Handle h;
			storage_type_t* storage = getFreeSlot(h);
			storage->callConstructor(std::forward<Args>(args)...);
			return h;
		}

		T* get(Handle h) {
			return getStoredTypeForHandle(h);
		}

		// TODO Handle release, marking for collection and collection from
		// single thread. Store in managing thread a set of pinned handles.
		// client thread may issue 'please delete' request, that the managing thread
		// then compares to the pinned handles and issues it or not
		// Prefer deletes only from master thread

		bool release(Handle h) {
			storage_t* storage = getStorageOfHandle(h);

			if (!storage) 
				return recordFailedAccess();

			if(!storage->deallocate(h))
				return recordFailedRelease();

			return true;
		}

		bool markForCollection(Handle& h) {
			storage_t* storage = getStorageOfHandle(h);

			if (!storage)
				return recordFailedAccess();

			if(!storage->markForCollection(h))
				return recordFailedRelease();

			return true;
		}
	};
}
