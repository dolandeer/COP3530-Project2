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
        std::map<std::string, std::vector<std::string>> fileData;
        // {key = "simplified_cz_name/state" || values = [weather type, month]}
        // find simplified cz name on input, compare against already initialized trie
        // read uscities -> init trie -> read NOAAData (compare against trie for name) -> store NOAAData in min/max heap
        // api c
    };

    std::vector<file> data;  // file<{key = "simplified_cz_name/state" || values = [weather type, month]}>
    weightedTrie* trie = nullptr;



public:
    void printData() const { // TODO helper function that prints data to console by row, should mirror database
    }





    // parse a single CSV file TODO redo this, dont insert data automatically, make an insertData function
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
                row.push_back(cell);
            }
            output.push_back(row);
        }
        CSVFile.close();
        return output;
    }

    void insertData(){} //TODO this will insert info into data, make sure to compare against trie here

    void getData(){} //TODO

};



//USCities Parsing (use a for loop to ignore any unwanted data)
// we only really need this database for converting city name -> lat/long for calling the API
// and for weighted trie initialization
// store data as a map: {key = "city/state" || values = [county, lat, long]}
// for severe weather data, we can assume every city within a county experienced the same weather
// storing city and state as key to avoid duplicate cities such as charleston SC and charleston WV
class USCData {
    std::map<std::string, std::vector<std::string>> data; // {Orlando/Florida, <Orange,x,y,pop>}

public:
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
        //insertData(output);
        return output;
    }

    //inserts output of readCSV into data
    void insertData(std::vector<std::vector<std::string>> input) {
        for (auto row : input) {
            int count = 0;
            std::string key = "";
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
        std::map<std::string, std::vector<std::string>>::iterator it;

        for (it = data.begin(); it != data.end(); it++)
        {
            std::cout << it->first    // string (key)
                      << " : ";
            for (auto i : it->second) {
                std::cout << i << ", ";
            }
            std::cout << std::endl;
        }
    }

    //TODO need getData function, map<string, vector<string>> is the format
};