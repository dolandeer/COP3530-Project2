//latitude and longitude distance formula, returns as a magnitude where lower is closer
#pragma once
# define M_PI           3.14159265358979323846
#include <cmath>
using namespace std;

inline double distance(double lat1, double lon1, double lat2, double lon2){
    const double R = 6371000; // earths radius
    const double phi1 = lat1 * M_PI/180; // degrees to radians
    const double phi2 = lat2 * M_PI/180;
    const double deltaPhi = (lat2-lat1) * M_PI/180;
    const double deltaLambda = (lon2-lon1) * M_PI/180;

    const double a = sin(deltaPhi/2) * sin(deltaPhi/2) + cos(phi1) * cos(phi2) * sin(deltaLambda/2) * sin(deltaLambda/2);
    const double c = 2 * atan2(sqrt(a), sqrt(1-a));

    const double d = R * c; // meters
    return d;
}
