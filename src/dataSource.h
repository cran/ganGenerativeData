// Copyright 2021 Werner Mueller
// Released under the GPL (>= 2)

#ifndef DATA_SOURCE
#define DATA_SOURCE
  
#include <map>
#include <random>
#include <fstream>
#include <algorithm>

#include "numberColumn.h"
#include "stringColumn.h"

using namespace std;

const string cValueTypeMismatch = "Type of value does not match";
const string cInvalidTypeId = "Invalid type id";
const string cDataSourceNotNormalized = "Datasource not normalized";
const string cInvalidColumnIndex = "Column index is invalid";
const string cInvalidType = "Invalid type";
const string cInvalidDimension = "Invalid dimension";
const string cInvalidTypePrefix = "Type of occurred value";
const string cInvalidTypeSuffix = "is invalid";


const string cDataSourceTypeId = "c46afa0e-51b6-4877-b4f4-53d909e34a7d";
const wstring cDensityColumn = L"Densities";

const string cNoDensities = "No density values calculated";

class DataSource{
public:
	DataSource():  _typeId(cDataSourceTypeId), _version(1), _normalized(false), _pG(new mt19937(_rd())), _pR(0), _pDensityVector(new NumberColumn(Column::NUMERICAL, cDensityColumn)) {
	}
    DataSource(const DataSource& dataSource): _typeId(cDataSourceTypeId), _version(1), _normalized(false), _pG(new mt19937(_rd())), _pR(0), _pDensityVector(new NumberColumn(Column::NUMERICAL, cDensityColumn)) {
	    _version = dataSource._version;
	    _normalized = false;
	  
	    for(int i = 0; i < dataSource.getColumnVector().size(); i++) {
		    Column::COLUMN_TYPE type = ((dataSource.getColumnVector())[i])->getColumnType();
			if(type == Column::STRING) {
			    const StringColumn* pStringColumn = dynamic_cast<const StringColumn*>(dataSource.getColumnVector()[i]);
				_columnVector.push_back(new StringColumn(*pStringColumn));
			} else if(type == Column::NUMERICAL) {
			    const NumberColumn* pNumberColumn = dynamic_cast<const NumberColumn*>(dataSource.getColumnVector()[i]);
				_columnVector.push_back(new NumberColumn(*pNumberColumn));
			} else {
				throw string(cInvalidColumnType);
			}
		}
	}
    DataSource(const vector<Column::COLUMN_TYPE>& columnTypes, const std::vector<wstring>& columnNames): _typeId(cDataSourceTypeId), _version(1), _normalized(false), _pG(new mt19937(_rd())), _pR(0), _pDensityVector(new NumberColumn(Column::NUMERICAL, cDensityColumn)) {
        _normalized = false;
        
        for(int i = 0; i < columnTypes.size(); i++) {
            Column::COLUMN_TYPE columnType = columnTypes[i];
            wstring columnName = columnNames[i];
            if(columnType == Column::STRING) {
                _columnVector.push_back(new StringColumn(columnType, columnName));
            } else if(columnType == Column::NUMERICAL) {
                _columnVector.push_back(new NumberColumn(columnType, columnName));
            } else {
                throw string(cInvalidColumnType);
            }
        }
    }
	~DataSource() {
		for(int i = 0; i < _columnVector.size(); i++) {
			delete _columnVector[i];
		}
		delete _pR;
		delete _pG;
	}

    virtual void clear() {
        for(int i = 0; i < _columnVector.size(); i++) {
            _columnVector[i]->clear();
        }
    }
    virtual int getDimension() {
        int dimension = 0;
        for(auto column : _columnVector) {
            if(column->getActive()) {
                dimension += column->getDimension();
            }
        }
        return dimension;
    }
    virtual int getDimension(int i) {
        if(i < 0 || i > _columnVector.size() - 1) {
            throw string(cInvalidIndex);  
        }
        return _columnVector[i]->getDimension();
    }
  
