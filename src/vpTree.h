// Copyright 2021 Werner Mueller
// Released under the GPL (>= 2)

#ifndef VP_TREE
#define VP_TREE

#include <queue>
#include <random>
#include <cmath>

#include "utils.h"
#include "progress.h"
#include "generativeData.h"

#define GD_RCPP

#ifdef GD_RCPP
#include <Rcpp.h>
#endif

using namespace std;

const string cDifferentSizes = "Sizes of vectors are different";
const string cNearestNeighborDifferent = "Nearest neighbor is different";
int cMaxNearestNeighbors = 128;

struct LpDistance{
    LpDistance() {}
    virtual ~LpDistance() {
    }
    virtual float operator()(const vector<float>& a, const vector<float>& b) = 0;
    virtual LpDistance& assign(const LpDistance& lpDistance) = 0;
};

struct L1Distance : public LpDistance {
    float operator()(const vector<float>& a, const vector<float>& b) {
        if(a.size() != b.size()) {
            throw string(cDifferentSizes);
        }
        float d = 0.0;
        for(int i = 0; i < a.size(); i++) {
            d += abs(a[i] - b[i]);
        }
        return d;
    }
    LpDistance& assign(const LpDistance& lpDistance) {
        *this = dynamic_cast<const L1Distance&>(lpDistance);
        return *this;
    }
};

struct L2Distance : public LpDistance {
    float operator()(const vector<float>& a, const vector<float>& b) {
        if(a.size() != b.size()) {
            throw string(cDifferentSizes);
        }
        float d = 0.0;
        for(int i = 0; i < a.size(); i++) {
            d += (a[i] - b[i]) * (a[i] - b[i]);
        }
        return sqrt(d);
    }
    LpDistance& assign(const LpDistance& lpDistance) {
        *this = dynamic_cast<const L2Distance&>(lpDistance);
        return *this;
    }
};

struct L2DistanceNan : public LpDistance {
    float operator()(const vector<float>& a, const vector<float>& b) {
        if(a.size() != b.size()) {
            throw string(cDifferentSizes);
        }
        float d = 0.0;
        for(int i = 0; i < a.size(); i++) {
            if(isnan(a[i]) || isnan(b[i])) {
                continue;
            }
            d += (a[i] - b[i]) * (a[i] - b[i]);
        }
        return sqrt(d);
    }
    LpDistance& assign(const LpDistance& lpDistance) {
        *this = dynamic_cast<const L2DistanceNan&>(lpDistance);
        return *this;
    }
};

struct L2DistanceNanIndexed : public LpDistance {
    L2DistanceNanIndexed(const vector<float>& distance): _distance(distance) {
    }
    L2DistanceNanIndexed(const L2DistanceNanIndexed& l2DistanceNanIndexed): _distance(l2DistanceNanIndexed._distance) {
    }
    float operator()(const vector<float>& a, const vector<float>& b) {
        if(a.size() != _distance.size() || b.size() != _distance.size()) {
            throw string(cDifferentSizes);
        }
        float d = 0.0;
        for(int i = 0; i < a.size(); i++) {
            if(isnan(_distance[i])) {
                continue;
            }
            d += (a[i] - b[i]) * (a[i] - b[i]);
        }
        return sqrt(d);
    }
    LpDistance& assign(const LpDistance& lpDistance) {
        *this = dynamic_cast<const L2DistanceNanIndexed&>(lpDistance);
        return *this;
    }
    vector<float> _distance;
};

class VpTreeData {
public:
    VpTreeData() {
    }
    virtual ~VpTreeData() {
    }
    
    virtual vector<float>& getNumberVector(int i) = 0;
    virtual int getSize() = 0;
};

class VpGenerativeData : public VpTreeData {
public:
    VpGenerativeData(GenerativeData& generativeData): _pGenerativeData(&generativeData) {
    }
    VpGenerativeData(const VpGenerativeData& vpGenerativeData): _pGenerativeData(vpGenerativeData._pGenerativeData) {
    }
    
