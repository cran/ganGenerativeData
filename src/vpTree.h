// Copyright 2021 Werner Mueller
// Released under the GPL (>= 2)

#ifndef VP_TREE
#define VP_TREE

#include <queue>
#include <random>

#include "inOut.h"
#include "progress.h"
#include "generativeData.h"

#define GD_RCPP

#ifdef GD_RCPP
#include <Rcpp.h>
#endif

using namespace std;

const string cDifferentSizes = "Sizes of vectors are different";
const string cNearestNeighbourDifferent = "Nearest neighbour is different";

struct LpDistance{
    LpDistance() {}
    virtual float operator()(const vector<float>& a, const vector<float>& b) = 0;
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
};
 
class VpTreeData {
public:
    VpTreeData() {
        ;
    };
    virtual vector<float> getNumberVector(int i) = 0;
    virtual int getSize() = 0;
};

class VpGenerativeData : public VpTreeData {
public:
    VpGenerativeData(GenerativeData& generativeData): _generativeData(generativeData) {
        ;
    }
    
    virtual vector<float> getNumberVector(int i) {
        return  _generativeData.getNormalizedNumberVector(i);
    };
    virtual int getSize() {
        return _generativeData.getNormalizedSize();  
    };
    
private:
    GenerativeData& _generativeData;
};

struct Distance {
    Distance(VpTreeData& vpTreeData, LpDistance& lpDistance): _vpTreeData(vpTreeData), _lpDistance(lpDistance) {}
    float operator()(const int& a, const int& b) {
        vector<float> aNumberVector;
        vector<float> bNumberVector;
        aNumberVector = _vpTreeData.getNumberVector(a);
        bNumberVector = _vpTreeData.getNumberVector(b);
        return _lpDistance(aNumberVector, bNumberVector);
    }
  
    VpTreeData& _vpTreeData;
    LpDistance& _lpDistance;
};

struct VpDistance {
    VpDistance(VpTreeData& vpTreeData, int index, LpDistance& lpDistance): _vpTreeData(vpTreeData), _index(index), _lpDistance(lpDistance) {}
    bool operator()(const int& a, const int& b) {
        vector<float> aNumberVector;
        vector<float> bNumberVector;
        vector<float> cNumberVector;
        aNumberVector = _vpTreeData.getNumberVector(a);
        bNumberVector = _vpTreeData.getNumberVector(b);
        cNumberVector = _vpTreeData.getNumberVector(_index);
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
    
    void write3(ofstream& os) {
        InOut::Write(os, _index);
        InOut::Write(os, _distance);
        InOut::Write(os, _category);
    }
    void read3(ifstream& is) {
        InOut::Read(is, _index);
        InOut::Read(is, _distance);
        InOut::Read(is, _category);
    }
    
private:
    int _index;
    float _distance;
    int _category;
};

class VpTree {
public:
    VpTree(VpTreeData& vpTreeData, LpDistance& lpDistance, Progress* pProgress): _pVpNode(0), _vpTreeData(vpTreeData), _tau(numeric_limits<float>::max()), _pProgress(pProgress), _lpDistance(lpDistance), _pG(new mt19937(_rd())), _pR(0) {
    }
    ~VpTree() {
        delete _pVpNode;
        delete _pR;
        delete _pG;
    }
    
    VpNode* build(int lower, int upper) {
        if(_pProgress != 0) {
            (*_pProgress)(0);
        }
        
        if(upper == lower) {
            return 0;
        }
    
        VpNode* pVpNode = new VpNode();
        pVpNode->setIndex(lower);
    
        if(upper - lower > 1) {
            int i = (int)((float)(*_pR)(*_pG) / (float)(_vpTreeData.getSize() - 1) * (float)(upper - lower - 1)) + lower;
            swap(_indexVector[lower], _indexVector[i]);
            int median = (upper + lower) / 2;
            nth_element(_indexVector.begin() + lower + 1,
                 _indexVector.begin() + median,
                _indexVector.begin() + upper,
                VpDistance(_vpTreeData, _indexVector[lower], _lpDistance));
          
            pVpNode->setThreshold(Distance(_vpTreeData, _lpDistance).operator()(_indexVector[lower], _indexVector[median]));
            pVpNode->setIndex(lower);
            pVpNode->setInVpNode(build(lower + 1, median));
            pVpNode->setOutVpNode(build(median, upper));
        }
    
        return pVpNode;
    }
    void build() {
        if(_pProgress != 0) {
            (*_pProgress)(0);
        };
        
        _indexVector.resize(_vpTreeData.getSize());
        for(int i = 0; i < _vpTreeData.getSize(); i++) {
            _indexVector[i] = i;
        }
        _pR = new uniform_int_distribution<int>(0, _vpTreeData.getSize() - 1);
        _pVpNode = build(0, _indexVector.size());
    }
    bool isBuilt() {
        if(_pVpNode != 0) {
            return true;
        } else {
            return false;
        }
    }
  