    DataSource & addData(const DataSource& dataSource) {
		for(int i = 0; i < _columnVector.size(); i++) {
			Column::COLUMN_TYPE type = _columnVector[i]->getColumnType();
			if(type == Column::STRING) {
				StringColumn* pStringColumnA = dynamic_cast<StringColumn*>(_columnVector[i]);
				StringColumn* pStringColumnB = dynamic_cast<StringColumn*>(dataSource.getColumnVector()[i]);

				for(int j = 0; j < pStringColumnB->getValueVector().size(); j++) {
					wstring value = pStringColumnB->getValue(j);
					pStringColumnA->addValue(value, false);
				}
			} else if(type == Column::NUMERICAL) {
				NumberColumn* pNumberColumnA = dynamic_cast<NumberColumn*>(_columnVector[i]);
				NumberColumn* pNumberColumnB = dynamic_cast<NumberColumn*>(dataSource.getColumnVector()[i]);

				for(int j = 0; j < pNumberColumnB->getValueVector().size(); j++) {
					pNumberColumnA->addValue((pNumberColumnB->getValueVector())[j]);
				}
			} else {
				throw string(cInvalidColumnType);
			}
		}
		return *this;
	}
  
    DataSource & addData(const DataSource& dataSource, const vector<int>& indexVector) {
        for(int i = 0; i < _columnVector.size(); i++) {
            Column::COLUMN_TYPE type = _columnVector[i]->getColumnType();
            if(type == Column::STRING) {
                StringColumn* pStringColumnA = dynamic_cast<StringColumn*>(_columnVector[i]);
                StringColumn* pStringColumnB = dynamic_cast<StringColumn*>(dataSource.getColumnVector()[i]);
        
                for(int j = 0; j < indexVector.size(); j++) {
        	        wstring value = pStringColumnB->getValue(indexVector[j]);
        	        pStringColumnA->addValue(value, false);
                }
            } else if(type == Column::NUMERICAL) {
                NumberColumn* pNumberColumnA = dynamic_cast<NumberColumn*>(_columnVector[i]);
                NumberColumn* pNumberColumnB = dynamic_cast<NumberColumn*>(dataSource.getColumnVector()[i]);
        
                for(int j = 0; j < pNumberColumnB->getValueVector().size(); j++) {
                    pNumberColumnA->addValue((pNumberColumnB->getValueVector())[indexVector[j]]);
                }
        
            } else {
                throw string(cInvalidColumnType);
            }
        }
        return *this;
    }
    
    void addValueRow(vector<std::wstring> valueVector) {
        for(int i = 0; i < (int)valueVector.size(); i++) {
            Column::COLUMN_TYPE type = _columnVector[i]->getColumnType();
            if(type == Column::STRING) {
                StringColumn* pStringColumn = dynamic_cast<StringColumn*>(_columnVector[i]);
                pStringColumn->addValue(valueVector[i]);
            } else if(type == Column::NUMERICAL) {
                NumberColumn* pNumberColumn = dynamic_cast<NumberColumn*>(_columnVector[i]);
                pNumberColumn->addValue(getFloatValue(valueVector[i]));
            } else {
                throw string(cInvalidColumnType);
            }
        }
    }
    float getFloatValue(const wstring& stringValue) {
        wstring sv = stringValue;
        float value;
        wstringstream wss(sv);
        wss >> value;
        if(wss.fail()) {
            string invalidType = cInvalidTypePrefix + " " + string(stringValue.begin(), stringValue.end()) + " " + cInvalidTypeSuffix;
            throw string(invalidType);
        }
        return value;
    }

