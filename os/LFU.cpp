#include <iostream>
#include <unordered_map>
#include <list>

using namespace std;

struct myLFU{
public:
    explicit myLFU(int capacity) : capacity_(capacity) {}

    int get(int key){
        if(!cache_.count(key)) return -1;

        int value = cache_[key].first;
        int freq = cache_[key].second;

        freq_[freq].erase(iter_[key]);
        freq_[freq+1].emplace_front(key);

        iter_[key] = freq_[freq+1].begin();

        if(freq_[freq].empty()) {
            freq_.erase(freq);
            if(freq == min_) min_++;
        }

        cache_[key].second ++;
        return value;
    }

    void put(int key, int value) {
        if(!cache_.count(key)) {
            if(cache_.size() == capacity_) {
                int temp = freq_[min_].back();
                freq_[min_].pop_back();
                iter_.erase(temp);
                cache_.erase(temp);
            }
            min_ = 1;
            freq_[min_].emplace_front(key);
            cache_[key] = make_pair(value, min_);
            iter_[key] = freq_[min_].begin();
        }
        else{
            cache_[key].first = value;
            get(key);
        }
    }

private:
    int min_;
    int capacity_;
    unordered_map<int , pair<int, int>> cache_;
    unordered_map<int, list<int>> freq_;
    unordered_map<int, list<int>::iterator> iter_;
};

int main() {
    myLFU cache(2);
    cache.put(1, 1);
    cache.put(2, 2);
    std::cout << cache.get(1) << std::endl;
    cache.put(3, 3);
    std::cout << cache.get(2) << std::endl;
    std::cout << cache.get(3) << std::endl;
    cache.put(4, 4);
    std::cout << cache.get(1) << std::endl;
    std::cout << cache.get(3) << std::endl;
    std::cout << cache.get(4) << std::endl;
    return 0;
}