    virtual vector<float>& getNumberVector(int i) {
        return  _pGenerativeData->getNormalizedNumberVectorReference(i);
    }
    virtual int getSize() {
        return _pGenerativeData->getNormalizedSize();  
    }
    
private:
    GenerativeData* _pGenerativeData;
};

struct Distance {
    Distance(VpTreeData& vpTreeData, LpDistance& lpDistance): _vpTreeData(vpTreeData), _lpDistance(lpDistance) {}
    float operator()(const int& a, const int& b) {
        vector<float>& aNumberVector = _vpTreeData.getNumberVector(a);
        vector<float>& bNumberVector = _vpTreeData.getNumberVector(b);
        return _lpDistance(aNumberVector, bNumberVector);
    }
  
    VpTreeData& _vpTreeData;
    LpDistance& _lpDistance;
};

struct VpDistance {
    VpDistance(VpTreeData& vpTreeData, int index, LpDistance& lpDistance): _vpTreeData(vpTreeData), _index(index), _lpDistance(lpDistance) {}
    bool operator()(const int& a, const int& b) {
        vector<float>& aNumberVector = _vpTreeData.getNumberVector(a);
        vector<float>& bNumberVector = _vpTreeData.getNumberVector(b);
        vector<float>& cNumberVector = _vpTreeData.getNumberVector(_index);
        if(_lpDistance(aNumberVector, cNumberVector) < _lpDistance(bNumberVector, cNumberVector)) {
            return true;
        } else {
            return false;
        }
    }

    VpTreeData& _vpTreeData;
    int _index;
    LpDistance& _lpDistance;
};

class VpElement {
public:
    VpElement(): _index(-1), _distance(0), _category(-1) {
        ;
    }
    VpElement(int index, float distance): _index(index), _distance(distance), _category(-1) {
        ;
    }
    VpElement(int index, float distance, bool positive, int category): _index(index), _distance(distance), _category(category) {
        ;
    }
    
    int getIndex() const {
        return _index;
    }
    void setIndex(int index) {
        _index = index;
    }
    float getDistance() const {
        return _distance;
    }
    void setDistance(float distance) {
        _distance = distance;
    }
    int getCategory() const {
        return _category;
    }
    void setCategory(int category) {
        _category = category;
    }
    bool operator<(const VpElement& vpElement) const {
        return getDistance() < vpElement.getDistance();
    }
    
    void write(ofstream& os) {
        InOut::Write(os, _index);
        InOut::Write(os, _distance);
        InOut::Write(os, _category);
    }
    void read(ifstream& is) {
        InOut::Read(is, _index);
        InOut::Read(is, _distance);
        InOut::Read(is, _category);
    }
    
private:
    int _index;
    float _distance;
    int _category;
};

struct VpElementCompare{
    bool operator()(const VpElement& a, const VpElement& b) {
        if(a.getDistance() < b.getDistance()) {
            return true;
        }
        if(a.getDistance() == b.getDistance()) {
            if(a.getIndex() < b.getIndex()) {
                return true;
            }
        }
        return false;
    }
};

class VpNode {
public:
	VpNode(): _index(-1), _threshold(0), _pInVpNode(0), _pOutVpNode(0) {}
    ~VpNode() {
        delete _pInVpNode;
        delete _pOutVpNode;
    }
	
    int getIndex() {
        return _index;
    }
    void setIndex(int index) {
        _index = index;
    }
    float getThreshold() {
        return _threshold;
	}
    void setThreshold(float threshold) {
        _threshold = threshold;
    }
	VpNode* getInVpNode() {
		return _pInVpNode;
	}
    void setInVpNode(VpNode* pVpNode) {
        _pInVpNode = pVpNode;
    }
    VpNode* getOutVpNode() {
        return _pOutVpNode;
    }
    void setOutVpNode(VpNode* pVpNode) {
        _pOutVpNode = pVpNode;
    }
  
private:
    int _index;
    float _threshold;
	VpNode* _pInVpNode;
	VpNode* _pOutVpNode;
};

