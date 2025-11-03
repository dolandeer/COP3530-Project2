#ifndef COP3530_PROJECT2_DATAHANDLER_H
#define COP3530_PROJECT2_DATAHANDLER_H

#include "parse.h"
#include "weightedTrie.h"
#include "minHeap"


// this exists to improve readability and data access within main
// the parse.h file is almost impossible to understand (sorry), in the interest of time making this
// as the "hub" for all operations is the simplest

class dataHandler {
    NOAAData* noaadata = nullptr;
    USCData* uscdata = nullptr;
    weightedTrie* trie = nullptr;
    MinHeap* minheap = nullptr;

public:
    dataHandler() {
        std::cerr << "Bad Initialization!!";
    }
    dataHandler(NOAAData* noaa, USCData* usc, weightedTrie* trie, MinHeap* heap) {
        this->noaadata = noaa;
        this->uscdata = usc;
        this->trie = trie;
        this->minheap = heap;
        if (!noaadata||!uscdata||!this->trie||!minheap){throw runtime_error("Bad pointers!");}
        usc->readCSV();
        usc->initTrie();
        std::cout << numWords() << std::endl;
        noaadata->readAllCSV();
        initHeap();
    }
    //datahandler functions
    std::vector<NOAAData::weatherRecord> getCitySevereEvents(std::string CityState) {
        std::transform(CityState.begin(), CityState.end(), CityState.begin(), ::tolower);
        if (trieSearch(CityState) == nullptr) return {};
        std::vector<NOAAData::weatherRecord> output = noaadata->getCountySevereEvents(CityState);
        std::cout << "getCitySevereEvents successful. found city : " << CityState <<std::endl;
        return output;
    }
    //essential weightedTrie functions
    trieNode* trieSearch(const std::string& word) {
        return trie->trieSearch(word);
    }
    std::string autocomplete(const std::string& word, const bool inverse = false) { // if inverse is true, then complete by LOWEST weight
        return trie->autocomplete(word, inverse);
    }
    trieNode* trieInsert(const std::string& word) {
        return trie->insertWord(word);
    }
    int numWords() {return trie->getRoot()->getWeight();}
    //essential USCData functions
    void iterateUSCMap() { // 31184 cities according to this dataset... google says that's incorrect
        //my console cant display them all but maybe yours can
        uscdata->iterateMap();
    }
    void initUSC() {
        uscdata->readCSV();
        uscdata->initTrie();
        std::cout << numWords() << std::endl;
    }
    //essential NOAA functions
    void printWeatherEventMap() {
        noaadata->printAllData();
    }
    std::unordered_map<std::string, std::vector<NOAAData::weatherRecord>> getWeatherEventMap() {
        return noaadata->getWeatherEventMap();
    }
    void printWeatherEventState(std::string state) {
        noaadata->printStateData(state);
    }
    //essential minHeap functions
    void initHeap() {
        std::unordered_map<std::string, std::vector<NOAAData::weatherRecord>> eventMap = getWeatherEventMap();
        std::unordered_map<std::string, std::vector<NOAAData::weatherRecord>>::iterator it;
        for (it = eventMap.begin(); it != eventMap.end(); it++) {
            std::string key = it->first;
            int weight = it->second.size();
            if (weight == 0) continue; //if weight is 0 we can assume it was skipped during data collection
            minheap->insert(key, weight);
        }
    }
    int heapSize() {
        return minheap->size();
    }
    int getCityWeightInHeap(const string &cityName) {
        return minheap->getWeight(cityName);
    }
    void printTopHeapNode() {
        auto min = minheap->getMin();
        std::cout << min.cityName << " : " << min.weight;
    }
};


#endif //COP3530_PROJECT2_DATAHANDLER_H