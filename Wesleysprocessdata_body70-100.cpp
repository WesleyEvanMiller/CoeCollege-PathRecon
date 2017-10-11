//Wesleysprocessdata Program File

#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include <algorithm>
#include <vector>

#include "Wesleysprocessdata_header70-100.hpp"

using namespace std;

vector<xEdep> energyDeps;                                              //Global Declaration of a vector of objects

processData::processData(){

	fileout = fopen("PATHDATA.txt", "w");

    int k = 0;

    for (inc = 0; inc < 30; inc += 2){                          //Runs 15 times (2 degree intervals)
        for (ang = inc; ang < 359; ang += 30){                  //Runs 12 times (12 30 degree sets for 360 degrees)
            for (xloc = -5.0; xloc <= 5.1; xloc += 0.1){        //Runs 102 times Total = 18360 (51 sets of 360)

                energyDeps.clear();                             //Clears the vector each loop iteration
                
                if(xloc < 0.1 && xloc > -0.1)                   //The xloc wasnt exactly zero. This ensures it is
                    xloc = 0;
                
                setEdep(xloc, ang);                             //Calls read file fucntion

                sort(energyDeps.begin(), energyDeps.end(), qSortEdep);        //Calls the predefined sort algorithm to sort least to greatest

                a = -49.5;                                      //In xy plane 0 being the center of the sensor a and b represent the side of the sensor
                b = -49.5;

                //totXEA = 0, totXEALoc = 0, totXEB = 0, totXEBLoc = 0;            //Variable Initation to find the average center of energy (works better commented out?)                  

                for(i = 0; i < 100 ;i++){                       //Finds the Total Energy in Layers 1 and 3 which are x oriented bars
                        totXEA += edep[1][i];                   //Finds the the Total Energy times bar location in Layers 1 and 3
                        totXEALoc += edep[1][i] * a;
                        a++;
                        
                        totXEB += edep[3][i];
                        totXEBLoc += edep[3][i] * b;
                        b++; 
                }
                
                if(totXEA > 0)                                  //Finds the Center of energy in Layer 1
                    xCLocA = totXEALoc / totXEA;
                if(totXEA==0)                                   //If there is no energy in Layer 1 the center location is -1
                    xCLocA = -1;
                if(totXEB > 0)                                  //Finds the Center of energy in Layer 3
                    xCLocB = totXEBLoc / totXEB;            
                if(totXEB == 0)                                 //If there is no energy in Layer 3 the center location is -1
                    xCLocB = -1;

                zPrimeI  = 5.0;				                                //Starts at z = 5 and travels to z = -5 
                zPrimeF = -5.0;				                                
                xPrimeFA = -1 / 10;                                     //Set location of energy in x on layer 1. We have experimented with setting this manually to -1 instead of xCLocA and got better results.
                xPrimeFB = -1 / 10;                                     //Set location of energy in x on layer 3. We have experimented with setting this manually to -1 instead of xCLocB and got better results.

                theta = atan((xPrimeFA - xPrimeFB) / .2);                   //arcTan to find theta for trig conversions
                xPrimeI = xloc;							                    //X location of scanner
                xPrimeF = xPrimeFA + (zPrimeF + 50)* tan(theta / 57.3);     //X final location of scanner

                for(int t=6999;t>6994;t--){                                 //Loops in reverse from the end of the vector to find high edep values. i > 6999 - x; x is the amount of points to be found 
                    highZ = kylesCorrection(energyDeps[t].getCoords());     //Inputs the high edeps to the kyles correction function to correct for bragg peaks that are off center of a layer
                    trigConv(highZ, ang);                                   //Sends corrected Z value and angle of the scan to be writen to a output file.
                }

                k++;                                                        //Program Progress Counter

                cout << k << endl;                                          //Prints Program Progress Counter

            }
        }
    }
    cout << "Done!" << endl;
    fclose(fileout);                                                        //Closes output file
}

void processData::setEdep(double xloc, int ang){        //Reads individual data files and inputs them into an array

    stringstream fileBuffer;                            //Temporary file name holder (string stream)
    fileBuffer << ang << "_" <<(xloc*10) << "_0.txt"; 	//Fills the string stream with file names
	fileBuffer >> fileName;                             //Puts the steam value into a string
    ifstream filein;                                    //Input stream of text file data                                   
    filein.open(fileName.c_str());                      //Opens a file according fileName which needs to be converted to a constant string

    for (bar = 0; bar < 100; bar++){                    //Runs through all bars in a layer (bars  = 0-99)
    /* To make a consistant coord system to search through I format the strings accordingly */    
        if(bar < 9)                                     //If under 10 (bar = 0-8) add 2 zeros to the string because the max is 100 (3 characters long)                           
            coordB = "00" + intToString(bar+1);
        if(bar == 99)                                   //If 100 (bar = 99) just set the coord to the bar value which is 100 
            coordB = intToString(bar+1);
        if(bar >= 9 && bar < 99)                        //If between 10 and 100 (9 <= bar < 99) add 1 zero
            coordB = "0" + intToString(bar+1);

        for (layer = 0; layer < 70; layer++)            //Runs through all layers (layers = 0 - 69)		
            {
                if(layer < 9)                           //If less than 10 (layer = 0-9) add one zero
                    coordL = "0" + intToString(layer+1);
                else                                    //Else set the coord to the layer value
                    coordL = intToString(layer+1);
                
                finalCoords = coordL + "-" + coordB;    //Build full coords through concatination (Layer - Bar)

                barIndex[layer][bar] = finalCoords;     //Puts coords in a 70x100 array to stay consistant with the input file data's order
                filein >> edep[layer][bar];             //Orders input file data into a 70x100 array (The data is order with array position cooresponding to the layer-bar *for now*)

                xEdep tmpenergyDeps;                    //Creates a temporary object

                tmpenergyDeps.setValues(barIndex[layer][bar], edep[layer][bar]);    //Calls the object's setValues function sets coords and energy deposition 
                energyDeps.push_back(tmpenergyDeps);    //Calls c++ vectors push back function to add objects to an object vector

            }
    }
    filein.close();
}

