// Requirement #1, make a Trie where each letter node has a "weight" that increases by 1
// each time a word is inserted containing that letter.
// This will be used to make an autocomplete in the search bar.
#pragma once
#include <vector>


class trieNode {
private:
    int weight = 0;
    std::vector<trieNode*> children;
    char letter = '';

public:
    //constructor
    trieNode() = default;
    trieNode(char letterIn) {
        this->letter = letterIn;
        this->weight = 1;
    }

};


/*
populate the trie with the USCities csv
then increase the weight of each node depending on if it appears within the noaa database
(important!!) the noaa database uses prefixes on counties such as "coastal" and "southern",
the best fix I can think of for this is either ignore keywords such as this in the
data collection process or see if any part of the "cz" is already in the trie and insert that part
the second option may be more efficient, im not sure if it will work
keywords ive noticed: coastal, inland, western/eastern/northern/southern
some of the data also uses a region or landmark instead of a county, we can ignore those as they are
usually duplicates and would not match up to any city
*/
class weightedTrie {
private:
    trieNode rootNode;
    trieNode* root = &rootNode; // root is always empty

public:
    //constructors
    weightedTrie() = default;
    ~weightedTrie() = default; // destructor

    //functions
    trieNode* getRoot() {
        return this->root;
    }

    bool insert(const std::string& word) {
        return false;
    }
};