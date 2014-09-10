#!/usr/bin/env python
# -*- coding: utf-8 -*-

from math import tanh, exp

def ActivationFunction(sum): 
    return ( 1.0 / ( 1.0 + exp( -1.0 * sum ) ) ); 

def myTestFunc(param):

    ############################## 
    #
    # File used by unit test
    # PMMLBasicsTest1::testExportNeuralNetworkPython
    #
    ############################## 

    nInput = 8;
    nOutput = 1;
    nHidden = 1;
    nNeurones = 10;
    myTestFunc_act = [];
    res = [];

    # --- Preprocessing of the inputs and outputs
    myTestFunc_minInput = [
      0.099999, 25048.9, 89334.9, 89.5523, 1050, 
    760.001, 1400.02, 10950, 
    ];
    myTestFunc_minOutput = [
        77.8117
    ];
    myTestFunc_maxInput = [
    0.028899, 14419.8, 15180.8, 15.2866, 34.6793, 
    34.6718, 161.826, 632.913, 
    ];
    myTestFunc_maxOutput = [
        45.7061
    ];
    # --- Values of the weights
    myTestFunc_valW = [
    -1.74548, 6.96551, -1.26357, 0.753663, 0.00165366, 
    0.004725, 0.00996979, 0.178798, -0.180981, -0.173569, 
    0.0855967, 
    ];
    # --- Constants
    indNeurone = 0;

    # --- Input Layers
    for i in range(nInput) :
        myTestFunc_act.append( ( param[i] - myTestFunc_minInput[i] ) / myTestFunc_maxInput[i] ) ;
        indNeurone += 1 ;
        pass

    # --- Hidden Layers
    for member in range(nHidden):
        CrtW = member * ( nInput + 2) + 2;
        sum = myTestFunc_valW[CrtW];
        CrtW += 1 ;
        for source in range(nInput) :
            sum += myTestFunc_act[source] * myTestFunc_valW[CrtW];
            CrtW += 1 ;
            pass
        myTestFunc_act.append( ActivationFunction(sum) ) ;
        indNeurone += 1 ;
        pass

    # --- Output
    for member in range(nOutput):
        sum = myTestFunc_valW[0];
        for source in range(nHidden):
            CrtW = source * ( nInput + 2) + 1;
            sum += myTestFunc_act[nInput+source] * myTestFunc_valW[CrtW];
            pass
        myTestFunc_act.append( sum );
        indNeurone += 1 ;
        res.append( myTestFunc_minOutput[member] + myTestFunc_maxOutput[member] * sum );
        pass

    return res;


