//
//  main.cpp
//  hdc_cPlusPlus
//
//  CSV Files obtained from https://git-disl.github.io/GTDLBench/datasets/mnist_datasets/
//
//  Created by Ally Thach on 8/12/20.
//  Copyright © 2020 Ally Thach. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <functional>
#include <iterator>
#include <iterator>
#include <time.h>
#include "stdc++.h"

using namespace std;

vector<int> linspace(int start_in, int end_in, int num_in);
void printLinspace(vector<int> v);
vector<vector<int> > zeros(int row, int col);
vector<vector<int> > ones(int row, int col);
int getNum(vector<int> vec);
vector<int> randperm(int num);
vector<int> elementWiseMulti(vector<vector<int> > v1, vector<vector<int> > v2);

int main()
{
    //auto start = chrono::steady_clock::now();
    
    //variables
    string trainCSV = "mnist_train.csv";
    string testCSV = "mnist_test.csv";
    string line = "";
    //int label;
    //int pixel;
    int row = 0;
    int lMax = 0;
    int lMin = 0;
    int dL = 0;
    int nLevel = 0;
    int M = 11;
    int D = 10000;
    int numTrainSamples = 60000;
    int numTestSamples = 10000;
    int numClasses = 10;
    int numFeatures = 784;
    //int sampleLevel = 0;
    //int indexLevel = 0;
    int accuracy = 0;
    //char comma;
    ifstream fin;
    ofstream fout;
    vector<vector<int> > trainset_array;
    vector<int> trainset_labels(numTrainSamples + 1);
    vector<vector<int> > testset_array;
    vector<int> testset_labels(numTestSamples + 1);
    vector<int> labels(10,0);
    vector<vector<int> > LD;
    vector<vector<int> > ID;
    vector<vector<int> > CH;
    vector<vector<int> > sampleHV;
    vector<vector<int> > nOne;
    vector<int> classes(numTrainSamples + 1);
    vector<int> L;
    vector<int> rowArray(numFeatures);
    vector<int> nAlter(D);
    vector<vector<int> > quantLevel;
    vector<vector<int> > quant;
    vector<vector<int> > classHV;

    
    
//    //testing to see if i can read the csv file
//    fin.open(trainCSV);
//
//    if(!fin.is_open())
//    {
//        cout << "Error: Can't open " + trainCSV << endl;
//    }
//
//    for(int i = 0; i < 1; i++)
//    {
//        if(fin >> label)
//        {
//            cout << label << endl;
//        }
//    }
    
    //loading data
    //CSV Format: label, pix-11, pix-12, pix-13, ...
    cout << "loading data..." << endl;
    time_t begin_L, end_L;
    time(&begin_L);
    fin.open("mnist_train.csv");
    if(!fin.is_open())
    {
        cout << "Error: Can't open " + trainCSV << endl;
    }
    else
    {
        while(!fin.eof())
        {
            
            if(row > numTrainSamples)
            {
                break;
            }
            
            fin >> trainset_labels.at(row);
            //fin.ignore(50000000, '\n');
            trainset_array.push_back(rowArray);
            
            for(int col = 0; col < 784; col++)
            {
                fin.ignore(50000000, ',');
                fin >> trainset_array[row][col];
                //cout << trainset_array[row][col] << " ";
            }
            row++;
        }
    }
    
    time(&end_L);
    cout <<"loading time="<<end_L-begin_L<<"seconds"<<endl;
    
    time_t begin_HV, end_HV;
    time(&begin_HV);
        //creating level hypervector
    cout << "creating level hypervector..." << endl;
    
    lMin= *min_element(trainset_array[0].begin(),trainset_array[0].end());
    lMax= *max_element(trainset_array[0].begin(),trainset_array[0].end());
    
    L = linspace(lMin, lMax, M);
    //printLinspace(L);
    
    nLevel = M;
    
    //cout << nLevel << endl;
    
    LD = zeros(nLevel, D); //level hypervector - a 2d vector of 0s and 1s
    

    for(int cols = 0; cols < D; cols++)
    {
      int tempRand = rand() % 2;
      LD[0][cols] = tempRand;
    }
    //printLinspace(LD[0]);
    
    nAlter = randperm(D); //1D vector of random permutations of D (number of hypervectors)
    random_shuffle(nAlter.begin(), nAlter.end());
    int numAlter = floor(D / (nLevel));
    int kalter;
    for(int i_ = 1; i_ < nLevel; i_++)
    {
        for(int j_ = 0; j_ < D; j_++)
        {
            LD[i_][j_] = LD[i_ - 1][j_];
        }
        
        for(int k_ = 0; k_ <numAlter; k_++)
        {
          kalter = nAlter[(i_ - 1)*numAlter + k_];
          LD[i_][kalter]=int(LD[i_][kalter]==0);
        }
    }
    
        //test to see if ID is being populated properly;
    
    int LD_test1=0;
    int LD_test2=0;
    for(int jtest = 0; jtest < D; jtest++)
    {
      LD_test1=LD_test1+ (LD[0][jtest]^LD[1][jtest]) ; //FIX ME: print out all 0s
      LD_test2=LD_test2+ (LD[5][jtest]^LD[0][jtest]) ;
    }
    cout << LD_test1<< endl;
    cout << LD_test2<< endl;
    //printLinspace(LD[1]);
    //creating identity hypervector
    cout << "creating identity hypervector..." << endl;
    numFeatures = static_cast<int>(trainset_array.at(0).size());
    //cout << numFeatures << endl;
    ID = zeros(numFeatures, D);

    for(int idR = 0; idR < numFeatures; idR++)
    {
      for(int idC = 0; idC < D; idC++)
      {
        ID[idR][idC] = rand() % 2;
      }
    }

    
    //test to see if ID is being populated properly;
    
        int ID_test=0;
        for(int j = 0; j < ID[0].size(); j++)
        {
            ID_test=ID_test+ (ID[1][j]^ID[0][j]) ; //FIX ME: print out all 0s
        }
        cout << ID_test<< endl;
    time(&end_HV);
    cout <<"Hypervector Creation time="<<end_HV-begin_HV<<"seconds"<<endl;

    //creating class hypervector
    time_t begin_Train, end_Train;
    time(&begin_Train);
    cout << "training: creating class hypervector..." << endl;
    CH = zeros(numClasses, D);
    vector<int>  sHV(D,0);
    vector<int>  sXorHV(D,0);
    vector<vector<int> > qOnes = ones(1, nLevel);
    int quantMin;
    int qcheck;
    int indMin=0;
    int i;
    for(int iSam = 0; iSam < 1000; iSam++)
    {
        
        for(int jSam = 0; jSam < numFeatures; jSam++)
        {
            indMin=0;
            quantMin = abs(trainset_array[iSam][jSam]-L[0]);
            for (i=1; i<M;i++)
            {
              qcheck=abs(trainset_array[iSam][jSam]-L[i]);
              if (quantMin > qcheck)
              {
                quantMin=qcheck;
                indMin=i;
              }
            }
                   
            transform(LD[indMin].begin(), LD[indMin].end(), ID[jSam].begin(), sXorHV.begin(), bit_xor<int>());
            transform (sHV.begin(), sHV.end(), sXorHV.begin(), sHV.begin(), plus<int>());
        }
         
        for(int jD = 0; jD < D; jD++)
        {
          sHV[jD]=int(sHV[jD] >= (numFeatures / 2));
        }
          
        for(int k_ = 0; k_ < classes.size(); k_++)
        {
            if(trainset_labels[iSam] == classes[k_])
            {
                transform (CH[k_].begin(), CH[k_].end(), sHV.begin(), CH[k_].begin(), plus<int>());
                labels.at(k_)=labels.at(k_)+1;
                break;
            }
        }
        cout<<iSam<<endl;
    }
    
    //TASK: understand this syntax
    
    classHV = zeros(numClasses, D);
    for(int ind = 0; ind < numClasses; ind++)
    {

        for(int jnd = 0; jnd < D; jnd++)
        {
            classHV[ind][jnd]=int(CH[ind][jnd] >= (labels.at(ind) / 2));

        }
    }
    fin.close();
    
        //test to see if ClassHV is being populated properly;
    
        int CH_test=0;
        for(int j = 0; j < D; j++)
        {
            CH_test=CH_test+ (classHV[1][j]^classHV[0][j]) ; //FIX ME: print out all 0s
        }
        cout << "checkPoint9.."<<endl;
        cout << "CH_test.."<< endl;
        cout << CH_test<< endl;
        time(&end_Train);
        cout <<"Training time="<<end_Train-begin_Train<<"seconds"<<endl;
    
       
    //testing
    cout << "testing..." << endl;
    
    time_t begin_Test, end_Test;
    time(&begin_Test);
    
    row = 0;
    fin.open("mnist_test.csv");
    if(!fin.is_open())
    {
        cout << "Error: Can't open " + testCSV << endl;
    }
    else
    {
        while(!fin.eof())
        {
            
            if(row > numTestSamples)
            {
                break;
            }
            
            fin >> testset_labels.at(row);
            //fin.ignore(50000000, '\n');
            testset_array.push_back(rowArray);
            
            for(int col = 0; col < numFeatures; col++)
            {
                fin.ignore(50000000, ',');
                fin >> testset_array[row][col];
                //cout << trainset_array[row][col] << " ";
            }
            row++;
        }
    }
    
    vector<int> predicted(numTestSamples); //predicted class vector
    
    vector<int>  qHV(D,0);
    vector<int>  qXorHV(D,0);
    vector<int>  qResult(numClasses,0);
    
    for(int iTest = 0; iTest < numTestSamples; iTest++)
    {
        for(int jTest = 0; jTest < numFeatures; jTest++)
        {
            indMin=0;
            quantMin = abs(testset_array[iTest][jTest]-L[0]);
            for (i=1; i<M;i++)
            {
              qcheck=abs(testset_array[iTest][jTest]-L[i]);
              if (quantMin > qcheck)
              {
                quantMin=qcheck;
                indMin=i;
              }
            }
               
                   
            transform(LD[indMin].begin(), LD[indMin].end(), ID[jTest].begin(), qXorHV.begin(), bit_xor<int>());
            
            transform (qHV.begin(), qHV.end(), qXorHV.begin(), qHV.begin(), plus<int>());
        }
         
        for(int jD = 0; jD < D; jD++)
        {
          if(qHV[jD] >= (numFeatures / 2))
          {
            qHV[jD] = 1;
          }
          else
          {
            qHV[jD] = 0;
          }
        }
        int testIndex = 0;
        transform(qHV.begin(), qHV.end(), classHV[0].begin(), qXorHV.begin(), bit_xor<int>());
        qResult[0] = accumulate(qXorHV.begin(), qXorHV.end(), 0);
        int minResult=qResult[0];
        for(int c = 1; c < numClasses; c++)
        {
            transform(qHV.begin(), qHV.end(), classHV[c].begin(), qXorHV.begin(), bit_xor<int>());
            qResult[c] = accumulate(qXorHV.begin(), qXorHV.end(), 0);
            if (qResult[c]<minResult)
            {
              testIndex = c;
              minResult=qResult[c];
            }
        }
        predicted[iTest] = classes.at(testIndex);
        if(predicted[iTest] == testset_labels.at(iTest))
        {
          accuracy += 1;
        }
        cout<<accuracy/(iTest+1)<<endl;
    }
    accuracy = (accuracy / numTestSamples)*100;
    cout << "accuracy: " << accuracy <<"%"<< endl;
    
    time(&end_Test);
    cout <<"Testing time="<<end_Test-begin_Test<<"seconds"<<endl;
    cout <<"Total Execution time="<<end_Test-begin_L<<"seconds"<<endl;
    
    return 0;
}
    
