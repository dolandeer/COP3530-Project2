// Requirement #1, make a Trie where each letter node has a "weight" that increases by 1
// each time a word is inserted containing that letter.
// This will be used to make an autocomplete in the search bar.
#pragma once
#include <vector>


class trieNode {
private:
    int weight = 0;
    char letter = '\0';
    std::vector<trieNode*> children;

public:
    //constructor
    trieNode() = default;
    trieNode(char letterIn) {
        this->letter = letterIn;
        this->weight = 1;
        this->children;
    }
    //access functions
    std::vector<trieNode*> getChildren() const {
        return this->children;
    }
    char getLetter() const {
        return this->letter;
    }
    int getWeight() const {
        return this->weight;
    }
    //modifier functions
    void increaseWeight() {
        this->weight++;
    }
    void addChild(trieNode* node) {
        children.push_back(node);
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

    bool insertWord(const std::string& word) {

        return false;
    }

    const trieNode* findLetter(trieNode* node, char letter) {
        if (node == nullptr) return nullptr;
        if (node->getLetter() == letter) return node;
        for (auto child : node->getChildren()) {
            trieNode* l = findLetter(child, letter);
            if (l != nullptr) return l;
        }
        return nullptr;
    }

    bool findWord(const std::string& word) {

    }
};