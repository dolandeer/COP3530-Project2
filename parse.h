#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include "weightedTrie.h"
#include <filesystem>

#include "parse.h"

//JSON Parsing (getting data from the API)
class JSONData {

};


//USCities Parsing (use a for loop to ignore any unwanted data)
// we only really need this database for converting city name -> lat/long for calling the API
// and for weighted trie initialization
// store data as a map: {key = "city/state" || values = [county, lat, long]}
// for severe weather data, we can assume every city within a county experienced the same weather
// storing city and state as key to avoid duplicate cities such as charleston SC and charleston WV
class USCData {
    std::unordered_map<std::string, std::vector<std::string>> data; // {Orlando/Florida, <Orange,x,y,pop>}
    std::unordered_map<std::string, std::vector<trieNode*>> countyMap; // Orange/Florida : Orlando, Kissimmee, etc
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
        for (auto row : input) {
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
            for (auto i : it->second) {
                std::cout << i << ", ";
            }
            std::cout << std::endl;
        }
        for (auto it1 = countyMap.begin(); it1 != countyMap.end(); it1++)
        {
            std::cout << it1->first    // string (key)
                      << " : ";
            for (auto i : it1->second) {
                std::cout << i->getCityState() << ", ";
            }
            std::cout << std::endl;
        }
    }

    void initTrie() {
        //init trie with City/State as nodes, leaf node stores County/State
        if (trie == nullptr) return;
        std::unordered_map<std::string, std::vector<std::string>>::iterator it;
        for (it = data.begin(); it != data.end(); it++) {
            std::string county = it->second[0];
            std::string cityState = it->first;
            std::string countyUpper = county;
            std::transform(countyUpper.begin(), countyUpper.end(), countyUpper.begin(), ::toupper);
            std::string stateUpper = cityState.substr(cityState.find("/") + 1);
            std::transform(stateUpper.begin(), stateUpper.end(), stateUpper.begin(), ::toupper);
            std::string key = countyUpper + "/" + stateUpper;
            trieNode* curr = trie->insertWord(cityState);
            countyMap[key].push_back(curr);
        }
        //init map with {County/State, vector<trieNode* city> cities}
    }


    //getData will return either City/State or County/State
    //getCountyMap returns a vector with the trieNodes of all cities within a county
    std::vector<trieNode*> getCountyMap(const std::string& CountyState) const {
        auto it = countyMap.find(CountyState);
        if (it != countyMap.end()) {
            return it->second;
        }
        return {}; // not found
    }


    std::string cityToCounty(std::string CityState) {
        for (auto it : countyMap) {
            for (auto city : it.second) {
                if (city->getCityState() == CityState) return it.first;
            }
        }
        return "";
    }

};

//NOAA Parsing (treating ',,' as end of row and ignoring all data after)
class NOAAData {
public:
    struct weatherRecord {
        std::string eventType;
        int year = -1;
        int month = -1;
    };

private:
    std::unordered_map<std::string, std::vector<weatherRecord>> data;  // {key = "cityname/state" || values = vector<[weather type, year, month]>}
    weightedTrie* trie = nullptr;
    USCData* uscData = nullptr;



public:
    //CONSTRUCTORS AND DESTRUCTORS
    NOAAData() {
        std::cerr << "NOAAData initialized without a weighted trie!! Was this intentional?" << std::endl;
    }
    NOAAData(weightedTrie* inputTrie, USCData* usc) {
        this->trie = inputTrie;
        this->uscData = usc;
    }

    //HELPER FUNCTIONS
    weightedTrie* getTrie() const {return this->trie;}


    //MAIN FUNCTIONS

