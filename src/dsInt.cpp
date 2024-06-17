// Copyright 2021 Werner Mueller
// Released under the GPL (>= 2)

#include <Rcpp.h>
using namespace Rcpp;
using namespace std;

#include "normalizeData.h"

namespace dsInt {
    DataSource* pDataSource = 0;
}

//' Write a data source to file
//'
//' Write a data source including settings of active columns to a file in binary format.
//' This file will be used as input in functions for generation of generative data.\cr
//'
//' @param fileName Name of data source file
//'
//' @return None
//' @export
//'
//' @examples
//' \dontrun{
//' dsCreateWithDataFrame(iris)
//' dsDeactivateColumns(c(5))
//' dsWrite("ds.bin")}
// [[Rcpp::export]]
void dsWrite(const std::string& fileName) {
    try {
        if(dsInt::pDataSource == 0) {
            throw string("No datasource");
        }
    
        ofstream outFile;
        outFile.open(fileName.c_str(), ios::binary);
        if(!outFile.is_open()) {
            throw string("File " + fileName + " could not be opened");
        }
        
        NormalizeData normalizeData;
        normalizeData.normalize(*dsInt::pDataSource);
  
        dsInt::pDataSource->write(outFile);
        outFile.close();
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

//' Read a data source from file
//'
//' Read a data source from a file in binary format
//'
//' @param fileName Name of data source file
//'
//' @return None
//' @export
//'
//' @examples
//' \dontrun{
//' dsCreateWithDataFrame(iris)
//' dsDeactivateColumns(c(5))
//' dsWrite("ds.bin")
//' dsRead("ds.bin")}
// [[Rcpp::export]]
void dsRead(const std::string& fileName) {
    try {
        ifstream is;
        is.open(fileName.c_str(), ios::binary);
        if(!is.is_open()) {
            throw string("File " + fileName + " could not be opened");
        }
    
        delete dsInt::pDataSource;
        dsInt::pDataSource = new DataSource();
        dsInt::pDataSource->read(is);
        is.close();
    
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
void dsCreate(const std::vector<std::wstring>& columnTypes, const std::vector<std::wstring>& columnNames) {
    try {
        delete dsInt::pDataSource;
    
        vector<Column::COLUMN_TYPE> enumColumnTypes;
        for(int i = 0; i < (int)columnTypes.size(); i++) {
            if(columnTypes[i] == L"double" || columnTypes[i] == L"float") {
                enumColumnTypes.push_back(Column::NUMERICAL);
            } else {
                enumColumnTypes.push_back(Column::STRING);
            }
        }
        dsInt::pDataSource = new DataSource(enumColumnTypes, columnNames);
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
void dsAddValueRow(const std::vector<std::wstring>& valueVector) {
    try {
        if(dsInt::pDataSource == 0) {
            throw string("No datasource");
        }
    
        dsInt::pDataSource->addValueRow(valueVector);
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

//' Deactivate columns
//'
//' Deactivate columns in a data source in order to exclude them from training
//' of generative models. Note that the training function in the package
//' supports only columns of type R-class numeric, R-type double. All columns of
//' other type have to be deactivated. The training function in the software
//' service for accelerated training of generative models supports columns of
//' any type.
//'
//' @param columnVector Vector of column indices
//'
//' @return None
//' @export
//'
//' @examples
//' \donttest{
//' dsCreateWithDataFrame(iris)
//' dsDeactivateColumns(c(5))
//' dsGetInactiveColumnNames()}
// [[Rcpp::export]]
void dsDeactivateColumns(const std::vector<int>& columnVector) {
    try {
        if(dsInt::pDataSource == 0) {
            throw string("No datasource");
        }
    
        std::vector<int> cV = columnVector;
        for(int i = 0; i < (int)cV.size(); i++) {
            cV[i] -= 1;
        }
        
        dsInt::pDataSource->setColumnsActive(cV, false);
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

//' Activate columns
//'
//' Activate columns in a data source in order to include them in training of generative models. By default columns are active.
//'
//' @param columnVector Vector of column indices
//'
//' @return None
//' @export
//'
//' @examples
//' \donttest{
//' dsCreateWithDataFrame(iris)
//' dsGetActiveColumnNames()
//' dsDeactivateColumns(c(5))
//' dsGetActiveColumnNames()
//' dsActivateColumns(c(5))
//' dsGetActiveColumnNames()}
// [[Rcpp::export]]
void dsActivateColumns(const std::vector<int>& columnVector) {
    try {
        if(dsInt::pDataSource == 0) {
            throw string("No datasource");
        }
    
        std::vector<int> cV = columnVector;
        for(int i = 0; i < (int)cV.size(); i++) {
          cV[i] -= 1;
        }
        
        dsInt::pDataSource->setColumnsActive(cV, true);
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

//' Get active column names
//'
//' Get active column names of a data source
//'
//' @return Vector of names of active columns
//' @export
//'
//' @examples
//' \donttest{
//' dsCreateWithDataFrame(iris)
//' dsDeactivateColumns(c(5))
//' dsGetActiveColumnNames()}
// [[Rcpp::export]]
std::vector<std::wstring> dsGetActiveColumnNames() {
    try {
        if(dsInt::pDataSource == 0) {
            throw string("No datasource");
        }
    
        return dsInt::pDataSource->getActiveColumnNames();
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

//' Get inactive column names
//'
//' Get inactive column names of a data source
//'
//' 
//'
//' @return Vector of names of inactive columns
//' @export
//'
//' @examples
//' \donttest{
//' dsCreateWithDataFrame(iris)
//' dsDeactivateColumns(c(5))
//' dsGetInactiveColumnNames()}
// [[Rcpp::export]]
std::vector<std::wstring> dsGetInactiveColumnNames() {
    try {
        if(dsInt::pDataSource == 0) {
            throw string("No datasource");
        }
    
        return dsInt::pDataSource->getInactiveColumnNames();
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

//' Get number of rows
//'
//' Get number of rows in a data source
//'
//' 
//'
//' @return Number of rows
//' @export
//'
//' @examples
//' \donttest{
//' dsCreateWithDataFrame(iris)
//' dsGetNumberOfRows()}
// [[Rcpp::export]]
int dsGetNumberOfRows() {
    try {
        if(dsInt::pDataSource == 0) {
            throw string("No datasource");
        }
    
        return dsInt::pDataSource->getSize();
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

//' Get a row in a data source
//'
//' Get a row in a data source for a row index.
//'
//' @param index Index of row
//'
//' @return List containing row in data source
//' @export
//'
//' @examples
//' \donttest{
//' dsCreateWithDataFrame(iris)
//' dsGetRow(1)}
// [[Rcpp::export]]
List dsGetRow(int index) {
    try {
        if(dsInt::pDataSource == 0) {
            throw string("No datasource");
        }
    
        List list;
        vector<Column*> const & columnVector = dsInt::pDataSource->getColumnVector();
        for(int j = 0; j < (int)columnVector.size(); j++) {
        Column::COLUMN_TYPE type = columnVector[j]->getColumnType();
            if(type == Column::STRING) {
                StringColumn* pStringColumn = dynamic_cast<StringColumn*>(columnVector[j]);
                wstring value = pStringColumn->getValue(index - 1);
                list.insert(list.end(), value);
            } else if(type == Column::NUMERICAL) {
                vector<float> numberVector = columnVector[j]->getNumberVector(index - 1);
                float value = numberVector[0];
                if(isnan(value)) {
                    //list.insert(list.end(), "NA");
                    list.insert(list.end(), NA_REAL);
                    continue;
                }
                
                list.insert(list.end(), value);
            } else {
                throw string(cInvalidColumnType);
            }
        }
    
        return list;
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
bool dsGetNormalized() {
    try {
        if(dsInt::pDataSource == 0) {
            throw string("No datasource");
        }
    
        return dsInt::pDataSource->getNormalized();
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}