class VpTree {
public:
    VpTree(): _pVpNode(0), _pVpTreeData(0), _tau(numeric_limits<float>::max()), _pProgress(0), _pLpDistance(0) {
    }
    VpTree(VpTreeData* pVpTreeData, LpDistance* pLpDistance, Progress* pProgress): _pVpNode(0), _pVpTreeData(pVpTreeData), _tau(numeric_limits<float>::max()), _pProgress(pProgress), _pLpDistance(pLpDistance) {
    }
    ~VpTree() {
        delete _pVpNode;
    }
    VpNode* build(int lower, int upper) {
        if(_pProgress != 0) {
            (*_pProgress)(_i);
        }
        
        if(upper == lower) {
            return 0;
        }
    
        VpNode* pVpNode = new VpNode();
        pVpNode->setIndex(lower);
    
        if(upper - lower > 1) {
            int i = _uniformIntDistribution.setParameters(lower, upper - 1)();
            
            swap(_indexVector[lower], _indexVector[i]);
            int median = (upper + lower) / 2;
            nth_element(_indexVector.begin() + lower + 1,
                 _indexVector.begin() + median,
                _indexVector.begin() + upper,
                VpDistance(*_pVpTreeData, _indexVector[lower], *_pLpDistance));
          
            pVpNode->setThreshold(Distance(*_pVpTreeData, *_pLpDistance).operator()(_indexVector[lower], _indexVector[median]));
            pVpNode->setIndex(lower);
            pVpNode->setInVpNode(build(lower + 1, median));
            pVpNode->setOutVpNode(build(median, upper));
        }
        _i++;
    
        return pVpNode;
    }
    void build(VpTreeData* pVpTreeData, LpDistance* pLpDistance, Progress* pProgress) {
        delete _pVpNode;
        
        _pVpTreeData = pVpTreeData;
        _pLpDistance = pLpDistance;
        _pProgress = pProgress;
        _i = 0;
        
        _indexVector.resize(_pVpTreeData->getSize());
        for(int i = 0; i < _pVpTreeData->getSize(); i++) {
            _indexVector[i] = i;
        }
        _uniformIntDistribution.seed(23);
        _pVpNode = build(0, _indexVector.size());
        
        if(_pProgress != 0) {
            (*_pProgress)(_pVpTreeData->getSize());
        }
    }
    bool isBuilt() {
        if(_pVpNode != 0) {
            return true;
        } else {
            return false;
        }
    }
  
    vector<VpElement> kNearestNeighbors(int k, vector<VpElement>& nearestNeighbors) {
        if(nearestNeighbors.size() <= k) {
            return nearestNeighbors;
        }
        
        vector<VpElement> kNearestNeighbors;
        for(int i = 0; i < k; i++) {
            int r = _uniformIntDistribution.setParameters(0, nearestNeighbors.size() - 1)();
            kNearestNeighbors.push_back(nearestNeighbors[r]);
            nearestNeighbors.erase(nearestNeighbors.begin() + r);
        }
        
        VpElementCompare vpElementCompare;
        sort(kNearestNeighbors.begin(), kNearestNeighbors.end(), vpElementCompare);
        
        return kNearestNeighbors;
    }
    void search(const vector<float>& target, int k, vector<VpElement>& nearestNeighbors) {
        priority_queue<VpElement> priorityQueue;
        _tau = numeric_limits<float>::max();
        _unique.clear();
        search(_pVpNode, target, k, priorityQueue);
        
        nearestNeighbors.clear();
        while(!priorityQueue.empty()) {
            nearestNeighbors.push_back(priorityQueue.top());
            priorityQueue.pop();
        }
        reverse(nearestNeighbors.begin(), nearestNeighbors.end());
        
        nearestNeighbors = kNearestNeighbors(k, nearestNeighbors);
    }

