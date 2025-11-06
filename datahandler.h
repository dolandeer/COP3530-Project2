#ifndef COP3530_PROJECT2_DATAHANDLER_H
#define COP3530_PROJECT2_DATAHANDLER_H

#include "parse.h"
#include "weightedTrie.h"
#include "minHeap.h"
#include <cpr/cpr.h>


// this exists to improve readability and data access within main
// the parse.h file is almost impossible to understand (sorry), in the interest of time making this
// as the "hub" for all operations is the simplest

class dataHandler {
    NOAAData* noaadata = nullptr;
    USCData* uscdata = nullptr;
    weightedTrie* trie = nullptr;
    MinHeap* minheap = nullptr;
    JSONData* json = nullptr;

    struct frontendResult {
        std::string place;
        std::string month;
        float avg_temp = 0.0;
        float current_temp = 0.0;
        vector<int> storm_events = {0,0,0,0,0,0,0,0,0,0,0,0};
        float deviation = 0.0;
        std::vector<float> history = {0,0,0,0,0,0,0};
    };

public:
    dataHandler() {
        std::cerr << "Bad Initialization!!";
    }
    dataHandler(NOAAData* noaa, USCData* usc, weightedTrie* trie, MinHeap* heap, JSONData* json) {
        this->noaadata = noaa;
        this->uscdata = usc;
        this->trie = trie;
        this->minheap = heap;
        this->json = json;
        noaadata->clear();
        if (!noaadata||!uscdata||!this->trie||!minheap){throw runtime_error("Bad pointers!");}
        usc->readCSV();
        usc->initTrie();
        std::cout << numWords() << std::endl;
        noaadata->readAllCSV();
        initHeap();
    }
    //datahandler functions
    std::vector<NOAAData::weatherRecord> getCitySevereEvents(std::string CityState) {
        std::transform(CityState.begin(), CityState.end(), CityState.begin(), ::tolower);
        if (trieSearch(CityState) == nullptr) return {};
        std::vector<NOAAData::weatherRecord> output = noaadata->getCountySevereEvents(CityState);
        std::cout << "getCitySevereEvents successful. found city : " << CityState <<std::endl;
        return output;
    }

    frontendResult getWeatherDataForCity(std::string place, int month) {
        std::transform(place.begin(), place.end(), place.begin(), ::tolower);
        frontendResult result;
        result.storm_events = std::vector<int>(12, 0);
        result.place = place;
        if (!trieSearch(place)) {
            std::cerr << "City not found in trie." << std::endl;
            return result;
        }
        auto events = noaadata->getCountySevereEvents(place);
        for (auto event : events) {
            if (event.month >= 1 && event.month <= 12)
                result.storm_events[event.month-1] += 1;
        }
        vector<float> APIData = APIWeatherDataByCity(place);
        result.current_temp = APIData[7];
        float avg = 0;
        for (int i = 0; i < 7; i++) {
            result.history[i] = APIData[i];
            avg += APIData[i];
        }
        avg /= 7;
        result.avg_temp = avg;
        result.deviation = result.current_temp-avg;
        return result;
    }

    std::string intToMonth(int month) {
        if (month == 1) return "January";
        if (month == 2) return "February";
        if (month == 3) return "March";
        if (month == 4) return "April";
        if (month == 5) return "May";
        if (month == 6) return "June";
        if (month == 7) return "July";
        if (month == 8) return "August";
        if (month == 9) return "September";
        if (month == 10) return "October";
        if (month == 11) return "November";
        if (month == 12) return "December";
        return "";
    }