    void search(const vector<float>& target, int k, vector<VpElement>& nearestNeighbours) {
        priority_queue<VpElement> priorityQueue;
        _tau = numeric_limits<float>::max();
        search(_pVpNode, target, k, priorityQueue);
        
        nearestNeighbours.clear();
        while(!priorityQueue.empty()) {
            nearestNeighbours.push_back(priorityQueue.top());
            priorityQueue.pop();
        }
        reverse(nearestNeighbours.begin(), nearestNeighbours.end());
    }

    void search(VpNode* pVpNode, const vector<float>& target, int k, priority_queue<VpElement>& priorityQueue) {
        if(pVpNode == 0) {
            return;
        }
        
        vector<float> numberVector = _vpTreeData.getNumberVector(_indexVector[pVpNode->getIndex()]);
        float d = _lpDistance(numberVector, target);
        
        if(d < _tau) {
            if(priorityQueue.size() == k) {
                priorityQueue.pop();
            }
            priorityQueue.push(VpElement(_indexVector[pVpNode->getIndex()], d));
            if(priorityQueue.size() == k) {
                _tau = priorityQueue.top().getDistance();
            }
        }
        
        if(d < pVpNode->getThreshold()) {
            search(pVpNode->getInVpNode(), target, k, priorityQueue);
            if(d + _tau >= pVpNode->getThreshold()) {
                search(pVpNode->getOutVpNode(), target, k, priorityQueue);
            }
        } else {
            search(pVpNode->getOutVpNode(), target, k, priorityQueue);
            if(d - _tau < pVpNode->getThreshold()) {
                search(pVpNode->getInVpNode(), target, k, priorityQueue);
            }
        }
    }
    void linearSearch(const vector<float>& target, int k, vector<VpElement>& nearestNeighbours) {
        priority_queue<VpElement> priorityQueue;
        float tau = numeric_limits<float>::max();
        for(int i = 0; (int)i < _vpTreeData.getSize(); i++) {
            vector<float> numberVector = _vpTreeData.getNumberVector(i);
            float d = _lpDistance(numberVector, target);
            
            if(d < tau) {
                if(priorityQueue.size() == k) {
                    priorityQueue.pop();
                }
                priorityQueue.push(VpElement(i, d));
                if(priorityQueue.size() == k) {
                    tau = priorityQueue.top().getDistance();
                }
            }
        }
        
        nearestNeighbours.clear();
        while(!priorityQueue.empty()) {
            nearestNeighbours.push_back(priorityQueue.top());
            priorityQueue.pop();
        }
        reverse(nearestNeighbours.begin(), nearestNeighbours.end());
    }
    
    void test(int begin, int end, int nNearestNeighbours) {
        if(begin >= _vpTreeData.getSize())
        {
            begin = _vpTreeData.getSize();
        }
        if(end > _vpTreeData.getSize())
        {
            end = _vpTreeData.getSize();
        }
        for(int i = begin; i < end; i++) {
            vector<float> numberVector = _vpTreeData.getNumberVector(i);
            vector<VpElement> nearestNeighbours;
            search(numberVector, nNearestNeighbours, nearestNeighbours);
            
            vector<VpElement> lNearestNeighbours;
            linearSearch(numberVector, nNearestNeighbours, lNearestNeighbours);
            
            for(int j = 0; j < (int)nearestNeighbours.size(); j++) {
                if(nearestNeighbours[j].getDistance() != lNearestNeighbours[j].getDistance()) {
                    throw string(cNearestNeighbourDifferent);
                }
            }
        }
    }
    VpTreeData& getVpTreeData() {
        return _vpTreeData;
    }
    
private:
    vector<int> _indexVector;
    VpNode* _pVpNode;
    VpTreeData& _vpTreeData;
    float _tau;
    Progress* _pProgress;
    LpDistance& _lpDistance;
    
    random_device _rd;
    mt19937* _pG;
    uniform_int_distribution<int>* _pR;
};

#endif
