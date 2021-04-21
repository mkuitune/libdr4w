#pragma once

#include <vector>
#include <list>
#include <stdint.h>

namespace dr4 {
	using namespace std;


	template<class T, int TYPEID>
	class HandleBuffer {
	public:
		static const uint16_t TAG_UNUSED = 0;
		static const uint16_t TAG_TO_BE_COLLECTED = 1;
	public:
		struct Handle {
			static const uint64_t INVALID = 0xffffffff;

			uint32_t index : 32;  // index to buffer - 4.2G object indices
			uint16_t typeId : 10; //  type
			uint16_t tag : 16;  // pseudo-unique marker to make sure that the entity in index is the same as requested
			uint8_t meta : 6;    //  usable for domain specific stuff if needed

			struct TagGenerator {
				uint16_t seed = 2;
				// tag 0 means 'unused'
				// tag 1 means 'marked for collection'
				uint16_t next() {
					uint16_t v = seed++;
					if (v == TAG_UNUSED) {
						seed = TAG_TO_BE_COLLECTED + 1;
						v = seed++;
					}
					return v;
				}
			};

			uint16_t getTag() const { return tag; }
			uint8_t getMeta() const { return meta; }
			uint16_t getType() const { return typeId; }
			uint32_t getIndex() const { return index; }

			void setMeta(uint8_t m) { meta = m; }

			bool equal(const Handle& h) const {
				// meta reserved for domain application specific use
				return index == h.index && typeId == h.typeId && tag == h.tag;
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

			bool isInvalid() {
				return equal(INVALID);
			}

			static Handle fromUint(uint64_t h) {
				return *((Handle*)(&h));
			}

			static Handle Create(uint16_t type, uint16_t tag, uint32_t index) {
				Handle h;
				h.index = index;
				h.typeId = type;
				h.tag = tag;
				h.meta = 0;
			}

			static Handle Create(uint16_t type, uint16_t tag, uint32_t index, uint8_t meta) {
				Handle h;
				h.index = index;
				h.typeId = type;
				h.tag = tag;
				h.meta = meta;
			}

			static Handle Invalid() {
				return fromUint(INVALID);
			}

		};
	private:
		typedef uint16_t tag_t;

		struct storage_type_t {
			T data;
			Handle handle;

			//bool tagsMatch(Handle h) {
			//	return handle.tag == h.tag;
			//}

			bool hasSameHandle(Handle h) const{return handle.e}

			bool markForCollection() {
				tag = TAG_TO_BE_COLLECTED;// this invalidates the handle
			}

			bool isMarkedForCollection() const{
				return tag == TAG_TO_BE_COLLECTED;
			}

			void callDeallocator() {
				data.~T();
				tag = TAG_UNUSED;
			}

			template<typename ... Args>
			void callConstructor() {
				void* addr =(void*) (&data);
				new(addr)T(std::forward<Args>(args)...);
			}

			void callConstructor() {
				void* addr =(void*) (&data);
				new(addr)T();
			}
		};


		struct storage_t {
			storage_type_t* data = 0;
			const uint16_t typeInfo;
			const size_t size = 0;
			const uint32_t idxOffset;
			size_t lastUnused;

			std::list<size_t> freed;

			Handle::TagGenerator m_tags;

			storage_t(uint16_t type, uint32_t bufferSize, uint32_t offset)
				: typeInfo(type),size(bufferSize), idxOffset(offset), lastUnused(0) {
				data = malloc(sizeof(storage_type_t) * size);
				memset(data, 0, sizeof(storage_type_t) * size);
			}
		
			// no deconstructor, the idea is that the move operation will deal with this
			void freeAll(){
				if (!data)
					return;

				storage_type_t* last = data + size;
				for (storage_type_t* s = data; fst != last; fst++) {
					if (s->tag != 0)
						s->callDeallocator();
				}
				free(data);
			}

			bool isFull() const {
				lastUnused == size && freed.empty();
			}

			size_t actualIndex(Handle h) const{
				return h.index - idxOffset;

			}