	void addColumn(const Column::COLUMN_TYPE& type, const wstring& name) {
		if (type == Column::STRING) {
			_columnVector.push_back(new StringColumn(type, name));
		} else if(type == Column::NUMERICAL) {
			_columnVector.push_back(new NumberColumn(type, name));
		} else {
			throw string(cInvalidColumnType);
		}
	}
	vector<float> getNumberVector(int i) {
		vector<float> numberVector;
		for(auto column : _columnVector) {
		    if(column->getActive()) {
		        vector<float> columnNumberVector = column->getNumberVector(i);
			    numberVector.insert(numberVector.end(), columnNumberVector.begin(), columnNumberVector.end());
		    }
		}
		return numberVector;
	}
	vector<float> getNormalizedNumberVector(int i) {
		vector<float> numberVector;
		for(auto column : _columnVector) {
		    if(column->getActive()) {
		        vector<float> columnNumberVector = column->getNormalizedNumberVector(i);
		        numberVector.insert(numberVector.end(), columnNumberVector.begin(), columnNumberVector.end());
		    }
        }
	    return numberVector;
	}
    vector<float> getDenormalizedNumberVector(int i) {
        vector<float> numberVector;
        for(auto column : _columnVector) {
            if(column->getActive()) {
                vector<float> columnNumberVector = column->getDenormalizedNumberVector(i);
                numberVector.insert(numberVector.end(), columnNumberVector.begin(), columnNumberVector.end());
            }
        }
        return numberVector;
    }
    vector<wstring> getActiveColumnNames() {
        vector<wstring> activeColumnNames;
        for(auto column : _columnVector) {
            if(column->getActive()) {
                activeColumnNames.push_back(column->getName());
            }
        }
        return activeColumnNames;
    }
    vector<wstring> getInactiveColumnNames() {
        vector<wstring> inactiveColumnNames;
        for(auto column : _columnVector) {
            if(!column->getActive()) {
                inactiveColumnNames.push_back(column->getName());
            }
        }
        return inactiveColumnNames;
    }
    int getColumnIndex(int numberVectorIndex) {
        int dimension = getDimension();
        if(numberVectorIndex < 0 || numberVectorIndex > dimension - 1) {
            throw string(cInvalidIndex);
        }
        int columnIndex = 0;
        int i = 0;
        int j = 0;
        for(i = 0; i < _columnVector.size(); i++) {
            if(_columnVector[i]->getActive()) {
                if(numberVectorIndex < j + _columnVector[i]->getDimension())
                {
                    break;
                }
                j = j + _columnVector[i]->getDimension();
            }
        }
    
        if(i < 0 || i > (int)_columnVector.size() - 1)
        {
            throw string(cInvalidColumnIndex);
        }
        return i;
    }
    wstring getNumberVectorIndexName(int numberVectorIndex) {
        int dimension = getDimension();
        if(numberVectorIndex < 0 || numberVectorIndex > dimension - 1) {
            throw string(cInvalidIndex);
        }
        int columnIndex = 0;
        int i = 0;
        int j = 0;
        for(i = 0; i < _columnVector.size(); i++) {
            if(_columnVector[i]->getActive()) {
                if(numberVectorIndex < j + _columnVector[i]->getDimension())
                {   
                    break;
                }
                j = j + _columnVector[i]->getDimension();
            }
        }
    
        if(i < 0 || i > (int)_columnVector.size() - 1)
        {
            throw string(cInvalidColumnIndex);
        }
    
        wstring numberVectorIndexName = _columnVector[i]->getName();
        Column::COLUMN_TYPE type = _columnVector[i]->getColumnType();
        if(type == Column::STRING) {
            StringColumn* pStringColumn = dynamic_cast<StringColumn*>(_columnVector[i]);
            int k = numberVectorIndex - j + 1;
            map<int, wstring>::const_iterator inverseValueMapIter;
            inverseValueMapIter = pStringColumn->getInverseValueMap().find(k);
            if(inverseValueMapIter ==  pStringColumn->getInverseValueMap().end()) {
                throw string(cInvalidIndex);
            }
            numberVectorIndexName += L".";
            numberVectorIndexName += inverseValueMapIter->second;
        } else if(type == Column::NUMERICAL) {
            ;
        } else {
            throw string(cInvalidColumnType);
        }

        return numberVectorIndexName;
    }
    vector<wstring> getNumbeVectorIndexNames(vector<int> numberVectorIndices) {
	    vector<wstring> numberVextorIndexNames;
	    for(int i = 0; i < numberVectorIndices.size(); i++) {
		    numberVextorIndexNames.push_back(getNumberVectorIndexName(numberVectorIndices[i]));
	    }

	    return numberVextorIndexNames;
    }
    vector<wstring> getColumnNames(vector<int> indexVector) {
        vector<wstring> columnNames;
        for(int i = 0; i < indexVector.size(); i++) {
            columnNames.push_back(_columnVector[indexVector[i]]->getName());
        }
        return columnNames;
    }
  
	vector<Column*> const & getColumnVector() const {
		return _columnVector;
	}
	vector<Column*>& getColumnVector() {
		return _columnVector;
	}
  
