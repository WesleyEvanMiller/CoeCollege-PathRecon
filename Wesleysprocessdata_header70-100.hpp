//Wesleysprocessdata Header File

#ifndef WESLEYSPROCESSDATA_HEADER_HPP
#define WESLEYSPROCESSDATA_HEADER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>

#include "Wesleysprocessdata_xEdep70-100.cpp"

using namespace std;

class processData{
    public:
        processData();
        void setEdep(double xloc, int ang);
        static bool qSortEdep(const xEdep& a ,const xEdep& b);
        double kylesCorrection(string locOfE);
        void trigConv(double highZx, int ang);
        string intToString(int toStringNum);
        int indexOfCoords(string itemToIdex);
        
        FILE *fileout;

        string fileName, coordB, coordL, finalCoords, barIndex[70][100];

        int ang, inc, layer, bar, i;

        double xloc, trueLocOfE, eOfPrev, eOfNext, zPrimeI, zPrimeF,
            xPrimeFA, xPrimeFB, xPrimeI, xPrimeF, theta, xCLocA, xCLocB, 
            totXEA, totXEB, totXEALoc, totXEBLoc, a, b, otherLayers, otherBars,
            currentLayer, currentBar, engyPreNext, highZ, xi, yi, xf, yf, energy, Ex, Ey, 
            px1, py1, px2, py2, edep[70][100];
            
    private:
    
    protected:

};
#endif