#include <iostream>
#include <cpr/cpr.h>
#include "crow_all.h"
#include "parse.h"
#include "datahandler.h"
#include <fstream>
#include <sstream>


//https://www.geeksforgeeks.org/cpp/inline-functions-cpp/
inline std::string load_file(const std::string& filename) {
    std::ifstream file(filename);
    std::stringstream out;
    out << file.rdbuf();
    return out.str();
}

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

    //INITIALIZATION:
    weightedTrie testTrie;
    USCData usc(&testTrie);
    NOAAData noaa(&testTrie, &usc);
    MinHeap heap;
    JSONData json;
    dataHandler handler(&noaa,&usc,&testTrie,&heap,&json);
    //CODE BELOW:

    std::string jsonOutput = handler.frontendSearch(handler, "miami/florida", 10);
    std::cout << jsonOutput << std::endl;

    //https://crowcpp.org/master/getting_started/a_simple_webpage/
    //https://crowcpp.org/master/guides/templating/
    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([]{
        return load_file("index.html");
     });
    CROW_ROUTE(app,"/app.js")([] {
        return load_file("app.js");
    });
    CROW_ROUTE(app,"/styles.css")([] {
        return load_file("styles.css");
    });

    CROW_ROUTE(app, "/api/autocomplete")([&handler](const crow::request& req){
    auto term = req.url_params.get("q");
    std::vector<std::string> suggestions;
    if(term) {
        suggestions = {handler.autocomplete(term,false),handler.autocomplete(term,true)};
        if (!suggestions.empty()) {
            if (suggestions[0] == suggestions[1]) suggestions.pop_back();
        }
    }
        crow::json::wvalue result = crow::json::wvalue::list(); // make it a JSON array
        for(size_t i = 0; i < suggestions.size(); i++) {
                result[i]["label"] = suggestions[i];
        }
        crow::response response(result);
        response.add_header("Access-Control-Allow-Origin", "*");
        return response;
    });

    CROW_ROUTE(app, "/api/search")([&handler](const crow::request& req){
        auto place = req.url_params.get("place");
        auto month = req.url_params.get("month");
        crow::json::wvalue result;
        std::string jsonData;
        if(place && month) {
            jsonData = handler.frontendSearch(handler, place, int(*month));
            result = crow::json::load(jsonData);
        }
        std::cout << "SEARCH CALLED: " << jsonData << std::endl;
        crow::response response(result);
        response.add_header("Access-Control-Allow-Origin", "*");
        return response;
    });

    app.port(1337).multithreaded().run();
}
