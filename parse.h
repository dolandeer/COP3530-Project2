#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>

//JSON Parsing (getting data from the API)
class JSONData {

};



//NOAA Parsing (treating ',,' as end of row and ignoring all data after)
class NOAAData {
private:
    //it may be more efficient to store this as a vector of dictionaries?
    //format {key = "simplified_cz_name/state" || values = [weather type, month]}
    //simplified cz name would be found by comparing cz_name against uscities database to remove prefixes
    //would require rewriting some of readCSV and printData, but would simplify getData
    std::vector<std::vector<std::vector<std::string>>> data;  // file<row<column<data>>>


public:
    void printData() const { // helper function that prints data to console by row, should mirror database
        std::cout << std::endl;
        for (const auto& file : data) {
            for (const auto& row : file) {
                for (const auto& cell : row) {
                    std::cout << cell << ", ";
                }
                std::cout << std::endl;
            }
        }
    }

    void insertData(std::vector<std::vector<std::string>> input) {
        data.push_back(input);
    }




    // parse a single CSV file
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

};



//USCities Parsing (use a for loop to ignore any unwanted data)
// we only really need this database for converting city name -> lat/long for calling the API
// and for weighted trie initialization
// store data as a vector: {key = "city/state" || values = [county, lat, long]}
// also store a vector of county names for comparison against NOAA data (NOAA tracks by county)
// for severe weather data, we can assume every city within a county experienced the same weather
// storing city and state as key to avoid duplicate cities such as charleston SC and charleston WV
class USCData {
    std::map<std::string, std::vector<std::string>> data; // {"Orlando/Florida, <FIPS,Orange,x,y,pop>"}

public:
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

    void printData(std::vector<std::vector<std::string>> file) const { // helper function that prints data to console by row, should mirror database
        std::cout << std::endl;
            for (const auto& row : file) {
                for (const auto& cell : row) {
                    std::cout << cell << ", ";
                }
                std::cout << std::endl;
            }
        }

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
                    value.push_back(cell);
                }
            }
            data[key] = value;
        }
    }


    void iterateMap() {
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


};