    int getSize() {
        int size = 0;
        for(int i = 0; i < _columnVector.size(); i++) {
            if(_columnVector[i]->getActive()) {
                size = _columnVector[i]->getSize();
                break;
            }
        }
      
        return size;
    }
    
    int getNormalizedSize() {
        int normalizedSize = 0;
        if(_normalized) {
            for(int i = 0; i < _columnVector.size(); i++) {
                if(_columnVector[i]->getActive()) {
                    normalizedSize = _columnVector[i]->getNormalizedSize();
                    break;
                }
            }
        }
        
        return normalizedSize;
    }
    void setNormalized(bool normalized) {
        _normalized = normalized;
    }
    bool getNormalized() const {
        return _normalized;
    }
  
  vector<float> getDataRandom(int rowCount) {
      vector<float> numberVector;
      
      if(_pR == 0) {
          _pR = new uniform_int_distribution<int>(0, getSize() - 1);
      }
      _indexVector.resize(rowCount, 0);
      for(int i = 0; i < (int)_indexVector.size(); i++) {
          _indexVector[i] = (*_pR)(*_pG);
      }
      
      for(int i = 0; i < rowCount; i++) {
          vector<float> rowNumberVector = getNumberVector((_indexVector)[i]);
          numberVector.insert(numberVector.end(), rowNumberVector.begin(), rowNumberVector.end());
      }
      return numberVector;
  }
    
    vector<float> getNormalizedDataRandom(int rowCount) {
        vector<float> numberVector;
        if(!_normalized) {
        throw string(cDataSourceNotNormalized);
        }
    
        if(_pR == 0) {
            _pR = new uniform_int_distribution<int>(0, getNormalizedSize() - 1);
        }
        _indexVector.resize(rowCount, 0);
        for(int i = 0; i < (int)_indexVector.size(); i++) {
        _indexVector[i] = (*_pR)(*_pG);
        }
    
        for(int i = 0; i < rowCount; i++) {
            vector<float> rowNumberVector = getNormalizedNumberVector((_indexVector)[i]);
            numberVector.insert(numberVector.end(), rowNumberVector.begin(), rowNumberVector.end());
        }
        return numberVector;
    }
  
    vector<float> getDenormalizedDataRandom(int rowCount) {
        vector<float> numberVector;
        if(!_normalized) {
            throw string(cDataSourceNotNormalized);
        }
      
        if(_pR == 0) {
             _pR = new uniform_int_distribution<int>(0, getNormalizedSize() - 1);
        }
        _indexVector.resize(rowCount, 0);
        for(int i = 0; i < (int)_indexVector.size(); i++) {
            _indexVector[i] = (*_pR)(*_pG);
        }
      
        for(int i = 0; i < rowCount; i++) {
            vector<float> rowNumberVector = getDenormalizedNumberVector((_indexVector)[i]);
            numberVector.insert(numberVector.end(), rowNumberVector.begin(), rowNumberVector.end());
        }
        return numberVector;
    }
    vector<vector<float>> getDenormalizedDataRandomWithDensities(int rowCount) {
        vector<float> numberVector;
        if(!_normalized) {
            throw string(cDataSourceNotNormalized);
        }
        
        if( _pDensityVector->getNormalizedValueVector().size() == 0) {
            throw string(cNoDensities);
        }
        
        if(_pR == 0) {
            _pR = new uniform_int_distribution<int>(0, getNormalizedSize() - 1);
        }
        _indexVector.resize(rowCount, 0);
        for(int i = 0; i < (int)_indexVector.size(); i++) {
            _indexVector[i] = (*_pR)(*_pG);
        }
        
        vector<float> densityVector(rowCount, 0);
        for(int i = 0; i < rowCount; i++) {
            densityVector[i] = _pDensityVector->getNormalizedValueVector()[_indexVector[i]];
            vector<float> rowNumberVector = getDenormalizedNumberVector(_indexVector[i]);
            numberVector.insert(numberVector.end(), rowNumberVector.begin(), rowNumberVector.end());
        }
        
        vector<vector<float>> numberVectorVector;
        numberVectorVector.push_back(numberVector);
        numberVectorVector.push_back(densityVector);
        return numberVectorVector;
    }
    
