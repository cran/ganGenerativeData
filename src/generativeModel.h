// Copyright 2021 Werner Mueller
// Released under the GPL (>= 2)

#ifndef DATA_MODEL
#define DATA_MODEL

#include <algorithm>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <iterator>
#include <sstream>
#include "inOut.h"
#include "utils.h"
#include "dataSource.h"

using namespace std;

const string csSeparator = "_";
const string cDataFileExtension = "data-00000-of-00001";
const string cIndexFileExtension = "index";
const string cInvalidLevel = "Invalid level";

const string cGenerativeModelTypeId = "ae82c484-a137-4a86-beef-441b75ed9096";

class TrainedModel {
public:
    TrainedModel(): _separator(csSeparator), _period(cPeriod), _dataFileExtension(cDataFileExtension), _indexFileExtension(cIndexFileExtension) {
        ;
    }
    void readVectors(const string& modelName) {
        readVector(BuildFileName()(modelName, _dataFileExtension), _dataVector);
        readVector(BuildFileName()(modelName, _indexFileExtension), _indexVector);
    }
    void writeVectors(const string& modelName) {
        writeVector(BuildFileName()(modelName, _dataFileExtension), _dataVector);
        writeVector(BuildFileName()(modelName, _indexFileExtension), _indexVector);
    }
    
    void readVector(const string& inFileName, vector<unsigned char>& dataVector) {
        ifstream inFile;
        inFile.open(inFileName.c_str(), ios::binary | ios::ate);
        if(!inFile.is_open()) {
            throw string("File could not be opened");
        }
        
        streamsize size = inFile.tellg();
        inFile.seekg(0, ios::beg);
        dataVector.resize(size, 0);
        inFile.read((char *)dataVector.data(), size);
        inFile.close();
    }
    void writeVector(const string& outFileName, vector<unsigned char>& dataVector) {
        ofstream outFile;
        outFile.open(outFileName.c_str(), ios::binary);
        if(!outFile.is_open()) {
            throw string("File could not be opened");
        }
        
        streamsize size = dataVector.size();
        outFile.write((char *)dataVector.data(), size);
        outFile.close();
    }
    void write(ofstream& os) {
        InOut::Write(os, _dataVector);
        InOut::Write(os, _indexVector);
    }
    void read(ifstream& is) {
        InOut::Read(is, _dataVector);
        InOut::Read(is, _indexVector);
    }
    
private:
    string _dataFileName;
    string _indexFileNmae;
    
    vector<unsigned char> _dataVector;
    vector<unsigned char> _indexVector;
    
    string _separator;
    string _period;
    string _dataFileExtension;
    string _indexFileExtension;
};

class GenerativeModel {
public:
    GenerativeModel(): _typeId(cGenerativeModelTypeId), _version(1), _numberOfTrainingIterations(0), _numberOfInitializationIterations(0), _numberOfHiddenLayerUnits(), _learningRate(), _dropout(0) {
    }
    GenerativeModel(DataSource& dataSource): _typeId(cGenerativeModelTypeId), _version(1), _numberOfTrainingIterations(0), _numberOfInitializationIterations(0), _numberOfHiddenLayerUnits(), _learningRate(), _dropout(0), _dataSource(dataSource) {
    }
    void writeWithReadingTrainedModel(ofstream& os, const string& modelName, int version = 1) {
        _trainedModel.readVectors(modelName);
        
        write(os, modelName, version);
    }
    void write(ofstream& os, const string& modelName, int version = 1, int numberOfIterations = 0) {
        InOut::Write(os, _typeId);
        InOut::Write(os, version);
        
        //_numberOfIterations += numberOfIterations;
        InOut::Write(os, _numberOfTrainingIterations);
        InOut::Write(os, _numberOfInitializationIterations);
        InOut::Write(os, _numberOfHiddenLayerUnits);
        InOut::Write(os, _learningRate);
        InOut::Write(os, _dropout);
        
        _dataSource.write(os);
        
        _trainedModel.write(os);
    }
    void read(ifstream& is, const string& modelName) {
        InOut::Read(is, _typeId);
        if(_typeId != cGenerativeModelTypeId) {
            throw string(cInvalidTypeId);
        }
        InOut::Read(is, _version);
        
        InOut::Read(is, _numberOfTrainingIterations);
        InOut::Read(is, _numberOfInitializationIterations);
        InOut::Read(is, _numberOfHiddenLayerUnits);
        InOut::Read(is, _learningRate);
        InOut::Read(is, _dropout);

        _dataSource.read(is);

        _trainedModel.read(is);
        
        _trainedModel.writeVectors(modelName);
    }
    
    int getNumberOfTrainingIterations() {
        return _numberOfTrainingIterations;
    }
    void setNumberOfTrainingIterations(int numberOfTrainingIterations) {
        _numberOfTrainingIterations = numberOfTrainingIterations;
    }
    
    int getNumberOfInitializationIterations() {
        return _numberOfInitializationIterations;
    }
    void setNumberOfInitializationIterations(int numberOfInitializationIterations) {
        _numberOfInitializationIterations = numberOfInitializationIterations;
    }
    
    int getNumberOfHiddenLayerUnits() {
        return _numberOfHiddenLayerUnits;
    }
    void setNumberOfHiddenLayerUnits(int numberOfHiddenLayerUnits) {
        _numberOfHiddenLayerUnits = numberOfHiddenLayerUnits;
    }
    
    float getLearningRate() {
        return _learningRate;
    }
    void setLearningRate(float learningRate) {
        _learningRate = learningRate;
    }
    
    float getDropout() {
        return _dropout;
    }
    void setDropout(float dropout) {
        _dropout = dropout;
    }
    
    DataSource& getDataSource() {
        return _dataSource;
    }
    TrainedModel& getTrainedModel() {
        return _trainedModel;
    }
    
private:
    string _typeId;
    int _version;
    int _numberOfTrainingIterations;
    int _numberOfInitializationIterations;
    int _numberOfHiddenLayerUnits;
    float _learningRate;
    float _dropout;
            
    DataSource _dataSource;
    TrainedModel _trainedModel;
};

#endif
