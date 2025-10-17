#include <iostream>
#include <cpr/cpr.h>
#include "crow_all.h"
#include "parse.h"


int main() {
    std::cout << "Hello, World!" << std::endl;
    // example API call. max of 10,000 calls a day for open-meteo so call as needed do not loop!
    /*
    cpr::Response r = cpr::Get(cpr::Url{"https://api.open-meteo.com/v1/forecast?latitude=52.52&longitude=13.41&forecast_days=1"});
    std::cout << r.status_code << std::endl;                  // 200
    std::cout << r.header["content-type"] << std::endl;       // application/json; charset=utf-8
    std::cout << r.text;                         // JSON text string
    */

    //example of parsing NOAA file, will be streamlined in the future
    NOAAData noaa;
    std::vector<std::vector<std::string>> in = noaa.readCSV("StormEvents_details-ftp_v1.0_d2000_c20250520.csv");
    noaa.insertData(in);
    noaa.printData();
}
