// Copyright 2021 Werner Mueller
// Released under the GPL (>= 2)

#ifndef STRING_COLUMN
#define STRING_COLUMN

#include "utils.h"
#include "column.h"

using namespace std;

const wstring cNA = L"NA";

const string cInvalidValue = "Invalid value";

class StringColumn : public Column{
public:
	StringColumn(const COLUMN_TYPE type): Column(type, NOMINAL) {
	}
	StringColumn(const COLUMN_TYPE type, const wstring& name): Column(type, NOMINAL, name, true){
	}
	StringColumn(const StringColumn& stringColumn): Column(stringColumn.getColumnType(), stringColumn.getScaleType(), stringColumn.getName(), stringColumn.getActive()) {
	    _valueMap = stringColumn.getValueMap();
		_inverseValueMap = stringColumn.getInverseValueMap();
	}
    virtual ~StringColumn() {
    }
  
	virtual void clear() {
	    _valueVector.clear();
	}

	virtual void addValue(const wstring& value, bool addNewValue = true) {
		int n = -1;
		map<wstring, int>::iterator valueMapIterator = _valueMap.find(value);
		if(valueMapIterator != _valueMap.end()) {
			n = valueMapIterator->second;
		} else {
			if(addNewValue) {
				n = _valueMap.size() + 1;
				_valueMap[value] = n;
				_inverseValueMap[n] = value;
			} else {
				n = 0;
			}
		}
		_valueVector.push_back(n);
	}
	virtual vector<float> getNumberVector(int i) {
	    if(i < 0 || i > ((int)_valueVector.size() - 1)) {
	        throw string(cInvalidIndex);
	    }
	  
		vector<float> numberVector;
		if(_scaleType == NOMINAL) {
			numberVector.resize(_valueMap.size(), 0);
			if(_valueVector[i] > 0) {
				numberVector[_valueVector[i] - 1] = 1;
			}
		} else {
			throw string(cInvalidColumnType);
		}
		return numberVector;
	}
	virtual vector<float> getNormalizedNumberVector(int i) {
	    if(i < 0 || i > ((int)_valueVector.size() - 1)) {
	        throw string(cInvalidIndex);
	    }
	  
	    vector<float> numberVector;
	    if(_scaleType == NOMINAL) {
	        numberVector.resize(_valueMap.size(), 0);
	        int index = _valueVector[i] - 1;
	        if(_valueVector[i] == 0) {
	            index = _uniformIntDistribution() - 1;
	        }
	        numberVector[index] = 1;
	    } else {
	        throw string(cInvalidColumnType);
	    }

	    return numberVector;
	}
    virtual vector<float> getDenormalizedNumberVector(int i) {
        return  getDenormalizedNumberVector(i);
    }
	virtual int getDimension() const {
		if(_scaleType == NOMINAL) {
			return _valueMap.size();
		} else {
			throw string(cInvalidScaleType);
		}
	}
	
  	vector<int>& getValueVector() {
	    return _valueVector;
	}
  	wstring getValue(int i) {
  		if(i < 0 || i > (getSize() - 1)) {
  			throw cInvalidIndex;
  		}

  		if(_valueVector[i] == 0) {
  			return L"";
  		}

  		map<int, wstring>::const_iterator iter = _inverseValueMap.find(_valueVector[i]);
  		if(iter == _inverseValueMap.end()) {
  			throw string(cInvalidValue);
  		}
  		return iter->second;
  	}
	map<wstring, int> const & getValueMap() const {
		return _valueMap;
	}
	map<int, wstring> const & getInverseValueMap() const {
		return _inverseValueMap;
	}
	virtual int getSize() {
		return _valueVector.size();
	}
	virtual int getNormalizedSize() {
		return _valueVector.size();
	}
  
	virtual void write(ofstream& os) {
		Column::write(os);
		InOut::Write(os, _valueMap);
		InOut::Write(os, _inverseValueMap);
		InOut::Write(os, _valueVector);
	}
	virtual void read(ifstream& is) {
		Column::read(is);
		InOut::Read(is, _valueMap);
		InOut::Read(is, _inverseValueMap);
		InOut::Read(is, _valueVector);
		
		_uniformIntDistribution.setParameters(1, _valueMap.size());
	}
    
private:
	map<wstring, int> _valueMap;
	map<int, wstring> _inverseValueMap;
	vector<int> _valueVector;
	
	UniformIntDistribution _uniformIntDistribution;
};

#endif