bool processData::qSortEdep(const xEdep& a, const xEdep& b){                //Comparison Function for sort

    return (a.energy<b.energy);
}

double processData::kylesCorrection(string locOfE){                         //Kyles correction corrects for the posibility of haveing a off centered bragg peak within a layer
    
    eOfPrev = 0;                                        //energy of previous layer
    eOfNext = 0;                                        //energy of next layer

    for(i = 0; i<7000; i++){                            //Runs through the entire vector to find the energy in the layer before and after the passed depth

        otherLayers = strtod(energyDeps[i].getCoords().substr(0,2).c_str(),NULL);       //Holds coords for all other layers in double format
        otherBars = strtod(energyDeps[i].getCoords().substr(3,3).c_str(),NULL);         //Holds coords for all other bars in double format
        currentLayer = strtod(locOfE.substr(0,2).c_str(),NULL);                         //Holds layer coords for passed coord
        currentBar = strtod(locOfE.substr(3,3).c_str(),NULL);                           //Holds bar coords for passed coord
        
        if(currentLayer == 70){                         //If its layer 70 only find previous bar energy
            if(otherLayers + 1 == currentLayer && otherBars == currentBar){     //While searching if it finds a layer and bar before the current one its sets the energy
                eOfPrev = energyDeps[indexOfCoords(energyDeps[i].getCoords())].getEnergy();
            }
            eOfNext = 1;
        }

        if(currentLayer == 1){                          //If its layer 1 only find next bar energy
            if(otherLayers - 1 == currentLayer && otherBars == currentBar){     //While searching if it finds a layer and bar after the current one its sets the energy   
                eOfNext = energyDeps[indexOfCoords(energyDeps[i].getCoords())].getEnergy();
            }
            eOfPrev = 1;
        }

        if(locOfE.substr(0,2) != "01" && locOfE.substr(0,2) != "70"){       //If neither find both pervious and next bars energies

            if(otherLayers + 1 == currentLayer && otherBars == currentBar){     //While searching if it finds a layer and bar before the current one its sets the energy
                eOfPrev = energyDeps[indexOfCoords(energyDeps[i].getCoords())].getEnergy();
            }
            if(otherLayers - 1 == currentLayer && otherBars == currentBar){     //While searching if it finds a layer and bar after the current one its sets the energy
                eOfNext = energyDeps[indexOfCoords(energyDeps[i].getCoords())].getEnergy(); 
            }
        }
    }

    if(eOfPrev == 0 || eOfNext == 0)                    //If either are zero return the layer number plus .5 for a bragg peak in the dead center
        trueLocOfE = currentLayer + .5;
    else                                                //Otherwise put the energies through the kylescorrection to compesate for a non centered peak
        trueLocOfE = currentLayer + .5 + log(energyDeps[indexOfCoords(locOfE)].getEnergy()/eOfPrev) -
            log10(energyDeps[indexOfCoords(locOfE)].getEnergy()/eOfNext); 
    return trueLocOfE;                                  //Return it location as depth of peak
}

void processData::trigConv(double highZx, int angle){                       //A function to write to an output file for image creation with ART
    
    xi = (-zPrimeI*cos(angle / 57.3) + xPrimeI*sin(angle / 57.3));			    //Using trig for x initial (unprimed) approx from z and x (primed)
    yi = (-zPrimeI*sin(angle / 57.3) - xPrimeI*cos(angle / 57.3));			    //Using trig for y initial (unprimed) approx from z and x (primed)
    xf = (-zPrimeF*cos(angle / 57.3) + xPrimeF*sin(angle / 57.3));			    //Using trig for x final (unprimed) approx from z and x (primed)
    yf = (-zPrimeF*sin(angle / 57.3) - xPrimeF*cos(angle / 57.3));			    //Using trig for y final (unprimed) approx from z and x (primed)
    energy = 0.0087*(-3915+2*sqrt(3.60728*pow(10,6)+2.8675*pow(10,6)*highZx));  //Energy in respect to depth of Bragg Peak
    Ex = energy*cos(theta / 57.3);                                              //Final x location of path
    Ey = (fabs(energy*sin(theta / 57.3)));                                      //Final y location of path
    px1 = sqrt(2 * 150 * 938.272046);                                           //Initial x prime location of path
    py1 = 0;                                                                    //Initial y prime location of path
    px2 = sqrt(2 * 938.272046*Ex);                                              //Final x prime location of path
    py2 = sqrt(2 * 938.272046*Ey);                                              //Final y prime location of path

    fprintf(fileout, "  %i    %.3f    %.3f     %.3f      %.3f    150.000     %.3f     %.3f"
        "     %.3f       %.3f    %.3f \n", angle, xi, yi, px1, py1, xf, yf, px2, py2, energy);  //Formated string for file print out
    if (highZx < 0)
        cout <<"error";
}

string processData::intToString(int toStringNum){                           //Utility function that converts Integers to Strings
    stringstream ss;
    ss << toStringNum;
    return ss.str();
}

int processData::indexOfCoords(string itemToIdex){                          //Utility function that converts Integers to Strings
    for(i=0;i<7000;i++)
        if(energyDeps[i].getCoords() == itemToIdex){
            return i;
        }
}

int main(){
    processData pd;                                                         //Calls the constructor
    return 0;
}