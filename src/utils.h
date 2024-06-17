// Copyright 2021 Werner Mueller
// Released under the GPL (>= 2)

#ifndef UTILS
#define UTILS

#include <iostream>
#include <random>

using namespace std;

const string cSeparator = "_";
const string cPeriod = ".";

class GetFileName {
public:
    string operator()(const string& name) {
        size_t dotPos = name.find_last_of(".");
        if(dotPos != string::npos && dotPos > 0) {
            return name.substr(0, dotPos);
        } else {
            return name; 
        }
    }
};

class BuildFileName {
public:
    string operator()(const string& name, const string& extension) {
        stringstream ss;
        ss << GetFileName()(name);
        if(extension != "") {
            ss << cPeriod;
            ss << extension;
        }
        
        return ss.str();
    }
};

class GetExtension {
public:
    string operator()(const string& name) {
        size_t dotPos = name.find_last_of(".");
        if(dotPos != string::npos) {
            return name.substr(dotPos + 1);
        } else {
            return ""; 
        }
    }
};

class GetValue {
public:
    string operator()(float value) {
        stringstream ss;
        ss << value;
        
        return ss.str();
    }
};

class UniformIntDistribution {
public:
    UniformIntDistribution(): _pG(new mt19937(random_device()())), _pR(0) {
        ;
    }
    ~UniformIntDistribution() {
        delete _pR;
        delete _pG;
    }
    UniformIntDistribution& setParameters(int a, int b) {
        delete _pR;
        _pR = new uniform_int_distribution<int>(a, b);
        return *this;
    }
    void seed(int i) {
        _pG->seed(i);   
    }
    int operator()() {
        return (*_pR)(*_pG);
    }
private:
    mt19937* _pG;
    uniform_int_distribution<int>* _pR;
};
  
class UniformRealDistribution {
public:
    UniformRealDistribution(): _pG(new mt19937(random_device()())), _pR(new uniform_real_distribution<float>(0, 1)) {
        ;
    }
    UniformRealDistribution(const UniformRealDistribution& uniformRealDistribution): _pG(new mt19937(random_device()())), _pR(new uniform_real_distribution<float>(0, 1)) {
        ;
    }
    ~UniformRealDistribution() {
        delete _pR;
        delete _pG;
    }
    UniformRealDistribution& setParameters(float a, float b) {
        delete _pR;
        _pR = new uniform_real_distribution<float>(a, b);
        return *this;
    }
    void seed(int i) {
        _pG->seed(i);   
    }
    float operator()() {
        return (*_pR)(*_pG);
    }
private:
    mt19937* _pG;
    uniform_real_distribution<float>* _pR;
};

class  RandomIndicesWithoutReplacement {
public:
    RandomIndicesWithoutReplacement() {
        ; 
    }
    vector<int> operator()(int size, float percent, int seed = -1) {
        vector<int> indices(size);
        for(int i = 0; i < (int)indices.size(); i++) {
            indices[i] = i;
        }
    
        vector<int> randomIndices;
        random_device rd;
        mt19937 mt(rd());
        if(seed > 0) {
            mt.seed(seed);
        }
    
        int n = round((float)size * percent / 100);
        for(int i = 0; i < n; i++) {
            if(indices.size() > 0) {
                uniform_int_distribution<int> uid(0, indices.size() - 1);
                int r = uid(mt);
                randomIndices.push_back(indices[r]);
                indices[r] = indices[indices.size() - 1];
                indices.resize(indices.size() - 1);
            }
        }
        return randomIndices;
    }
};

#endif
