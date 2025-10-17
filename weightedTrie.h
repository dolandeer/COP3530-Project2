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