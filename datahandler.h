#ifndef COP3530_PROJECT2_DATAHANDLER_H
#define COP3530_PROJECT2_DATAHANDLER_H

#include "parse.h"
#include "weightedTrie.h"


// this exists to improve readability and data access within main

class dataHandler {
    NOAAData* noaadata = nullptr;
    USCData* uscdata = nullptr;
    weightedTrie* trie = nullptr;

public:
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
    //essential USCData functions
    void iterateUSCMap() {
        uscdata->iterateMap();
    }
};


#endif //COP3530_PROJECT2_DATAHANDLER_H