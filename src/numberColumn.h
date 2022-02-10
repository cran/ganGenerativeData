// Copyright 2021 Werner Mueller
// Released under the GPL (>= 2)

#ifndef NUMBER_COLUMN
#define NUMBER_COLUMN

#include "column.h"

#define GD_RCPP

#ifdef GD_RCPP
#include <Rcpp.h>
#endif

using namespace std;

class NumberColumn : public Column{
public:
	NumberColumn():Column(NUMERICAL, LINEAR), _max(0), _min(0) {
	}
 	NumberColumn(const COLUMN_TYPE type):Column(type, LINEAR), _max(0), _min(0) {
	}
	NumberColumn(const COLUMN_TYPE type, const wstring& name): Column(type, LINEAR, name, true), _max(0), _min(0) {
	}
    NumberColumn(const COLUMN_TYPE type, const SCALE_TYPE scaleType, const wstring& name): Column(type, scaleType, name, true), _max(0), _min(0) {
    }
	NumberColumn(const NumberColumn& numberColumn):Column(numberColumn.getColumnType(), numberColumn.getScaleType(), numberColumn.getName(), numberColumn.getActive()), _max(numberColumn.getMax()), _min(numberColumn.getMin()) {
	}
    virtual ~NumberColumn() {
    }
  
    virtual void clear() {
	    _max = 0;
	    _min = 0;
        _valueVector.clear();
	    _normalizedValueVector.clear();
    }

	virtual void addValue(float value) {
		_valueVector.push_back(value);
	}
    virtual void addNormalizedValue(float value) {
        _normalizedValueVector.push_back(value);
    }
	virtual vector<float> getNumberVector(int i) {
		if(i < 0 || i > ((int)_valueVector.size() - 1)) {
			throw string(cInvalidIndex);
		}

		vector<float> numberVector;
		numberVector.push_back(_valueVector[i]);
		return numberVector;
	}
	virtual vector<float> getNormalizedNumberVector(int i) {
		if(i < 0 || i > ((int)_normalizedValueVector.size() - 1)) {
			throw string(cInvalidIndex);
		}

		vector<float> numberVector;
		if(_scaleType == LINEAR) {
			numberVector.push_back(_normalizedValueVector[i]);
	    } else {
	        throw cInvalidScaleType;
	    }
		return numberVector;
	}
    virtual vector<float> getDenormalizedNumberVector(int i) {
        if(i < 0 || i > ((int)_normalizedValueVector.size() - 1)) {
            throw string(cInvalidIndex);
        }
        
        vector<float> numberVector;
        if(_scaleType == LINEAR) {
            float value = _min + (_max - _min) * _normalizedValueVector[i];
            numberVector.push_back(value);
        } else {
            throw cInvalidScaleType;
        }
        return numberVector;
    }
    virtual int getSize() {
        return _valueVector.size();
    }
	virtual int getNormalizedSize() {
	    return _normalizedValueVector.size();
	}
	void setMax(float max) {
		_max = max;
	}
	float getMax() const {
		return _max;
	}
	void setMin(float min) {
		_min = min;
	}
	float getMin() const {
		return _min;
	}
    virtual int getDimension() const {
        return 1;
    }
  
	vector<float>& getValueVector() {
	    return _valueVector;
	}
	vector<float>& getNormalizedValueVector() {
		return _normalizedValueVector;
	}

	virtual void write(ofstream& os) {
		Column::write(os);
	  
		InOut::Write(os, _max);
		InOut::Write(os, _min);
		InOut::Write(os, _valueVector);
		InOut::Write(os, _normalizedValueVector);
	}
	virtual void read(ifstream& is) {
		Column::read(is);
	  
		InOut::Read(is, _max);
		InOut::Read(is, _min);
		InOut::Read(is, _valueVector);
		InOut::Read(is, _normalizedValueVector);
	}

private:
    float _max;
	float _min;
	vector<float> _valueVector;
	vector<float> _normalizedValueVector;
};

#endif