vector<int> linspace(int start_in, int end_in, int num_in)
{
    vector<int> linspaced;
    
    int start = start_in;
    int end = end_in;
    int num = num_in;
    
    if(num == 0)
    {
        return linspaced;
    }
    
    if(num == 1)
    {
        linspaced.push_back(start);
        return linspaced;
    }
    
    int delta = (end - start) / (num - 1);
    
    for(int i = 0; i < num - 1; i++)
    {
        linspaced.push_back(start + delta * i);
    }
    linspaced.push_back(end);
        
    
    return linspaced;
}

void printLinspace(vector<int> v)
{
    cout << "size: " << v.size() << endl;
    for(int i=0; i< v.size(); i++)
    {
        cout << v[i] << " ";
    }
    cout << endl;
}

vector<vector<int> > zeros(int row, int col)
{
    vector<vector<int> > zeroArray(row, vector<int>(col ,0));
    
    return zeroArray;
}


vector<vector<int> > ones(int row, int col)
{
    vector<vector<int> > zeroArray(row, vector<int>(col ,1));
    
    return zeroArray;
}

int getNum(vector<int> v)
{
    int n = static_cast<int>(v.size());
    srand(time(NULL));
    
    int index = rand() % n;
    int num = v[index];
    
    swap(v[index], v[n - 1]);
    v.pop_back()   ;
    
    return num;
}

vector<int> randperm(int num)
{
    vector<int> v(num);
    
    for(int i = 0; i < num; i++)
    {
        v[i] = i ;
    }
    
    return v;
}
