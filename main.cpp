#include <iostream>
#include <cpr/cpr.h>
#include "crow_all.h"
#include "parse.h"
#include "datahandler.h"


int main() {
    std::cout << "Hello, World!" << std::endl;
    // example API call. max of 10,000 calls a day for open-meteo so call as needed do not loop!

    /*
    cpr::Response r = cpr::Get(cpr::Url{"https://api.open-meteo.com/v1/forecast?latitude=29.6516&longitude=-82.3248&daily=weather_code,temperature_2m_max,temperature_2m_min&hourly=temperature_2m,weather_code&current=temperature_2m,weather_code&timezone=America%2FNew_York&past_days=5&forecast_days=1&wind_speed_unit=mph&temperature_unit=fahrenheit&precipitation_unit=inch"});
    std::cout << r.status_code << std::endl;                  // 200
    std::cout << r.header["content-type"] << std::endl;       // application/json; charset=utf-8
    std::cout << r.text;                         // JSON text string
    */

    //TEST

    //example of parsing NOAA file, will be streamlined in the future
    /*
    NOAAData noaa;
    std::vector<std::vector<std::string>> in = noaa.readCSV("StormEvents_details-ftp_v1.0_d2000_c20250520.csv");
    noaa.insertData(in);
    noaa.printData();
    */
    //INITIALIZATION:
    weightedTrie testTrie;
    weightedTrie* triePtr = &testTrie;
    USCData usc(triePtr);
    USCData* uscPtr = &usc;
    NOAAData noaa(triePtr, uscPtr);
    NOAAData* noaaPtr = &noaa;
    MinHeap heap;
    MinHeap* heapPtr = &heap;
    dataHandler handler(noaaPtr,uscPtr,triePtr,heapPtr);
    //CODE BELOW:

    std::string city = handler.autocomplete("soul");
    std::cout << city << std::endl;


    auto vec = handler.getCitySevereEvents(city);
    for (auto event : vec) {
        std::cout << " {" << event.eventType << ", " << event.year << event.month << "} " << std::endl;
    }

    std::cout << handler.heapSize() << std::endl;
    handler.printTopHeapNode();
    handler.printTopNodes(10);
    //handler.printWeatherEventState("florida");

    //noaa.readCSV("StormEvents_details-ftp_v1.0_d2000_c20250520.csv");

    //std::cout << testTrie.trieSearch("TEST T");
}
