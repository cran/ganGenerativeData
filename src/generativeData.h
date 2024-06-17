// Copyright 2021 Werner Mueller
// Released under the GPL (>= 2)

#ifndef GENERATIVE_DATA
#define GENERATIVE_DATA

#include "dataSource.h"

using namespace std;

const wstring cColumnNameSeparator = L" ";

const string cInvalidVectorSize = "Invalid size of vector";
const string cGenerativeDataSourceTypeId = "15e02d71-de25-4e2f-8b79-d9e1d7c4a5ed";
  
class GenerativeData : public DataSource {
public:
	GenerativeData() {
		_typeId = cGenerativeDataSourceTypeId;
	}
    
	GenerativeData(const DataSource& dataSource) {
	   	_typeId = cGenerativeDataSourceTypeId;
		_normalized = true;

		for(int i = 0; i < (int)dataSource.getColumnVector().size(); i++) {
			if(!(dataSource.getColumnVector())[i]->getActive()) {
				continue;
			}
			Column::COLUMN_TYPE columnType = ((dataSource.getColumnVector())[i])->getColumnType();
			Column::SCALE_TYPE scaleType = ((dataSource.getColumnVector())[i])->getScaleType();
			if(columnType == Column::STRING) {
			    if(scaleType == Column::NOMINAL) {
			        const StringColumn* pStringColumn = dynamic_cast<const StringColumn*>(dataSource.getColumnVector()[i]);
			        
			        vector<wstring> columnNames;
			        const map<int, wstring>& inverseValueMap = pStringColumn->getInverseValueMap();
			        map<int, wstring>::const_iterator iter(inverseValueMap.begin());
			        while(iter != inverseValueMap.end()) {
			            columnNames.push_back(iter->second);
			            iter++;
			        }
			        
			        int dimension = pStringColumn->getDimension();
			        NumberArrayColumn* pNumberArrayColumn = new NumberArrayColumn(Column::NUMERICAL_ARRAY, pStringColumn->getName(), dimension);
			        pNumberArrayColumn->setColumnNames(columnNames);
			        
			        _columnVector.push_back(pNumberArrayColumn);
			    } else {
			        throw string(cInvalidScaleType);
			    }
			} else if(columnType == Column::NUMERICAL) {
				const NumberColumn* pNumberColumn = dynamic_cast<const NumberColumn*>(dataSource.getColumnVector()[i]);
				_columnVector.push_back(new NumberColumn(*pNumberColumn));
			} else {
				throw string(cInvalidColumnType);
			}
		}
		
		delete _pDensityVector;
		_pDensityVector = new NumberColumn(*dataSource.getDensityVector());
	}

	void addValueLine(const vector<float>& valueVector, int offset = 0) {
		int index = offset;
		for(int i = 0; i < (int)_columnVector.size(); i++) {
			int dimension = 0;
			Column::COLUMN_TYPE type = _columnVector[i]->getColumnType();
			if(type == Column::NUMERICAL) {
				NumberColumn* pNumberColumn = dynamic_cast<NumberColumn*>(_columnVector[i]);
				pNumberColumn->addNormalizedValue(valueVector[index]);
				dimension = pNumberColumn->getDimension();
			} else if(type == Column::NUMERICAL_ARRAY) {
			    NumberArrayColumn* pNumberArrayColumn = dynamic_cast<NumberArrayColumn*>(_columnVector[i]);
			    pNumberArrayColumn->addNormalizedValue(valueVector, index);
			    dimension = pNumberArrayColumn->getDimension();	
			} else {
				throw string(cInvalidColumnType);
			}

			index += dimension;
		}

		if(index - offset != getDimension()) {
			throw string(cInvalidIndex);
		}
	}

	void addValueLines(const vector<float>& valueVector) {
		int dimension = getDimension();
		if(valueVector.size() % dimension != 0) {
			throw string(cInvalidVectorSize);
		}
		for(int i = 0; i < (int)valueVector.size() / dimension; i++) {
			addValueLine(valueVector, i * dimension);
		}
	}
    
	void read(ifstream& is) {
		InOut::Read(is, _typeId);
		if(_typeId != cGenerativeDataSourceTypeId) {
			throw string(cInvalidTypeId);
		}

		readWithoutTypeId(is);
		
		buildNormalizedNumberVectorVector();
	}
    
private:
	vector<uniform_int_distribution<int>*> _pRVector;
};

#endif
