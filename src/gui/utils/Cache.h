//
// Created by TheDaChicken on 7/22/2025.
//

#ifndef CACHE_H
#define CACHE_H

#include <unordered_map>
#include <memory>
#include <cassert>
#include <list>

template<typename Key, typename Value>
class Cache
{
	public:
		Cache(const Cache &) = delete;
		Cache &operator=(const Cache &) = delete;
		Cache(Cache &&) = default;
		Cache &operator=(Cache &&) = default;

		explicit Cache(const std::size_t max_size = 100)
			: max_size_(max_size)
		{

		}

		void Clear()
		{
			item_list.clear();
			item_map.clear();
			curr_size_ = 0;
		}

		bool Contains(const Key &key) const
		{
			return item_map.find(key) != item_map.end();
		}

		std::shared_ptr<Value> Get(const Key &key)
		{
			auto it = item_map.find(key);
			if (it == item_map.end())
				return nullptr;

			item_list.splice(item_list.begin(), item_list, it->second);
			return it->second->second.value;
		}

		void Remove(const Key &key)
		{
			auto it = item_map.find(key);
			if (it != item_map.end())
			{
				curr_size_ -= it->second.size;
				item_list.remove(it->second);
				item_map.erase(it);
			}
		}

		template<typename... Args>
		std::pair<bool, std::shared_ptr<Value>> TryInsert(const Key &key, std::size_t size, std::shared_ptr<Value> value)
		{
			ReduceTo(max_size_ - size);

			auto it = item_map.find(key);
			if (it != item_map.end())
				return std::make_pair(false, it->second->second.value);

			item_list.emplace_front(
				key,
				CacheEntry{value, size}
			);
			item_map[key] = item_list.begin();
			curr_size_ += size;
			return std::make_pair(true, item_map[key]->second.value);
		}

		std::shared_ptr<Value> Take(const Key &key)
		{
			auto it = item_map.find(key);
			if (it == item_map.end())
				return nullptr;

			assert(it->second.second.value);

			auto entry = std::move(it->second.value);

			curr_size_ -= it->second.size;
			item_map.erase(it);
			item_list.erase(it->second);
			return entry;
		}

		void ReduceTo(const int size)
		{
			auto list_iter = item_list.end();

			while (curr_size_ > size)
			{
				--list_iter;

				if (list_iter == item_list.begin())
					break;

				auto &key = *list_iter;
				auto it = item_map.find(key.first);
				if (it == item_map.end())
					continue;

				auto &entry = it->second->second;
				if (entry.value)
				{
					if (!entry.value.unique())
						continue;

					curr_size_ -= entry.size;
				}

				item_map.erase(it);

				list_iter = item_list.erase(list_iter);
			}
		}

	private:
		struct CacheEntry
		{
			std::shared_ptr<Value> value;
			std::size_t size;
		};

		std::list<std::pair<Key, CacheEntry> > item_list;
		std::unordered_map<Key, decltype(item_list.begin())> item_map;

		std::size_t curr_size_ = 0;
		std::size_t max_size_;
};

#endif //CACHE_H