    //https://stackoverflow.blog/2022/06/02/a-beginners-guide-to-json-the-data-format-for-the-internet/
    std::string frontendSearch(dataHandler& handler, std::string place, int month) {
        std::transform(place.begin(), place.end(), place.begin(), ::tolower);
        frontendResult data = handler.getWeatherDataForCity(place, month);
        std::ostringstream output;

        //make json
        output << "{";
        output << "\"results\":[{";
        output << "\"place\":\"" << data.place << "\",";
        output << "\"month\":" << '"' <<intToMonth(month)<< '"' << ",";
        output << "\"avg_temp\":" << std::fixed<< std::setprecision(2) << data.avg_temp << ",";
        output << "\"current_temp\":" <<std::fixed<< std::setprecision(2) << data.current_temp << ",";
        output << "\"storm_events\":" << data.storm_events[month-1] << ",";
        output << "\"deviation\":" <<std::fixed << std::setprecision(2) << data.deviation << ",";
        output << "\"history\":[";
        output << data.history[0];
        output << ",";
        output << data.history[1];
        output << ",";
        output << data.history[2];
        output << ",";
        output << data.history[3];
        output << ",";
        output << data.history[4];
        output << ",";
        output << data.history[5];
        output << ",";
        output << data.history[6];
        output << "]";
        output << "}],";

        output << "\"ranking\":[";
        std::vector<Node> topCities = handler.getTopMinHeap(16);
        for (auto i = 0; i < topCities.size(); i++) {
            if (i > 0) output << ",";
            output << "{";
            output << "\"place\":\"" << topCities[i].cityName << "\",";
            output << "\"score\":" << topCities[i].weight;
            output << "}";
        }

        output << "]";
        output << "}";


        return output.str();
    }

    //api call
    vector<float> APIWeatherDataByCity(std::string place) {
        vector<float> past7Days = {0,0,0,0,0,0,0,0}; // past 7 days + current temp
        float curr;
        std::pair<float,float> cityCoords = {0,0};
        cityCoords = uscdata->getCityCoords(place);
        if (cityCoords.first != 0 && cityCoords.second != 0) {
            //the place exists
            cpr::Response r = cpr::Get(cpr::Url{"https://api.open-meteo.com/v1/forecast?latitude=" + std::to_string(cityCoords.first) + "&longitude=" + std::to_string(cityCoords.second) + "&daily=temperature_2m_max&current=temperature_2m&timezone=auto&past_days=7&forecast_days=1&wind_speed_unit=ms&temperature_unit=fahrenheit&precipitation_unit=inch"});
            //std::cout << r.text; debug
            curr = json->parseCurrentTemperature(r.text);
            past7Days = json->parseWeeklyTemperature(r.text);
        }
        return past7Days;
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
    void iterateUSCMap() { // 31184 cities according to this dataset...
        //my console cant display them all but maybe yours can
        uscdata->iterateMap();
    }
    void initUSC() {
        uscdata->readCSV();
        uscdata->initTrie();
        std::cout << numWords() << std::endl;
    }
    //essential NOAA functions
    void printWeatherEventMap() {
        noaadata->printAllData();
    }
    std::unordered_map<std::string, std::vector<NOAAData::weatherRecord>> getWeatherEventMap() {
        return noaadata->getWeatherEventMap();
    }
    void printWeatherEventState(std::string state) {
        noaadata->printStateData(state);
    }
    //essential minHeap.h functions
    void initHeap() {
        std::unordered_map<std::string, std::vector<NOAAData::weatherRecord>> eventMap = getWeatherEventMap();
        std::unordered_map<std::string, std::vector<NOAAData::weatherRecord>>::iterator it;
        for (it = eventMap.begin(); it != eventMap.end(); it++) {
            std::string key = it->first;
            int weight = it->second.size();
            if (weight == 0) continue; //if weight is 0 we can assume it was skipped during data collection
            minheap->insert(key, weight);
        }
    }
    int heapSize() {
        return minheap->size();
    }
    int getCityWeightInHeap(const std::string &cityName) {
        return minheap->getWeight(cityName);
    }
    void printTopHeapNode() {
        auto min = minheap->getMin();
        std::cout << min.cityName << " : " << min.weight << std::endl;
    }
    void printTopNodes(int x) {
        minheap->printTopX(x);
    }
    vector<Node> getTopMinHeap(int x) {
        return minheap->getTop(x);
    }
};


#endif //COP3530_PROJECT2_DATAHANDLER_H