    void search(VpNode* pVpNode, const vector<float>& target, int k, priority_queue<VpElement>& priorityQueue) {
        if(pVpNode == 0) {
            return;
        }
        
        vector<float>& numberVector = _pVpTreeData->getNumberVector(_indexVector[pVpNode->getIndex()]);
        float d = (*_pLpDistance)(numberVector, target);
        if(d <= _tau) {
            _unique.insert(d);
            if(_unique.size() > k || priorityQueue.size() > cMaxNearestNeighbors) {
                float tau = priorityQueue.top().getDistance();
                while(!priorityQueue.empty() && priorityQueue.top().getDistance() == tau) {
                    priorityQueue.pop();
                }
                _unique.erase(tau);
                priorityQueue.push(VpElement(_indexVector[pVpNode->getIndex()], d));   
                _tau = priorityQueue.top().getDistance();
            } else {
                priorityQueue.push(VpElement(_indexVector[pVpNode->getIndex()], d));
            }
        }
        
        if(d < pVpNode->getThreshold()) {
            search(pVpNode->getInVpNode(), target, k, priorityQueue);
            if(d + _tau >= pVpNode->getThreshold()) {
                search(pVpNode->getOutVpNode(), target, k, priorityQueue);
            }
        } else if(d == pVpNode->getThreshold()) {
            search(pVpNode->getInVpNode(), target, k, priorityQueue);
            search(pVpNode->getOutVpNode(), target, k, priorityQueue);
        } else if(d > pVpNode->getThreshold()) {
            search(pVpNode->getOutVpNode(), target, k, priorityQueue);
            if(d - _tau <= pVpNode->getThreshold()) {
                search(pVpNode->getInVpNode(), target, k, priorityQueue);
            }
        }
    }
    void linearSearch(const vector<float>& target, int k, vector<VpElement>& nearestNeighbors) {
        priority_queue<VpElement> priorityQueue;
        float tau = numeric_limits<float>::max();
        _unique.clear();
        for(int i = 0; (int)i < _pVpTreeData->getSize(); i++) {
            vector<float> numberVector = _pVpTreeData->getNumberVector(i);
            float d = (*_pLpDistance)(numberVector, target);
            if(d <= _tau) {
                _unique.insert(d);
                if(_unique.size() > k) {
                    float tau = priorityQueue.top().getDistance();
                    while(!priorityQueue.empty() && priorityQueue.top().getDistance() == tau) {
                        priorityQueue.pop();
                    }
                    _unique.erase(tau);
                    priorityQueue.push(VpElement(i, d));  
                    _tau = priorityQueue.top().getDistance();
                    
                } else {
                    priorityQueue.push(VpElement(i, d));      
                }
            }
        }
        nearestNeighbors.clear();
        while(!priorityQueue.empty()) {
            nearestNeighbors.push_back(priorityQueue.top());
            priorityQueue.pop();
        }
        reverse(nearestNeighbors.begin(), nearestNeighbors.end());
        
        nearestNeighbors = kNearestNeighbors(k, nearestNeighbors);
    }
   
    void test(int begin, int end, int nNearestNeighbors) {
        if(begin >= _pVpTreeData->getSize())
        {
            begin = _pVpTreeData->getSize();
        }
        if(end > _pVpTreeData->getSize())
        {
            end = _pVpTreeData->getSize();
        }
        for(int i = begin; i < end; i++) {
            vector<float> numberVector = _pVpTreeData->getNumberVector(i);
            vector<VpElement> nearestNeighbors;
            search(numberVector, nNearestNeighbors, nearestNeighbors);
            
            vector<VpElement> lNearestNeighbors;
            linearSearch(numberVector, nNearestNeighbors, lNearestNeighbors);
            
            for(int j = 0; j < (int)nearestNeighbors.size(); j++) {
                if(nearestNeighbors[j].getDistance() != lNearestNeighbors[j].getDistance()) {
                    throw string(cNearestNeighborDifferent);
                }
            }
        }
    }
    VpTreeData& getVpTreeData() {
        return *_pVpTreeData;
    }
    LpDistance& getLpDistance() {
        return *_pLpDistance;
    }
    
private:
    vector<int> _indexVector;
    VpNode* _pVpNode;
    VpTreeData* _pVpTreeData;
    float _tau;
    Progress* _pProgress;
    LpDistance* _pLpDistance;
    
    set<float> _unique;
    int _i;
    
    UniformIntDistribution _uniformIntDistribution;
};

#endif