    vector<float> getNormalizedData(int row, int rowCount) {
        if(row < 0 || row > getNormalizedSize() - 1) {
            throw string(cInvalidIndex);
        }
        vector<float> numberVector;
        if(!_normalized) {
            throw string(cDataSourceNotNormalized);
        }
    
        for(int i = 0; i < rowCount; i++) {
            if(row + i < getNormalizedSize()) {
                vector<float> rowNumberVector = getNormalizedNumberVector(row + i);
                numberVector.insert(numberVector.end(), rowNumberVector.begin(), rowNumberVector.end());
            }
        }
        numberVector.resize(getDimension() * rowCount, 0);
    
        return numberVector;
    }
  
    vector<float> getRow(int i) {
        vector<float> rowNumberVector = getNumberVector(i);
        return rowNumberVector;
    }
  
    vector<float> getNormalizedRow(int i) {
        vector<float> rowNumberVector = getNormalizedNumberVector(i);
        return rowNumberVector;
    }
  
    vector<float> getDenormalizedRow(int i) {
        vector<float> rowNumberVector = getDenormalizedNumberVector(i);
        return rowNumberVector;
    }
    
    void setColumnActive(int i, bool active) {
	    (_columnVector[i])->setActive(active);
	}
	void setColumnsActive(vector<int> indexVector, bool active) {
		for(int i = 0; i < (int)indexVector.size(); i++) {
		    if(indexVector[i] < 0 ||indexVector[i] > _columnVector.size() - 1) {
		        throw string(cInvalidIndex);  
		    }
			(_columnVector[indexVector[i]])->setActive(active);
		}
	}
  
	void write(ofstream& os, int version = 2) {
	    InOut::Write(os, _typeId);
	  
		InOut::Write(os, version);
	    InOut::Write(os, _normalized);
		InOut::Write(os, _indexVector);

		int size = _columnVector.size();
		InOut::Write(os, size);
		for(int i = 0; i < _columnVector.size(); i++) {
			int t = static_cast<int>(_columnVector[i]->getColumnType());
			InOut::Write(os, t);
			_columnVector[i]->write(os);
		}
		
		int t = static_cast<int>(_pDensityVector->getColumnType());
		InOut::Write(os, t);
		_pDensityVector->write(os);
	}
 
	void read(ifstream& is) {
	    InOut::Read(is, _typeId);
	    if(_typeId != cDataSourceTypeId) {
	        throw string(cInvalidTypeId);
	    }
	  
	    readWithoutTypeId(is);
	}
    void readWithoutTypeId(ifstream& is) {
        InOut::Read(is, _version);
        InOut::Read(is, _normalized);
        InOut::Read(is, _indexVector);
    
        int size = 0;
        InOut::Read(is, size);
        _columnVector.resize(size);
        int t = -1;
        for(int i = 0; i < size; i++) {
            InOut::Read(is, t);
            Column::COLUMN_TYPE type = static_cast<Column::COLUMN_TYPE>(t);
            if(type == Column::STRING) {
                _columnVector[i] = new StringColumn(type);
                _columnVector[i]->read(is);
            } else if(type == Column::NUMERICAL) {
                _columnVector[i] = new NumberColumn(type);
                _columnVector[i]->read(is);
            } else {
                throw string(cInvalidColumnType);
            }
        }
        
        if(_version > 1) {
            InOut::Read(is, t);
            Column::COLUMN_TYPE type = static_cast<Column::COLUMN_TYPE>(t);
            if(type == Column::NUMERICAL) {
                delete _pDensityVector;
                _pDensityVector = new NumberColumn(Column::NUMERICAL, cDensityColumn);
                _pDensityVector->read(is);
            } else {
                throw string(cInvalidColumnType);
            }
        }
    }
    
    NumberColumn* getDensityVector() {
        return _pDensityVector;
    }

protected:
	random_device _rd;
  
    string _typeId;
	int _version;
	bool _normalized;
	
	mt19937* _pG;
	uniform_int_distribution<int>* _pR;
	
	vector<int> _indexVector;
	vector<Column*> _columnVector;
	
	NumberColumn* _pDensityVector;
};

#endif
