// Copyright 2021 Werner Mueller
// Released under the GPL (>= 2)

#ifndef DENSITY
#define DENSITY

#include <queue>

#include "inOut.h"
#include "generativeData.h"
#include "vpTree.h"
#include "normalizeData.h"

const string cInvalidDensiyValue = "Invalid density value inf";

class Density{
public:
    Density(DataSource& dataSource, VpTree& vpTree, int nNearestNeighbors, Progress* pProgress) : _dataSource(dataSource), _vpTree(vpTree), _nNearestNeighbors(nNearestNeighbors), _pProgress(pProgress) {}
    void calculateDensityValues() {
        vector<float>& densityVector = _dataSource.getDensityVector()->getValueVector();
        densityVector.resize(_dataSource.getNormalizedSize(), 0);
        
        for(int i = 0; i < (int)densityVector.size(); i++) {
            if(_pProgress != 0) {
                (*_pProgress)(i);
            }
            
            vector<float> numberVector = _dataSource.getNormalizedNumberVector(i);
            vector<VpElement> nearestNeighbours;
            _vpTree.search(numberVector, _nNearestNeighbors, nearestNeighbours);
            
            float d = calculateDensityValue(nearestNeighbours);
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
    float calculateDensityValue(vector<VpElement> nearestNeighbours) {
        float d = 0;
        for(int i = 0; (int)i < nearestNeighbours.size(); i++) {
            d += nearestNeighbours[i].getDistance() * nearestNeighbours[i].getDistance();
        }
        d = (float)nearestNeighbours.size() / d;
        return d;
    }
    float calculateDensityValue(vector<float>& numberVector) {
        NormalizeData normalizeData;
        vector<float> normalizedNumberVector = normalizeData.getNormalizedNumberVector(_dataSource, numberVector);
        vector<VpElement> nearestNeighbours;
        if(_vpTree.isBuilt()) {
            _vpTree.search(normalizedNumberVector, _nNearestNeighbors, nearestNeighbours);    
        } else {
            _vpTree.linearSearch(normalizedNumberVector, _nNearestNeighbors, nearestNeighbours);    
        }
        float d = calculateDensityValue(nearestNeighbours);
        d = normalizeData.getNormalizedNumber(_dataSource.getDensityVector(), d, true);
        
        return d;
    }
    float calculateQuantile(float percentage) {
        vector<float>& densityVector = _dataSource.getDensityVector()->getNormalizedValueVector();
        vector<float> dV;
        dV.reserve(densityVector.size());
        dV.insert(dV.end(), densityVector.begin(), densityVector.end());
        int n = (int)ceil(percentage / (float)100 * (float)dV.size());
        nth_element(dV.begin(), dV.begin() + n, dV.end());
        return dV[n];
    }

private:
    DataSource& _dataSource;
    VpTree& _vpTree;
    int _nNearestNeighbors;
    Progress* _pProgress;
};
    
#endif
