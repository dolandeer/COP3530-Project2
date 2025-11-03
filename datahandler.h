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

public:
    dataHandler() {
        std::cerr << "Bad Initialization!!";
    }
    dataHandler(NOAAData* noaa, USCData* usc, weightedTrie* trie) {
        this->noaadata = noaa;
        this->uscdata = usc;
        this->trie = trie;
        //read all csv
        usc->readCSV();
        usc->initTrie();
        std::cout << numWords() << std::endl;
        noaadata->readAllCSV();
        noaadata->printAllData();
    }
    //datahandler functions
    std::vector<NOAAData::weatherRecord> getCitySevereEvents(std::string CityState) {
        if (trieSearch(CityState) == nullptr) return {};
        std::string CountyState = uscdata->cityToCounty(CityState);
        if (CountyState.empty()) return {};
        std::vector<NOAAData::weatherRecord> output = noaadata->getCountySevereEvents(CountyState);
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

    //essential minHeap functions
};


#endif //COP3530_PROJECT2_DATAHANDLER_H