    // parse a single CSV file and insert it into our trusty map
    //END_YEARMONTH at col 4 : int (format "200012" = December of 2000)
    //event type at col 13 : string
    //cz_name at col 16 : string (NEEDS TO BE SIMPLFIED) done
    //STATE at col 9 : string
    //read only important cells!!
    void readCSV(const std::string& filename) {
        int column = 0;
        std::vector<std::vector<std::string>> output;
        std::ifstream CSVFile("data/noaa_database/" + filename);
        if (!CSVFile.is_open()) {
            std::cerr << "Unable to open file: " << filename << std::endl;
            return;
        }
        std::string line;
        if (!std::getline(CSVFile, line)) {        // read first line
            std::cerr << "File is empty: " << filename << std::endl;
            return;
        }
        while (std::getline(CSVFile, line)) {
            //row
            column = 0;
            size_t pos = line.find(",,");
            if (pos != std::string::npos) {
                line = line.substr(0, pos);
            } // ignore all data after ,,
            std::stringstream ss(line);
            std::string cell;

            std::string yearMonth;
            std::string state;
            std::string eventType;
            std::string czName;

            while (std::getline(ss, cell, ',')) {
                column++;
                //column
                if (!cell.empty() && cell.front() == '"' && cell.back() == '"') {
                    cell = cell.substr(1, cell.size() - 2);
                }
                if (column == 4) yearMonth = cell;
                if (column == 9) state = cell;
                if (column == 13) eventType = cell;
                if (column == 16) czName = cell;
            }

            std::string fixedCZ = fixNOAAczName(czName);
            std::string CountyState = fixedCZ + "/" + state;
            auto cityNodesMap = uscData->getCountyMap(CountyState);
            if (cityNodesMap.empty()) continue; // skip if not found in county map



            if (yearMonth.empty() || yearMonth.length() < 6) {
                std::cerr << "INVALID YEARMONTH: " << yearMonth << std::endl;
                continue;
            }
            int year = std::stoi(yearMonth.substr(0, 4));
            int month = std::stoi(yearMonth.substr(4, 2));
            weatherRecord curr;
            curr.eventType = eventType;
            curr.month = month;
            curr.year = year;
            for (trieNode* cityNode : cityNodesMap) {
                std::string cityState = cityNode->getCityState();
                data[cityState].push_back(curr);
            }
        }
        CSVFile.close();
    }

    void printAllData() {
        std::unordered_map<std::string, std::vector<weatherRecord>>::iterator it;
        int counter = 0;
        for (it = data.begin(); it != data.end(); it++)
        {
            counter++;
            std::cout << it->first    // string (key)
                      << " : ";
            for (auto weatherRecord  : it->second) { // data within the struct
                std::cout << "{";
                std::cout << weatherRecord.eventType << ", " << weatherRecord.month << ", " << weatherRecord.year;
                std::cout << "} ";
            }
            std::cout << std::endl;
        }
        std::cout << counter << std::endl;
    }

    std::vector<weatherRecord> getCountySevereEvents(std::string CountyState) {
        auto it = data.find(CountyState);
        if (it != data.end()) return it->second;
        return {};
    }


    void readAllCSV() {
        std::string filepath = "data/noaa_database/";
        std::vector<std::string> csvFiles;

        for (auto file : std::filesystem::directory_iterator(filepath)) {
            readCSV(file.path().filename());
        }

    }


    std::string fixNOAAczName(std::string rawName) { // cz name is almost always in the wrong format,
        //this should look at all of the possible prefixes and suffixes to remove them
        std::transform(rawName.begin(), rawName.end(), rawName.begin(), ::toupper); // we like uppercase for uniformity
        //remove coastal areas
        if (rawName.find("GULF") != std::string::npos || rawName.find("BAY") != std::string::npos || rawName.find("OUT ") != std::string::npos) return "";
        // all of the prefixes I could find
        std::vector<std::string> prefixes = {"NORTH ", "SOUTH ", "NORTHERN ", "SOUTHERN ", "EASTERN ", "WESTERN ",
            "EAST ", "WEST ", "CENTRAL ", "UPPER ", "LOWER ", "COASTAL ", "INLAND "
        };
        //suffixes
        std::vector<std::string> suffixes = {" COUNTY", " ZONE", " AREA", " PARISH"};
        //actually remove them
        for (auto prefix : prefixes) {
            if (rawName.find(prefix) == 0) {
                rawName = rawName.substr(prefix.length());
                break;
            }
        }
        for (auto suffix : suffixes) {
            if (rawName.find(suffix) != std::string::npos) {
                auto suffixPos = rawName.find(suffix);
                rawName = rawName.substr(0, suffixPos);
                break;
            }
        }
        return rawName;
    }
};


