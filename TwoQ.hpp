#pragma once

#include <algorithm>
#include <exception>
#include <unordered_map>
#include <list>

/*---------------2Q Cache---------------
http://www.vldb.org/conf/1994/P439.PDF*/

template <typename V, typename K = int>
struct TQ_cache {
private:
    size_t cache_sz;
    size_t A1in_sz, A1out_sz, Am_sz;

    std::list<V> A1in, A1out, Am;

    using ListIt = typename std::list<V>::iterator;
    std::unordered_map<K, ListIt> A1in_hash, A1out_hash, Am_hash;
    
    bool A1in_full() { return (A1in.size() == A1in_sz); }
    bool A1out_full() { return (A1out.size() == A1out_sz); }
    bool Am_full() { return (Am.size() == Am_sz); }

public:
    TQ_cache(int sz) {
       if (sz <= 0) {
            throw std::exception(std::invalid_argument(""));
        }
        A1in_sz = (int) (sz * 0.25f);
        A1out_sz = (int) (sz * 0.5f);
        Am_sz = sz - A1out_sz - A1in_sz; //A1in is 25%, A1out is 50%, Am - 25%
    }

    template <typename F> bool lookup_update(K key, F slow_get_page) {
        //if there is X in cache
        auto hit = Am_hash.find(key);
        if (hit != Am_hash.end()) { //if X page is in Am then push it in front (LRU)
            auto eltit = hit->second;
            if (eltit != Am.begin()) {
                Am.splice(Am.begin(), Am, eltit, std::next(eltit));
            }
            return true;
        }
        hit = A1out_hash.find(key);
        if (hit != A1out_hash.end()) { // if X is in A1out then put it in AM
            if (Am_full()){
                Am_hash.erase(Am.back());
                Am.pop_back();
            }
            Am.push_front(*(hit->second));
            Am_hash[key] = Am.begin();
            A1out_hash.erase(key);
            A1out.erase(hit->second);

            return true;
        }
        hit = A1in_hash.find(key);
        if (hit != A1in_hash.end()) { //if X is in A1out then do nothing

            return true;
        }

        //if there is no X in cache
        if (A1in_full()) {
            if (A1out_full()) {
                A1out_hash.erase(A1out.back());
                A1out.pop_back(); //Move page from the tail A1out to trash
            }
            A1out.push_front(A1in.back());
            K k_tmp;
            for (auto it : A1in_hash) { //idk how else move elements from A1in_hash to A1out_hash
                if (*(it.second) == A1in.back()) {
                    k_tmp = it.first;
                    break;
                }
            }
            A1out_hash[k_tmp] = A1out.begin();
            A1in_hash.erase(A1in.back()); 
            A1in.pop_back(); //push page from the tail of A1in to front of A1out
        }
        A1in.push_front(slow_get_page(key));
        A1in_hash[key] = A1in.begin();

        return false;
    }

    void display() {
        std::cout << ">>>Cache<<<\nA1in (" << A1in_sz << "): ";
        for (auto it : A1in) {
            std::cout << it << " ";
        }
        std::cout << "\nA1in_hash: ";
        for (auto it : A1in_hash) {
            std::cout << it.first << "-" << *(it.second) << " ";
        }
        std::cout << "\nA1out (" << A1out_sz << "): ";
        for (auto it : A1out) {
            std::cout << it << " ";
        }
        std::cout << "\nA1out_hash: ";
        for (auto it : A1out_hash) {
            std::cout << it.first << "-" << *(it.second) << " ";
        }
        std::cout << "\nAm (" << Am_sz << "): ";
        for (auto it : Am) {
            std::cout << it << " ";
        } 
        std::cout << "\nAm_hash: ";
        for (auto it : Am_hash) {
            std::cout << it.first << "-" << *(it.second) << " ";
        }
        std::cout << "\n";     
    }
};
