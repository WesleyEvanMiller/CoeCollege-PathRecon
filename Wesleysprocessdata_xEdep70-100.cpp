//Wesleysprocessdata Program File

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>

using namespace std;

class xEdep{
    public:
        void setValues(string xy, double Mev);
        string getCoords(){return coords;}          //Returns the coords
        double getEnergy(){return energy;}         //Returns the energy
        string coords;
        double energy;
};

void xEdep::setValues(string xy, double Mev){       //Allows for the values of the object to be set when called
    coords = xy;
    energy = Mev;
}







