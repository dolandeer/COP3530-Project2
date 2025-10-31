#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include "weightedTrie.h"

//JSON Parsing (getting data from the API)
class JSONData {

};



//NOAA Parsing (treating ',,' as end of row and ignoring all data after)
//TODO remake the NOAAData readCSV AFTER implementing weightedTrie
class NOAAData {
private:


    // because of how many files we have, each file will be its own structure
    struct file {
        std::unordered_map<std::string, std::vector<std::string>> fileData;
        // {key = "simplified_cz_name/state" || values = [weather type, month]}
        // find simplified cz name on input, compare against already initialized trie
        // read uscities -> init trie -> read NOAAData (compare against trie for name) -> store NOAAData in min/max heap
        // api c
    };

    std::vector<file> data;  // file<{key = "simplified_cz_name/state" || values = [weather type, month]}>
    weightedTrie* trie = nullptr;



public:
    //CONSTRUCTORS AND DESTRUCTORS
    NOAAData() {
        std::cerr << "NOAAData initialized without a weighted trie!! Was this intentional?" << std::endl;
    }
    NOAAData(weightedTrie* inputTrie) {
        this->trie = inputTrie;
    }

    //HELPER FUNCTIONS
    weightedTrie* getTrie() const {return this->trie;}

    void printData() const {} // TODO helper function that prints data to console by row, should mirror database}


    //MAIN FUNCTIONS

    // parse a single CSV file, return format row<col<cell>>
    std::vector<std::vector<std::string>> readCSV(const std::string& filename) {
        std::vector<std::vector<std::string>> output;
        std::ifstream CSVFile("data/noaa_database/" + filename);
        if (!CSVFile.is_open()) {
            std::cerr << "Unable to open file: " << filename << std::endl;
            return output;
        }
        std::string line;
        while (std::getline(CSVFile, line)) {
            auto pos = line.find(",,");
            line = line.substr(0, pos); // ignore all data after ,,
            std::vector<std::string> row;
            std::stringstream ss(line);
            std::string cell;
            while (std::getline(ss, cell, ',')) {
                if (cell[0] == '"') { // removal of quotations
                    cell.replace(cell.begin(),cell.begin()+1,"");
                    cell.replace(cell.end()-1,cell.end(),"");
                }
                row.push_back(cell);
            }
            output.push_back(row);
        }
        CSVFile.close();
        return output;
    }

    bool compareCity(std::string cityName) { // CSV->Trie comparison function
        if (this->trie == nullptr) {
            std::cerr << "compareCity failed" << std::endl;
            return false;
        }

        if (trie->trieSearch(cityName)) return true;
        cityName=cityName.substr(cityName.find_first_of(" \t")+1); //remove prefix
        if (trie->trieSearch(cityName)) return true;

        //failed for some reason
        std::cerr << "compareCity failed" << std::endl;
        return false;
    }

    void insertData(){} //TODO this will insert info into data, make sure to compare against trie before

    void getData(){} //TODO will return data from the map

};



//USCities Parsing (use a for loop to ignore any unwanted data)
// we only really need this database for converting city name -> lat/long for calling the API
// and for weighted trie initialization
// store data as a map: {key = "city/state" || values = [county, lat, long]}
// for severe weather data, we can assume every city within a county experienced the same weather
// storing city and state as key to avoid duplicate cities such as charleston SC and charleston WV
class USCData {
    std::unordered_map<std::string, std::vector<std::string>> data; // {Orlando/Florida, <Orange,x,y,pop>}
    weightedTrie* trie = nullptr;

public:
    USCData(weightedTrie* inputTrie) {
        trie = inputTrie;
    }

    //parses CSV
    std::vector<std::vector<std::string>> readCSV() {
        std::vector<std::vector<std::string>> output;
        std::ifstream CSVFile("data/simplemaps_uscities/uscities.csv");
        if (!CSVFile.is_open()) {
            std::cerr << "Unable to open file" << std::endl;
            return output;
        }
        std::string line;
        while (std::getline(CSVFile, line)) {
            std::vector<std::string> row;
            std::stringstream ss(line);
            std::string cell;
            int count = 0;
            while (std::getline(ss, cell, ',')) {
                row.push_back(cell);
                count++;
                if (count == 9) break;
            }
            output.push_back(row);
        }
        CSVFile.close();
        insertData(output);
        return output;
    }

    //inserts output of readCSV into data
    void insertData(const std::vector<std::vector<std::string>>& input) {
        for (const auto& row : input) {
            int count = 0;
            std::string key;
            std::vector<std::string> value;
            for (auto cell : row) {
                cell.replace(cell.begin(),cell.begin()+1,"");
                cell.replace(cell.end()-1,cell.end(),"");
                if (count <= 3) { // city/state
                    if (count != 1 && count != 2) key += cell;
                    if (count == 0) key += "/";
                    count++;
                }
                else {
                    if (count != 4) value.push_back(cell);
                    count++;
                }
            }
            data[key] = value;
        }
    }

    //iterates data to verify its stored properly
    void iterateMap() { // helper function to ensure data is formatted correctly
        std::unordered_map<std::string, std::vector<std::string>>::iterator it;
        for (it = data.begin(); it != data.end(); it++)
        {
            std::cout << it->first    // string (key)
                      << " : ";
            for (const auto& i : it->second) {
                std::cout << i << ", ";
            }
            std::cout << std::endl;
        }
    }

    void initTrie() {
        if (trie == nullptr) return;
        std::unordered_map<std::string, std::vector<std::string>>::iterator it;
        for (it = data.begin(); it != data.end(); it++) {
            trie->insertWord(it->first);
        }
    }

    //TODO need getData function, map<string, vector<string>> is the format

    std::map<std::string, std::vector<std::string>> getData() {

    }
};