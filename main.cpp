#include <iostream>
#include <cassert>
#include <vector>
#include <map>

#include "TwoQ.hpp"
#include "LRU.hpp"

int slow_get_page_int(int key) { return key; }

void Cache_Comparison() {
    for (int i = 0; i < 10; i++) { //10 tests

        int hits_2q = 0, hits_lru = 0, hits_ideal = 0;
        TQ_cache<int> example_2q(20);
        LRU_cache<int> example_lru(20);
        std::vector<int> mas;

        for (int i = 0; i < 200; ++i) {
            int q = 1 + rand() % 180;
            mas.push_back(q); //generating 200 random requests from 180 pages
            if (example_2q.lookup_update(q, slow_get_page_int))
                hits_2q += 1; //hits with 2q cache
            if (example_lru.lookup_update(q, slow_get_page_int))
                hits_lru += 1; //hits with lru cache
        }

        //finding the most frequently requested pages for ideal cache
        std::sort(mas.begin(), mas.end());

        std::map<int, int> numcnt;
        for (int i = 0; i < mas.size(); i++) {
            numcnt[mas[i]]++;
        }

        std::vector<std::pair<int, int>> v(numcnt.begin(), numcnt.end());
        std::sort(v.begin(), v.end(), 
        [](const std::pair<int, int>& e1, const std::pair<int, int>& e2) {
            return e1.second > e2.second;
        });

        for(size_t i = 0; i < v.size() && i < 20; i++) {
            hits_ideal += (v[i].second - 1); //number of repetitions of |cache|-
        }

        std::cout << "TOTAL HITS: " << hits_2q << " (lru: " << hits_lru << ", ideal:" << hits_ideal << ")\n"; 
    }
    
}

int main() {
    Cache_Comparison();
    int hits = 0;
    int n;
    size_t m;

    std::cin >> m >> n;
    assert(std::cin.good());
    TQ_cache<int> c(m);

    for (int i = 0; i < n; ++i) {
        int q;
        std::cin >> q;
        assert(std::cin.good());
        if (c.lookup_update(q, slow_get_page_int))
        hits += 1;
    }
    std::cout << hits << std::endl;
    return 0;
}