			storage_type_t* getStorage(Handle h) {
				uint32_t fullIdx = h.index;

				if (fullIdx < idxOffset)
					return nullptr;

				size_t actual = fullIdx - idxOffset;

				if (actual > data.size())
					return nullptr;

				storage_type_t* stored = &data[actual];

				if (!stored->tagsMatch(h))
					return nullptr;

				return &stored;
			}

			T* get(Handle h) {
				storate_type_t* stored = getStorage(h);
				if (stored)
					return &stored->data;
				else
					return nullptr;
			}

			bool deallocate(Handle h) {
				auto s = getStorage(h);
				if (!s) 
					return false;

				freed.push_back(actualIndex(h));
				s->deallocate();
				return true;
			}

			bool markForCollection(Handle h) {
				auto s = getStorage(h);
				if (!s)
					return false;

				s->markForCollection();
				return true;
			}

			void collect() {
				for (size_t i = 0; i < size; i++) {
					if (data[i].isMarkedForColletion()) {
						data[i].callDeallocator();
						freed.push_back(i) :
					}
				}
			}

			storage_type_t* storeNextEmpty(uint32_t& fullIdx) {
				if (isFull())
					return nullptr;

				uint32_t idx;
				if (!freed.empty()){
					idx = freed.back();
					freed.pop_back();
				}
				else {
					idx = lastUnused++;
				}

				fullIdx = idxOffset + idx;
				return &data[idx];
			}
		
			// call constructor in calling function
			storage_type_t* allocNew(Handle& out) {
				uint32_t fullIdx;
				auto stored = storeNextEmpty(fullIdx);
				if (!stored) {
					return nullptr;
				}
				out = Handle::Create(typeInfo, stored->tag, fullIdx);
				//stored->callConstructor();
				//Handle h = Handle::Create(typeInfo, stored->tag, fullIdx);
				return stored;
			}


			//storage_t* CreateNew(size_t size, uint32_t offset) {
			//	storage_t* ptr = new storage_t(size, offset);
			//	return ptr;
			//}

		}; // Storage_t (single chunk) ends

		const uint16_t m_type;// give unique type-id on generation
		const size_t m_bufsize;

		std::vector<storage_t> m_storage;

		uint64_t m_nFailedReleased = 0;
		uint64_t m_nFailedAccesses = 0;

		bool recordFailedRelease() {
			m_nFailedReleased++;
			return false;
		}

		bool recordFailedAccess() {
			m_nFailedAccessses++;
			return false;
		}

		storage_type_t* getFreeSlot(Handle& h) {
			
			for (auto& s : m_storage) {
				if (!s.isFull()) {
					return s.allocNew(h);
				}
			}
			size_t newOffset = m_storage.size() * m_bufferSize;
			m_storage.emplace_back(type, m_bufferSize, newOffset);
			auto& s = m_storage.back();
			return s.allocNew(h);
		}

		T* getStoredForHandle(Handle h) {
			uint32_t idx = h.index / m_bufferSize;
			if (idx = > m_storage.size())
				return nullptr;
			return	m_storage[idx].get(h);
		}

	public:
		HandleBuffer(uint16_t type, size_t bufsize) :m_type(type), m_bufsize(bufsize) {
			m_storage.push_back(storage_t::Create(m_bufsize));
		}
		~HandleBuffer() {
			for (auto& stor : m_storage) {
				stor.freeAll();
			}
		}
// TODO
		Handle push(const T& value) {
			Handle h;
			T* slot;
			getFreeSlot(h, &slot);
			*slot = value;
			return h;
		}

		Handle create() {
			Handle h;
			T* slot;
			getFreeSlot(h, &slot);
			*slot = value;
			new(*slot) T();
			return h;
		}

		Handle push(T&& value) {
			Handle h;
			T* slot;
			getFreeSlot(h, &slot);
			new(*slot) T(std::move(value));
			return h;
		}

		bool isValid(Handle& h) {
			if (h.typeId != m_type)
				return false;
		}

		bool release(Handle& h) {
			storage_type_t* stored = getStoredForIndex(h.index);
			if (!stored) {
				return recordFailedRelease();
			}

			if (!storageIsForHandle(*stored, h)) {
				return recordFailedRelease();
			}

			stored->first = 0; // reset tag
			stored->second->~T(); // destructor

			return true;
		}


	};
}
