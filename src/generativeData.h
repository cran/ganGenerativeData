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
		_normalized = dataSource.getNormalized();

		for(int i = 0; i < (int)dataSource.getColumnVector().size(); i++) {
			if(!(dataSource.getColumnVector())[i]->getActive()) {
				continue;
			}
			Column::COLUMN_TYPE columnType = ((dataSource.getColumnVector())[i])->getColumnType();
			Column::SCALE_TYPE scaleType = ((dataSource.getColumnVector())[i])->getScaleType();
			if(columnType == Column::STRING) {
				throw string(cInvalidColumnType);
			} else if(columnType == Column::NUMERICAL) {
				const NumberColumn* pNumberColumn = dynamic_cast<const NumberColumn*>(dataSource.getColumnVector()[i]);
				_columnVector.push_back(new NumberColumn(*pNumberColumn));
			} else {
				throw string(cInvalidColumnType);
			}
		}
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

	vector<float> getNormalizedDataRandom(int rowCount, int indexVector) {
		vector<float> numberVector;
		if(!_normalized) {
			throw string(cDataSourceNotNormalized);
		}

		if(_pRVector[indexVector] == 0) {
			_pRVector[indexVector] = new uniform_int_distribution<int>(0, (int)_indexVectors[indexVector].size() - 1);
		}
		_indexVector.resize(rowCount, 0);
		for(int i = 0; i < (int)_indexVector.size(); i++) {
			_indexVector[i] = (*_pRVector[indexVector])(*_pG);
		}

		for(int i = 0; i < rowCount; i++) {
			vector<float> rowNumberVector = getNormalizedNumberVector(_indexVectors[indexVector][(_indexVector)[i]]);
			numberVector.insert(numberVector.end(), rowNumberVector.begin(), rowNumberVector.end());
		}
		return numberVector;
	}

    /*
	void write(ofstream& os, int version, int rowCount) {
		int size = _columnVector.size();
		for(int i = 0; i < _columnVector.size(); i++) {
			NumberColumn* pNumberColumn = dynamic_cast<NumberColumn*>(_columnVector[i]);
			if(pNumberColumn->getValueVector().size() > 0) {
				pNumberColumn->getValueVector().resize(rowCount);
			}
			if(pNumberColumn->getNormalizedValueVector().size() > 0) {
				pNumberColumn->getNormalizedValueVector().resize(rowCount);
			}
		}
		DataSource::write(os, version);
	}
    */
	void read(ifstream& is) {
		InOut::Read(is, _typeId);
		if(_typeId != cGenerativeDataSourceTypeId) {
			throw string(cInvalidTypeId);
		}

		readWithoutTypeId(is);
	}

private:
	void getInverseValueMap(const map<wstring, int>& valueMap, map<int, wstring>& inverseValueMap) {
		map<wstring, int>::const_iterator mapIter = valueMap.begin();
		while(mapIter != valueMap.end()) {
			inverseValueMap[mapIter->second] = mapIter->first;
			mapIter++;
		}
	}

	vector<vector<int>> _indexVectors;
	vector<uniform_int_distribution<int>*> _pRVector;
};

#endif
