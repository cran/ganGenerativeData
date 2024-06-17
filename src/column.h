// Copyright 2021 Werner Mueller
// Released under the GPL (>= 2)

#ifndef COLUMN
#define COLUMN

#include "inOut.h"

using namespace std;

const string cInvalidColumnType = "Invalid column type";
const string cInvalidActiveColumn = "Invalid active column";
const string cInvalidScaleType = "Invalid scale type";
const string cInvalidIndex = "Invalid index";
const string cSuffixInvalidColumnType = "invalid column type";

class Column{
public:
	enum COLUMN_TYPE {
		STRING,
		NUMERICAL,
		NUMERICAL_ARRAY
	};
    enum SCALE_TYPE {
        LINEAR,
        LOGARITHMIC,
        BINARY,
        NOMINAL
    };
  
	Column(const COLUMN_TYPE type, const SCALE_TYPE scaleType): _type(type), _scaleType(scaleType), _name(L""), _active(true) {
	}
	Column(const COLUMN_TYPE type, const SCALE_TYPE scaleType, const wstring& name, bool active): _type(type),  _scaleType(scaleType), _name(name), _active(active) {
	}
	virtual ~Column() {
	}
	
	virtual vector<float> getNumberVector(int i) = 0;
	virtual vector<float> getNormalizedNumberVector(int i) = 0;
	virtual vector<float> getDenormalizedNumberVector(int i) = 0;
	virtual void clear() = 0;
	virtual int getDimension() const = 0;
	virtual int getSize() = 0;
	virtual int getNormalizedSize() = 0;
	
	const COLUMN_TYPE getColumnType() const {
		return _type;
	}
	const SCALE_TYPE getScaleType() const {
	    return _scaleType;
	}
	void setName(const wstring& name) {
	    _name = name;
	}
	const wstring getName() const {
		return _name;
	}
	void setActive(bool active) {
		_active = active;
	}
	bool getActive() const {
		return _active;
	}

	virtual void write(ofstream& os) {
		InOut::Write(os, _name);
		InOut::Write(os, _active);
		
		int st = static_cast<int>(_scaleType);
		InOut::Write(os, st);
	}
	virtual void read(ifstream& is) {
		InOut::Read(is, _name);
		InOut::Read(is, _active);
		
		int st = 0;
		InOut::Read(is, st);
		_scaleType = static_cast<SCALE_TYPE>(st);
	}
	
protected:
	COLUMN_TYPE _type;
    SCALE_TYPE _scaleType;
	wstring _name;
	bool _active;
};

#endif
