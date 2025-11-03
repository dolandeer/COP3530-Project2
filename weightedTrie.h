// Requirement #1, make a Trie where each letter node has a "weight" that increases by 1
// each time a word is inserted containing that letter.
// This will be used to make an autocomplete in the search bar.
#pragma once
#include <vector>


class trieNode {
private:
    int weight = 0;
    char letter = '\0';
    std::string CityState;
    std::string CountyState;
    //make each leaf node store the county of the city
    //every time a county appears increment each city in the county by 1

public:
    trieNode* children[28]; //0-25 are alphabetical, 26 is space
    bool isLeaf = false;
    //constructor
    trieNode() {
        for (int i = 0; i<28; i++) {
            children[i] = nullptr;
        }
    }
    trieNode(char key) {
        for (int i = 0; i<27; i++) {
            children[i] = nullptr;
        }
        this->letter = key;
    }
    //access functions
    char getLetter() const {return this->letter;}
    int getWeight() const {return this->weight;}
    int getNumChildren() {
        int counter = 0;
        for (int i = 0; i<28; i++) {if (this->children[i] != nullptr) counter++;}
        return counter;
    }
    std::string getCity() {
        return this->CityState;
    }
    std::string getCounty() {
        return this->CountyState;
    }
    void setCity(std::string word) {
        this->CityState = word;
    }
    void setCounty(std::string word) {
        this->CountyState = word;
    }
    //modifier functions
    void increaseWeight() {this->weight++;}
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
^^^ to ignore, if compareCity fails then simply do not insert the data anywhere
*/
class weightedTrie {
private:
    trieNode rootNode;
    trieNode* root = &rootNode; // root is always empty

public:
    //constructors
    weightedTrie() = default;
    ~weightedTrie() {
        deleteRecursive(this->root);
        //this->root = nullptr;
    }; // destructor

    //functions
    void deleteRecursive(trieNode* node) {
        if (node != nullptr) {
            for (int i = 0; i < 28; ++i) {
                if (node->children[i] != nullptr) {
                    deleteRecursive(node->children[i]);
                    node->children[i] = nullptr;
                }
            }
            if (node != root) delete (node);
        }
    }


    trieNode* insertWord(trieNode* root, const std::string& word) {
        trieNode* current = root;
        current->increaseWeight(); // the root will hold the number of words present within the trie
        for (char letter : word) {
            int index = indexLetter(letter);
            if (index == -1) continue;
            if (letter >= 'A' && letter <= 'Z'){letter = letter - 'A' + 'a';}
            if (current->children[index] == nullptr) {
                trieNode* newNode = new trieNode(letter);
                current->children[index] = newNode;
            }
            current = current->children[index];
            current->increaseWeight();
        }
        current->setCity(word);
        current->isLeaf = true; // if a node is marked as isLeaf, that means that it is the end of a word
        return current;
    }

    void setCounty(trieNode* node, std::string word) {
        node->setCounty(word);
    }

    trieNode* trieSearch(trieNode* root, const std::string& word) {
        trieNode* current = root;
        for (char letter : word) {
            int index = indexLetter(letter);
            if (index == -1) continue;
            //DEBUG:
            //std::cout << "WEIGHT OF " << current->getLetter() << ":" << current->getWeight() << std::endl;
            //
            if (current->children[index] == nullptr) return nullptr;
            current = current->children[index]; // does exist, continue down
        }
        //DEBUG:
        //std::cout << "WEIGHT OF " << current->getLetter() << ":" << current->getWeight() << std::endl;
        //
        if (current->isLeaf) return current;
        return nullptr;
    }

    std::string autocomplete(trieNode* root, const std::string& word, bool inverse) { // autocompletes
        trieNode* current = root;
        std::string output;
        for (char letter : word) {
            int index = indexLetter(letter);
            if (index == -1) continue;
            if (current->children[index] == nullptr){return "";} // does not exist
            current = current->children[index]; // does exist, continue down
            output += letter;
        }
        if (current->isLeaf) return output; // we have input a real word
        while (!current->isLeaf) {
            if (current->getNumChildren() == 0) return output;
            trieNode* target = nullptr;
            for (int i = 0; i<28; i++) {
                if (current->children[i] != nullptr) {
                    if (target==nullptr) target = current->children[i];
                    if (!inverse&&current->children[i]->getWeight() > target->getWeight()) target = current->children[i]; //prioritize HIGHER
                    if (inverse&&current->children[i]->getWeight() < target->getWeight()) target = current->children[i]; //prioritize LOWER
                }
            }
            current = target;
            if (current != nullptr) output += current->getLetter();
        }
        return output;
    }

    // WRAPPERS
    trieNode* insertWord(const std::string& word) {
        trieNode* root = this->getRoot();
        return insertWord(root, word);
    }

    trieNode* trieSearch(const std::string& word) {
        trieNode* root = this->getRoot();
        return trieSearch(root, word);
    }
    std::string autocomplete(const std::string& word, const bool inverse = false) { // if inverse is true, then complete by LOWEST weight
        trieNode* root = this->getRoot();
        return autocomplete(root, word, inverse);
    }

    // HELPERS
    int numWords() {return this->getRoot()->getWeight();}
    trieNode* getRoot() {return this->root;}

    int indexLetter(char letter) { //narrowing conversion from capital to lowercase
        if (letter >= 'A' && letter <= 'Z'){letter = letter - 'A' + 'a';}
        if (letter == ' ') return 26;
        if (letter == '/') return 27;
        if (letter >= 'a' && letter <= 'z') return letter - 'a';
        return -1;
    }
};