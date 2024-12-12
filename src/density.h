// Copyright 2021 Werner Mueller
// Released under the GPL (>= 2)

#ifndef DENSITY
#define DENSITY

#include <queue>

#include "inOut.h"
//#include "generativeData.h"
#include "dataSource.h"
#include "vpTree.h"
#include "normalizeData.h"

const string cInvalidDensiyValue = "Invalid density value inf";

class Density{
public:
    Density(DataSource& dataSource, VpTree* vpTree, int nNearestNeighbors, Progress* pProgress) : _dataSource(dataSource), _vpTree(vpTree), _nNearestNeighbors(nNearestNeighbors), _pProgress(pProgress) {}
    
    void calculateDensityValues() {
        vector<float>& densityVector = _dataSource.getDensityVector()->getValueVector();
        densityVector.resize(_dataSource.getNormalizedSize(), 0);
        
        for(int i = 0; i < (int)densityVector.size(); i++) {
            if(_pProgress != 0) {
                (*_pProgress)(i);
            }
            
            vector<float>& numberVector = _dataSource.getNormalizedNumberVectorReference(i);
            vector<VpElement> nearestNeighbors;
            _vpTree->search(numberVector, _nNearestNeighbors, nearestNeighbors);
            
            //float d = calculateDensityValue(nearestNeighbors);
            float d = calculateKNearestNeighborDensityEstimation(nearestNeighbors, densityVector.size(), _dataSource.getDimension());
            densityVector[i] = d;
            
            if(isinf(d)) {
                throw string(cInvalidDensiyValue);
            }
        }
        
        NormalizeData normalizeData;
        normalizeData.normalize(_dataSource.getDensityVector(), true);
        _dataSource.getDensityVector()->getValueVector().clear();
        
        if(_pProgress != 0) {
            (*_pProgress)(_dataSource.getNormalizedSize());
        }
    }
    /*
    float calculateDensityValue(vector<VpElement> nearestNeighbors) {
        float d = 0;
        for(int i = 0; (int)i < (int)nearestNeighbors.size(); i++) {
            d += nearestNeighbors[i].getDistance() * nearestNeighbors[i].getDistance();
        }
        d = (float)nearestNeighbors.size() / d;
        return d;
    }
    */
    float calculateUnitSphereVolume(int dimension) {
        return powf(M_PI, (float)dimension / 2) / tgammaf((float)dimension / 2 + 1);
    }
    
    float calculateKNearestNeighborDensityEstimation(vector<VpElement> nearestNeighbors, long n, int dimension) {
        //float c = (float)nearestNeighbors.size() / (float)n * tgammaf((float)dimension / 2 + 1) / powf(M_PI, (float)dimension / 2);
        float c = (float)nearestNeighbors.size() / (float)n / calculateUnitSphereVolume(dimension);
        
        float knnDensityEstimation = 0;
        if(nearestNeighbors.size() > 0) {
            knnDensityEstimation = c / powf(nearestNeighbors[nearestNeighbors.size() - 1].getDistance(), (float) dimension);
        }
        return knnDensityEstimation;
    }
    
    float calculateDensityValue(vector<float>& numberVector) {
        NormalizeData normalizeData;
        vector<float> normalizedNumberVector = normalizeData.getNormalizedNumberVector(_dataSource, numberVector);
        vector<VpElement> nearestNeighbors;
        if(_vpTree->isBuilt()) {
            _vpTree->search(normalizedNumberVector, _nNearestNeighbors, nearestNeighbors);    
        } else {
            _vpTree->linearSearch(normalizedNumberVector, _nNearestNeighbors, nearestNeighbors);    
        }
        //float d = calculateDensityValue(nearestNeighbours);
        vector<float>& densityVector = _dataSource.getDensityVector()->getNormalizedValueVector();
        float d = calculateKNearestNeighborDensityEstimation(nearestNeighbors, densityVector.size(), _dataSource.getDimension());
        d = normalizeData.getNormalizedNumber(_dataSource.getDensityVector(), d, true);
        
        return d;
    }
    
    float calculateQuantile(float percent) {
        vector<float>& densityVector = _dataSource.getDensityVector()->getNormalizedValueVector();
        vector<float> dV;
        dV.reserve(densityVector.size());
        dV.insert(dV.end(), densityVector.begin(), densityVector.end());
        //int n = (int)ceil(percent / (float)100 * (float)dV.size());
        int n = (int)floor(percent / (float)100 * (float)dV.size()) - 1;
        if(n < 0) {
            n = 0;
        }
        if(n >= dV.size()) {
            n = dV.size() - 1;
        }
        nth_element(dV.begin(), dV.begin() + n, dV.end());
        
        return dV[n];
    }
    
    float calculateInverseQuantile(float densityValue) {
        vector<float>& densityVector = _dataSource.getDensityVector()->getNormalizedValueVector();
        if(densityVector.size() == 0) {
            return 0;
        }
        
        float k = 0;
        for(int i = 0; i < (int)densityVector.size(); i++) {
            if(densityVector[i] <= densityValue) {
                k++;
            }
        }
        
        return (float)k / (float)densityVector.size() * 100;
    }

private:
    DataSource& _dataSource;
    VpTree* _vpTree;
    int _nNearestNeighbors;
    Progress* _pProgress;
};
    
#endif
