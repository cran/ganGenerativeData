#ifndef NUMBER_ARRAY_COLUMN
#define NUMBER_ARRAY_COLUMN

#include "inOut.h"
#include "numberColumn.h"
#include "stringColumn.h"

using namespace std;

class NumberArrayColumn : public Column{
public:
    NumberArrayColumn(const COLUMN_TYPE& type, int size):Column(type, BINARY) {
         _numberColumnArray.resize(size, NumberColumn(NUMERICAL));
    }
    NumberArrayColumn(const COLUMN_TYPE& type, const wstring& name, int size): Column(type, BINARY, name, true) {
         _numberColumnArray.resize(size, NumberColumn(NUMERICAL));
    }
    NumberArrayColumn(const COLUMN_TYPE& type, const SCALE_TYPE& scaleType, const wstring& name, bool active, int size): Column(type, scaleType, name, active) {
        _numberColumnArray.resize(size, NumberColumn(NUMERICAL));
    }
    NumberArrayColumn(const NumberArrayColumn& numberArrayColumn): Column(numberArrayColumn.getColumnType(), numberArrayColumn.getScaleType(), numberArrayColumn.getName(), numberArrayColumn.getActive()) {
        _numberColumnArray.resize(numberArrayColumn.getDimension(), NumberColumn(NUMERICAL));
    } 

    virtual void clear() {
        _numberColumnArray.clear();
    }

    void setColumnNames(const vector<wstring> columnNames) {
        for(int i = 0; i < (int)_numberColumnArray.size(); i++) {
            _numberColumnArray[i].setName(columnNames[i]);
        }
        setValueMap();
    }
    const vector<NumberColumn>& getNumberColumnArray() const {
        return _numberColumnArray;
    }
    
  	virtual void addValue(const vector<float>& valueVector, int offset) {
	    for(int i = 0; i < (int)_numberColumnArray.size(); i++) {
		      _numberColumnArray[i].addValue(valueVector[offset + i]);
        }
    }
    virtual void addNormalizedValue(const vector<float>& valueVector, int offset) {
        for(int i = 0; i < (int)_numberColumnArray.size(); i++) {
            _numberColumnArray[i].addNormalizedValue(valueVector[offset + i]);
        }
    }
    virtual vector<float> getNumberVector(int i) {
        if(i < 0 || i > (getSize() - 1)) {
            throw cInvalidIndex;
        }

        vector<float> numberVector;
        for(int j = 0; j < (int)_numberColumnArray.size(); j++) {
            numberVector.push_back(_numberColumnArray[j].getValueVector()[i]);
        }
        return numberVector;
    }
    virtual vector<float> getNormalizedNumberVector(int i) {
        if(i < 0 || i > (getNormalizedSize() - 1)) {
            throw cInvalidIndex;
        }

        vector<float> numberVector;
		for(int j = 0; j < (int)_numberColumnArray.size(); j++) {
            float value = _numberColumnArray[j].getNormalizedValueVector()[i];
            /*
            if(_scaleType == BINARY) {
                if(value >= 0.5) {
                value = 1;
            } else {
            value = 0;
            }
            */
            numberVector.push_back(value);
        }
        return numberVector;
    }
    virtual vector<float> getDenormalizedNumberVector(int i) {
        return getNormalizedNumberVector(i);
    }
    virtual int getSize() {
        if(_numberColumnArray.size() > 0)	{
            return _numberColumnArray[0].getSize();
        } else {
            return 0;
	    }
	}
    virtual int getNormalizedSize() {
        if(_numberColumnArray.size() > 0)	{
            return _numberColumnArray[0].getNormalizedSize();
        } else {
            return 0;
        }
    }
    virtual int getDimension() const {
        return _numberColumnArray.size();
    }
    
    float getMax() const {
        float max = 1;
        if(_scaleType == BINARY) {
            max = 1;
        } else {
            throw string( cInvalidScaleType);
        }
        return max;
    }
    float getMin() const {
        float min = 0;
        if(_scaleType == BINARY) {
            min = 0;
        } else {
            throw string( cInvalidScaleType);
        }
        return min;
    }
    
    wstring getMaxValue(int i) {
        vector<float> numberVector = getNormalizedNumberVector(i);
        float max = 0;
        int index = -1;
        for(int j = 0; j < (int) numberVector.size(); j++) {
            if(numberVector[j] > max) {
                max = numberVector[j];
                index = j;
            }
        }
        
        if(index != - 1 && max >= 0.5) {
            return _numberColumnArray[index].getName();
        } else {
            return cNA;  
        }
    }
    
    wstring getMaxValue(vector<float>& numberVector) {
        float max = 0;
        int index = -1;
        for(int j = 0; j < (int) numberVector.size(); j++) {
            if(numberVector[j] > max) {
                max = numberVector[j];
                index = j;
            }
        }
        
        if(index != - 1 && max >= 0.5) {
            return _numberColumnArray[index].getName();
        } else {
            return cNA;  
        }
    }
    
    virtual void write(ofstream& os) {
        Column::write(os);
        InOut::Write(os, _valueMap);
        
        int size = _numberColumnArray.size();
        InOut::Write(os, size);
        for(int i = 0; i < (int)_numberColumnArray.size(); i++) {
            _numberColumnArray[i].write(os);
        }
    }
    
    virtual void read(ifstream& is) {
        Column::read(is);
        InOut::Read(is, _valueMap);
        
        int size = 0;
        InOut::Read(is, size);
        _numberColumnArray.resize(size);
        for(int i = 0; i < (int)_numberColumnArray.size(); i++) {
            _numberColumnArray[i].read(is);
        }
    }
    
    map<wstring, int> const & getValueMap() const {
        return _valueMap;
    }
    
    void setValueMap() {
        for(int i = 0; i < (int)_numberColumnArray.size(); i++) {
            wstring name = _numberColumnArray[i].getName();
            _valueMap[name] = i;
        }
    }
    
    virtual vector<float> getNormalizedNumberVector(wstring value) {
        vector<float> numberVector(_numberColumnArray.size(), 0);
        map<wstring, int>::const_iterator valueMapIter;
        valueMapIter = _valueMap.find(value);
        if(valueMapIter != _valueMap.end()) {
            numberVector[valueMapIter->second] = 1;
        }
        
        return numberVector;
    }
    
private:
    map<wstring, int> _valueMap;
    vector<NumberColumn> _numberColumnArray;
};

#endif
