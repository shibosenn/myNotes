#include <list>
#include <unordered_map>
#include <iostream>

using namespace std;

class myLRU{
public:

    explicit myLRU(int capacity) : capacity_(capacity) {}
    
    int get(int key){
        if(!cache_.count(key))
        {
            return -1;
        }
        auto it = cache_[key];
        int value = it->second;
        key_list_.erase(it);
        key_list_.emplace_front(make_pair(it->first, it->second));
        cache_[key] = key_list_.begin();
        return value;
    }

    void put(int key, int value) {
        if(!cache_.count(key)) {
            if(cache_.size() == capacity_) {
                int temp = key_list_.back().first;
                cache_.erase(temp);
                key_list_.pop_back();
            }
            key_list_.emplace_front(make_pair(key, value));
            cache_[key] = key_list_.begin();
        }
        else {
            auto it = cache_[key];
            key_list_.erase(it);
            it->second = value;
            key_list_.emplace_front(make_pair(it->first, it->second));
            cache_[key] = key_list_.begin();
        }
    }

private:
    unordered_map<int, list<pair<int, int>>::iterator> cache_;
    list<pair<int, int>> key_list_;
    int capacity_;
};

int main() {
    myLRU cache(2);
    cache.put(1, 1);
    cache.put(2, 2);
    std::cout << cache.get(1) << std::endl;
    cache.put(3, 3);
    std::cout << cache.get(2) << std::endl;
    cache.put(4, 4);
    std::cout << cache.get(1) << std::endl;
    std::cout << cache.get(3) << std::endl;
    std::cout << cache.get(4) << std::endl;

    return 0;
}