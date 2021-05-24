// Copyright 2021 Werner Mueller
// Released under the GPL (>= 2)

#include <Rcpp.h>
using namespace Rcpp;
using namespace std;

#include "generativeData.h"

namespace gdInt {
    DataSource* pDataSource = 0;
    GenerativeData* pGenerativeData = 0;
  
    string inGenerativeDataFileName = "";
    string inDataSourceFileName = "";
    int batchSize = 256;
    int maxSize = batchSize * 25000;
  
    const string cMaxSizeExceeded = "Max size of generative data exceeded";
}

// [[Rcpp::export]]
void gdReset() {
    try {
        delete gdInt::pDataSource;
        gdInt::pDataSource = 0;
        delete gdInt::pGenerativeData;
        gdInt::pGenerativeData = 0;
        
        gdInt::inGenerativeDataFileName = "";
        gdInt::inDataSourceFileName = "";
    } catch (const string& e) {
        ::Rf_error(e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
std::string gdGetDataSourceFileName() {
    try {
        return gdInt::inDataSourceFileName;
    } catch (const string& e) {
        ::Rf_error(e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
std::string gdGetGenerativeDataFileName() {
    try {
        return gdInt::inGenerativeDataFileName;
    } catch (const string& e) {
        ::Rf_error(e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
int gdGetBatchSize() {
    try {
        return gdInt::batchSize;
    } catch (const string& e) {
        ::Rf_error(e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
int gdGetMaxSize() {
    try {
        return gdInt::maxSize;
    } catch (const string& e) {
        ::Rf_error(e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
void gdDataSourceRead(const std::string& inFileName) {
    try {
        ifstream is;
        is.open(inFileName.c_str(), std::ios::binary);
        if(!is.is_open()) {
            throw string("File " + inFileName + " could not be opened");
        }
        gdInt::inDataSourceFileName = inFileName;
    
        delete gdInt::pDataSource;
        gdInt::pDataSource = new DataSource();
        gdInt::pDataSource->read(is);
        is.close();
    
    } catch (const string& e) {
        ::Rf_error(e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
void gdGenerativeDataRead(const std::string& inFileName) {
    try {
        ifstream is;
        is.open(inFileName.c_str(), std::ios::binary);
        if(!is.is_open()) {
            throw string("File " + inFileName + " could not be opened");
        }
        gdInt::inGenerativeDataFileName = inFileName;
    
        delete gdInt::pGenerativeData;
        gdInt::pGenerativeData = new GenerativeData();
        gdInt::pGenerativeData->read(is);
        is.close();
    
        if(gdInt::pGenerativeData->getNormalizedSize() > gdInt::maxSize) {
            throw string(gdInt::cMaxSizeExceeded);
        }
    
    } catch (const string& e) {
        ::Rf_error(e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
void gdGenerativeDataWrite(const std::string& outFileName) {
    try {
        if(gdInt::pGenerativeData == 0) {
            throw string("No generative data");
        }
    
        ofstream outFile;
        outFile.open(outFileName.c_str(), std::ios::binary);
        if(!outFile.is_open()) {
            throw string("File " + outFileName + " could not be opened");
        }
    
        gdInt::pGenerativeData->DataSource::write(outFile);
        outFile.close();
    } catch (const string& e) {
        ::Rf_error(e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
std::vector<float> gdDataSourceGetDataRandom(int rowCount) {
    try {
        if(gdInt::pDataSource == 0) {
            throw string("No datasource");
        }
    
        std::vector<float> v = gdInt::pDataSource->getDataRandom(rowCount);
        return v;
    } catch (const string& e) {
        ::Rf_error(e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
std::vector<float> gdDataSourceGetNormalizedDataRandom(int rowCount) {
    try {
        if(gdInt::pDataSource == 0) {
            throw string("No datasource");
        }
    
        std::vector<float> v = gdInt::pDataSource->getNormalizedDataRandom(rowCount);
        return v;
    } catch (const string& e) {
        ::Rf_error(e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
std::vector<float> gdGenerativeDataGetNormalizedDataRandom(int rowCount) {
    try {
        if(gdInt::pGenerativeData == 0) {
            throw string("No generative data");
        }
  
        std::vector<float> v = ((DataSource*)gdInt::pGenerativeData)->getNormalizedDataRandom(rowCount);
        return v;
    } catch (const string& e) {
        ::Rf_error(e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
std::vector<float> gdGenerativeDataGetDenormalizedDataRandom(int rowCount) {
  try {
    if(gdInt::pGenerativeData == 0) {
      throw string("No generative data");
    }
    
    std::vector<float> v = ((DataSource*)gdInt::pGenerativeData)->getDenormalizedDataRandom(rowCount);
    return v;
  } catch (const string& e) {
    ::Rf_error(e.c_str());
  } catch(...) {
    ::Rf_error("C++ exception (unknown reason)");
  }
}

// [[Rcpp::export]]
int gdGetDataSourceDimension() {
    try {
        if(gdInt::pDataSource == 0) {
            throw string("No datasource");
        }
    
        return gdInt::pDataSource->getDimension();
    } catch (const string& e) {
        ::Rf_error(e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
void gdAddValueRows(const std::vector<float>& valueRows) {
    try {
        if(gdInt::pGenerativeData == 0) {
            gdInt::pGenerativeData = new GenerativeData(*gdInt::pDataSource);
        }
    
        gdInt::pGenerativeData->addValueLines(valueRows); 
    } catch (const string& e) {
        ::Rf_error(e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

//' Get number of rows
//'
//' Get number of rows in generative data
//'
//' @return Number of rows
//' @export
//'
//' @examples
//' \donttest{gdRead("gd.bin")
//' gdGetNumberOfRows()}
// [[Rcpp::export]]
int gdGetNumberOfRows() {
//int gdGetNormalizedSize() {
    try {
        if(gdInt::pGenerativeData == 0) {
            throw string("No gnerative data");
        }
    
        return gdInt::pGenerativeData->getNormalizedSize();
    } catch (const string& e) {
        ::Rf_error(e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
std::vector<std::wstring> gdGetColumnNames(std::vector<int>& indexVector) {
  try {
    if(gdInt::pGenerativeData == 0) {
      throw string("No generative data");
    }
    
    for(int i = 0; i < indexVector.size(); i++) {
      indexVector[i] -= 1;
    }
    vector<wstring> columnNames = gdInt::pGenerativeData->getColumnNames(indexVector);
    return columnNames;
  } catch (const string& e) {
    ::Rf_error(e.c_str());
  } catch(...) {
    ::Rf_error("C++ exception (unknown reason)");
  }
}

// [[Rcpp::export]]
int gdGetGenerativeDataDimension() {
    try {
        if(gdInt::pGenerativeData == 0) {
            throw string("No generative data");
        }
    
        return gdInt::pGenerativeData->getDimension();
    } catch (const string& e) {
        ::Rf_error(e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
std::vector<std::wstring> gdGetNumberVectorIndexNames(std::vector<int>& numberVectorIndices) {
    try {
        //if(gdInt::pGenerativeDataSource == 0) {
        if(gdInt::pDataSource == 0) {
            //throw string("No gnerative datasource");
            throw string("No datasource");
        }
    
        vector<int> indexVector = numberVectorIndices;
        for(int i = 0; i < indexVector.size(); i++) {
            indexVector[i] -= 1;
        }
        //vector<wstring> numberVectorIndexNames = gdInt::pGenerativeDataSource->getNumbeVectorIndexNames(indexVector);
        vector<wstring> numberVectorIndexNames = gdInt::pDataSource->getNumbeVectorIndexNames(indexVector);
        return numberVectorIndexNames;
    } catch (const string& e) {
        ::Rf_error(e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

//' Get a row in generative data
//'
//' Get a row in generative data containing values of active columns for a row index
//'
//' @param index Index of row
//'
//' @return Row in generative data
//' @export
//'
//' @examples
//' \donttest{gdRead("gd.bin")
//' gdGetRow(1)}
// [[Rcpp::export]]
List gdGetRow(int index) {
    try {
      if(gdInt::pGenerativeData == 0) {
        throw string("No generative data");
    }
    
    List list;
    vector<Column*> const & columnVector = gdInt::pGenerativeData->getColumnVector();
    for(int j = 0; j < columnVector.size(); j++) {
        Column::COLUMN_TYPE type = columnVector[j]->getColumnType();
        if(type == Column::NUMERICAL) {
          vector<float> numberVector = columnVector[j]->getDenormalizedNumberVector(index - 1);
          float value = numberVector[0];
          list.insert(list.end(), value);
        } else {
          throw string(cInvalidColumnType);
        }
    }
    return list;
    } catch (const string& e) {
      ::Rf_error(e.c_str());
    } catch(...) {
      ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
float gdGetMax(int i) {
  try {
    if(gdInt::pGenerativeData == 0) {
      throw string("No generative data");
    }
    
    vector<Column*> const & columnVector = gdInt::pGenerativeData->getColumnVector();
    if(i - 1 < 0 || i - 1 > (int)columnVector.size() - 1) {
        throw string(cInvalidColumnIndex);
    }
    
    float max = 0;
    Column::COLUMN_TYPE type = columnVector[i - 1]->getColumnType();
    if(type == Column::NUMERICAL) {
        NumberColumn* pNumberColumn = dynamic_cast<NumberColumn*>(columnVector[i - 1]);
        max = pNumberColumn->getMax();
    } else {
        throw string(cInvalidColumnType);
    }
    return max;
  } catch (const string& e) {
    ::Rf_error(e.c_str());
  } catch(...) {
    ::Rf_error("C++ exception (unknown reason)");
  }
}

// [[Rcpp::export]]
float gdGetMin(int i) {
  try {
      if(gdInt::pGenerativeData == 0) {
        throw string("No generative data");
      }
    
      vector<Column*> const & columnVector = gdInt::pGenerativeData->getColumnVector();
      if(i - 1 < 0 || i - 1 > (int)columnVector.size() - 1) {
          throw string(cInvalidColumnIndex);
      }
    
      float min = 0;
      Column::COLUMN_TYPE type = columnVector[i - 1]->getColumnType();
      if(type == Column::NUMERICAL) {
          NumberColumn* pNumberColumn = dynamic_cast<NumberColumn*>(columnVector[i - 1]);
          min = pNumberColumn->getMin();
      } else {
          throw string(cInvalidColumnType);
      }
      return min;
  } catch (const string& e) {
      ::Rf_error(e.c_str());
  } catch(...) {
      ::Rf_error("C++ exception (unknown reason)");
  }
}
