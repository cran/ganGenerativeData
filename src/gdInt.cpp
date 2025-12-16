// Copyright 2021 Werner Mueller
// Released under the GPL (>= 2)

#include <Rcpp.h>

using namespace Rcpp;
using namespace std;

#include "density.h"
#include "generativeModel.h"

const string cInvalidNearestNeighborsSize = "Invalid size of nearest neighbors";
const string cDifferentListSizes = "Sizes of lists are different";

namespace gdInt {
    GenerativeModel* pGenerativeModel = 0;
    DataSource* pDataSource = 0;
    GenerativeData* pGenerativeData = 0;

    VpTree* pVpTree = 0;
    VpTreeData* pVpTreeData = 0;
    LpDistance* pLpDistance = 0;

    VpTree* pDensityVpTree = 0;
    VpTreeData* pDensityVpTreeData = 0;
    LpDistance* pDensityLpDistance = 0;

    string inGenerativeDataFileName = "";
    string inDataSourceFileName = "";
    int batchSize = 256;
    int maxSize = batchSize * 100000;
    int nNearestNeighbors = 20;

    const string cMaxSizeExceeded = "Max size of generative data exceeded";
}

std::wstring ToWstring(std::string Str) {
    std::vector<wchar_t> buf(Str.size());
    std::use_facet<std::ctype<wchar_t>>(std::locale()).widen(Str.data(),
                                        Str.data() + Str.size(),
                                        buf.data());
    return std::wstring(buf.data(), buf.size());
}

// [[Rcpp::export]]
void gdReset() {
    try {
        delete gdInt::pGenerativeModel;
        gdInt::pGenerativeModel = 0;
        delete gdInt::pDataSource;
        gdInt::pDataSource = 0;
        delete gdInt::pGenerativeData;
        gdInt::pGenerativeData = 0;

        delete gdInt::pVpTree;
        gdInt::pVpTree = 0;
        delete gdInt::pVpTreeData;
        gdInt::pVpTreeData = 0;
        delete gdInt::pLpDistance;
        gdInt::pLpDistance = 0;

        delete gdInt::pDensityVpTree;
        gdInt::pDensityVpTree = 0;
        delete gdInt::pDensityVpTreeData;
        gdInt::pDensityVpTreeData = 0;
        delete gdInt::pDensityLpDistance;
        gdInt::pDensityLpDistance = 0;

        gdInt::inGenerativeDataFileName = "";
        gdInt::inDataSourceFileName = "";
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
std::string gdGetDataSourceFileName() {
    try {
        return gdInt::inDataSourceFileName;
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
std::string gdGetGenerativeDataFileName() {
    try {
        return gdInt::inGenerativeDataFileName;
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
int gdGetBatchSize() {
    try {
        return gdInt::batchSize;
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
int gdGetMaxSize() {
    try {
        return gdInt::maxSize;
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
std::string gdGetFileName(const std::string& fileName) {
    try {
        return GetFileName()(fileName);
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
void gdCreateGenerativeModel() {
    try {
        delete gdInt::pGenerativeModel;
        gdInt::pGenerativeModel = new GenerativeModel(*gdInt::pDataSource);
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
void gdWriteWithReadingTrainedModel(const std::string& outFileName) {
    try {
        ofstream os;
        os.open(outFileName.c_str(), std::ios::binary);
        if(!os.is_open()) {
            throw string("File " + outFileName + " could not be opened");
        }

        //delete gdInt::pGenerativeModel;
        //gdInt::pGenerativeModel = new GenerativeModel(*gdInt::pDataSource);

        gdInt::pGenerativeModel->writeWithReadingTrainedModel(os, GetFileName()(outFileName));
        os.close();
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
bool gdReadGenerativeModel(const std::string& inFileName) {
    try {
        ifstream is;
        is.open(inFileName.c_str(), std::ios::binary);
        if(!is.is_open()) {
            //throw string("File " + inFileName + " could not be opened");
            return false;
        }

        delete gdInt::pGenerativeModel;
        gdInt::pGenerativeModel = new GenerativeModel();

        gdInt::pGenerativeModel->read(is, GetFileName()(inFileName));
        is.close();

        return true;
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
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
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
bool gdGenerativeDataRead(const std::string& inFileName) {
    try {
        ifstream is;
        is.open(inFileName.c_str(), std::ios::binary);
        if(!is.is_open()) {
            //throw string("File " + inFileName + " could not be opened");
            return false;
        }
        gdInt::inGenerativeDataFileName = inFileName;

        delete gdInt::pGenerativeData;
        gdInt::pGenerativeData = new GenerativeData();
        gdInt::pGenerativeData->read(is);
        is.close();

        if(gdInt::pGenerativeData->getNormalizedSize() > gdInt::maxSize) {
            throw string(gdInt::cMaxSizeExceeded);
        }

        return true;
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
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
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

//' Write subset of generative data
//'
//' Write subset of randomly selected rows of generative data
//'
//' @param fileName Name of subset generative data file
//' @param percent Percent of randomly selected rows
//'
//' @return None
//' @export
//'
//' @examples
//' \dontrun{
//' gdRead("gd.bin")
//' gdWriteSubset("gds.bin", 50)}
// [[Rcpp::export]]
void gdWriteSubset(const std::string& fileName, float percent) {
    try {
        if(gdInt::pGenerativeData == 0) {
            throw string("No generative data");
        }

        ofstream outFile;
        outFile.open(fileName.c_str(), std::ios::binary);
        if(!outFile.is_open()) {
            throw string("File " + fileName + " could not be opened");
        }

        vector<int> randomIndices = RandomIndicesWithoutReplacement()(gdInt::pGenerativeData->getNormalizedSize(), percent);
        sort(randomIndices.begin(), randomIndices.end());

        GenerativeData generativeData(dynamic_cast<DataSource&>(*gdInt::pGenerativeData));
        for(int i = 0; i < (int)randomIndices.size(); i++) {
            vector<float> normalizedNumberVector = gdInt::pGenerativeData->getNormalizedNumberVector(randomIndices[i]);
            generativeData.addValueLine(normalizedNumberVector);
        }

        if(gdInt::pGenerativeData->getDensityVector()->getNormalizedSize() > 0) {
            generativeData.getDensityVector()->getNormalizedValueVector().resize(randomIndices.size(), 0);
            for(int i = 0; i < (int)randomIndices.size(); i++) {
                generativeData.getDensityVector()->getNormalizedValueVector()[i] = gdInt::pGenerativeData->getDensityVector()->getNormalizedValueVector()[randomIndices[i]];
            }
        }

        generativeData.DataSource::write(outFile);
        outFile.close();
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
void gdCreateGenerativeData() {
    try {
        if(gdInt::pDataSource == 0) {
            throw string("No datasource");
        }

        delete gdInt::pGenerativeData;
        gdInt::pGenerativeData = new GenerativeData(*gdInt::pDataSource);
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
void gdCreateDataSourceFromGenerativeModel() {
    try {
        if(gdInt::pGenerativeModel == 0) {
            throw string("generative model");
        }

        delete gdInt::pDataSource;
        gdInt::pDataSource = new DataSource(gdInt::pGenerativeModel->getDataSource());
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
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
        ::Rf_error("%s", e.c_str());
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
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
std::vector<float> gdDataSourceGetNormalizedDataRandomReference(int rowCount) {
    try {
        if(gdInt::pDataSource == 0) {
            throw string("No datasource");
        }

        std::vector<float> v;
        v.reserve(rowCount * gdInt::pDataSource->getDimension());
        gdInt::pDataSource->getNormalizedDataRandomReference(v, rowCount);
        return v;
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
std::vector<float> gdDataSourceGetDataRandomPercent(float percent) {
    try {
        if(gdInt::pDataSource == 0) {
            throw string("No datasource");
        }

        vector<int> randomIndices = RandomIndicesWithoutReplacement()(gdInt::pDataSource->getSize(), percent);

        vector<float> dataSource;
        for(int i = 0; i < (int)randomIndices.size(); i++) {
            int index = randomIndices[i];
            vector<float> numberVector = gdInt::pDataSource->getRow(index);
            dataSource.insert(dataSource.end(), numberVector.begin(), numberVector.end());
        }

        return dataSource;
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
std::vector<float> gdGenerativeDataGetDenormalizedDataRandom(float percent) {
    try {
        if(gdInt::pGenerativeData == 0) {
            throw string("No generative data");
        }

        vector<int> randomIndices = RandomIndicesWithoutReplacement()(gdInt::pGenerativeData->getNormalizedSize(), percent);

        vector<float> denormalizedGenerativeData;
        for(int i = 0; i < (int)randomIndices.size(); i++) {
            int index = randomIndices[i];
            vector<float> denormalizedNumberVector = ((DataSource*)gdInt::pGenerativeData)->getDenormalizedRow(index);
            denormalizedGenerativeData.insert(denormalizedGenerativeData.end(), denormalizedNumberVector.begin(), denormalizedNumberVector.end());
        }

        return denormalizedGenerativeData;
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
std::vector<std::vector<float>> gdGenerativeDataGetDenormalizedDataRandomWithDensities(float percent) {
    try {
        if(gdInt::pGenerativeData == 0) {
            throw string("No generative data");
        }

        if(gdInt::pGenerativeData->getDensityVector()->getNormalizedValueVector().size() == 0) {
            throw string(cNoDensities);
        }

        vector<int> randomIndices = RandomIndicesWithoutReplacement()(gdInt::pGenerativeData->getNormalizedSize(), percent);

        vector<float> densityVector(randomIndices.size(), 0);
        vector<float> denormalizedGenerativeDataVector;
        for(int i = 0; i < (int)randomIndices.size(); i++) {
            int index = randomIndices[i];
            vector<float> denormalizedNumberVector = ((DataSource*)gdInt::pGenerativeData)->getDenormalizedRow(index);
            denormalizedGenerativeDataVector.insert(denormalizedGenerativeDataVector.end(), denormalizedNumberVector.begin(), denormalizedNumberVector.end());

            densityVector[i] = gdInt::pGenerativeData->getDensityVector()->getNormalizedValueVector()[index];
        }

        vector<vector<float>> denormalizeGnerativeDataWithDensities;
        denormalizeGnerativeDataWithDensities.push_back(denormalizedGenerativeDataVector);
        denormalizeGnerativeDataWithDensities.push_back(densityVector);
        return denormalizeGnerativeDataWithDensities;
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
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
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
void gdAddValueRows(const std::vector<float>& valueRows) {
    try {
        if(gdInt::pGenerativeData == 0) {
            throw string("No generative data");
        }

        gdInt::pGenerativeData->addValueLines(valueRows);
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
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
//' \dontrun{
//' gdRead("gd.bin")
//' gdGetNumberOfRows()}
// [[Rcpp::export]]
int gdGetNumberOfRows() {
    try {
        if(gdInt::pGenerativeData == 0) {
            throw string("No gnerative data");
        }

        return gdInt::pGenerativeData->getNormalizedSize();
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
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

        for(int i = 0; i < (int)indexVector.size(); i++) {
            indexVector[i] -= 1;
        }
        vector<wstring> columnNames = gdInt::pGenerativeData->getColumnNames(indexVector);
        return columnNames;
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
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
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
std::vector<std::wstring> gdGetNumberVectorIndexNames(std::vector<int>& numberVectorIndices) {
    try {
        if(gdInt::pGenerativeData == 0) {
            throw string("No generative data");
        }

        vector<int> indexVector = numberVectorIndices;
        for(int i = 0; i < (int)indexVector.size(); i++) {
            indexVector[i] -= 1;
        }
        vector<wstring> numberVectorIndexNames = gdInt::pGenerativeData->getNumbeVectorIndexNames(indexVector);
        return numberVectorIndexNames;
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

//' Get a row in generative data
//'
//' Get a row in generative data for a row index
//'
//' @param index Index of row
//'
//' @return List containing a denormalized row in normalized generative data.
//' @export
//'
//' @examples
//' \dontrun{
//' gdRead("gd.bin")
//' gdGetRow(1000)}
// [[Rcpp::export]]
List gdGetRow(int index) {
    try {
        if(gdInt::pGenerativeData == 0) {
            throw string("No generative data");
        }

        List list;
        vector<Column*> const & columnVector = gdInt::pGenerativeData->getColumnVector();
        for(int i = 0; i < (int)columnVector.size(); i++) {
            Column::COLUMN_TYPE type = columnVector[i]->getColumnType();
            if(type == Column::NUMERICAL) {
                vector<float> numberVector = columnVector[i]->getDenormalizedNumberVector(index - 1);
                float value = numberVector[0];
                list.insert(list.end(), value);
            } else if(type == Column::NUMERICAL_ARRAY) {
                NumberArrayColumn* pNumberArrayColumn = dynamic_cast<NumberArrayColumn*>(columnVector[i]);
                wstring value = pNumberArrayColumn->getMaxValue(index - 1);
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
float gdGetMax(int i) {
    try {
        if(gdInt::pGenerativeData == 0) {
            throw string("No generative data");
        }

        if(i - 1 < 0 || i - 1 > gdInt::pGenerativeData->getDimension() - 1) {
            throw string(cInvalidColumnIndex);
        }
        int j = gdInt::pGenerativeData->getColumnIndex(i - 1);
        vector<Column*> const& columnVector = gdInt::pGenerativeData->getColumnVector();

        float max = 0;
        Column::COLUMN_TYPE type = columnVector[j]->getColumnType();
        if(type == Column::NUMERICAL) {
            NumberColumn* pNumberColumn = dynamic_cast<NumberColumn*>(columnVector[j]);
            max = pNumberColumn->getMax();
        } else if(type == Column::NUMERICAL_ARRAY) {
            NumberArrayColumn* pNumberArrayColumn = dynamic_cast<NumberArrayColumn*>(columnVector[j]);
            max = pNumberArrayColumn->getMax();
        } else {
            throw string(cInvalidColumnType);
        }
        return max;
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
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

        if(i - 1 < 0 || i - 1 > gdInt::pGenerativeData->getDimension() - 1) {
            throw string(cInvalidColumnIndex);
        }
        int j = gdInt::pGenerativeData->getColumnIndex(i - 1);
        vector<Column*> const& columnVector = gdInt::pGenerativeData->getColumnVector();

        float min = 0;
        Column::COLUMN_TYPE type = columnVector[j]->getColumnType();
        if(type == Column::NUMERICAL) {
            NumberColumn* pNumberColumn = dynamic_cast<NumberColumn*>(columnVector[j]);
            min = pNumberColumn->getMin();
        } else if(type == Column::NUMERICAL_ARRAY) {
            NumberArrayColumn* pNumberArrayColumn = dynamic_cast<NumberArrayColumn*>(columnVector[j]);
            min = pNumberArrayColumn->getMin();
        } else {
            throw string(cInvalidColumnType);
        }
        return min;
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
void gdResetDensitiyValues() {
    try {
        if(gdInt::pGenerativeData == 0) {
            throw string("No generative data");
        }

        gdInt::pGenerativeData->getDensityVector()->clear();
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
void gdIntCalculateDensityValues() {
    try {
        if(gdInt::pGenerativeData == 0) {
            throw string("No generative data");
        }

        VpGenerativeData vpGenerativeData(*gdInt::pGenerativeData);
        L2Distance l2Distance;
        Progress progress(gdInt::pGenerativeData->getNormalizedSize());
        VpTree vpTree;
        vpTree.build(&vpGenerativeData, &l2Distance, 0);

        Density density(*gdInt::pGenerativeData, &vpTree, gdInt::nNearestNeighbors, &progress);
        density.calculateDensityValues();

        progress(gdInt::pGenerativeData->getNormalizedSize());
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

//' Calculate density value for a data record
//'
//' Calculate density value for a data record.
//' By default for the calculation a linear search is performed on normalized
//' generative data. When a search tree is used search is performed on a tree
//' for generative data which is built once in the first function call.
//'
//' @param dataRecord List containing an unnormalized data record.
//' @param useSearchTree Boolean value indicating if a search tree should be used.
//'
//' @return Normalized density value number
//' @export
//'
//' @examples
//' \dontrun{
//' gdRead("gd.bin")
//' dv <- gdCalculateDensityValue(list(6.1, 2.6, 5.6, 1.4))}
// [[Rcpp::export]]
float gdCalculateDensityValue(List dataRecord, bool useSearchTree = false) {
    try {
        if(gdInt::pGenerativeData == 0) {
            throw string("No generative data");
        }

        vector<float> numberVector;
        for(List::iterator iterator = dataRecord.begin(); iterator != dataRecord.end(); ++iterator) {
          float number = (float)as<double>(*iterator);
          numberVector.push_back(number);
        }

        if(useSearchTree) {
            if(gdInt::pDensityVpTree == 0) {
                delete gdInt::pDensityVpTree;
                gdInt::pDensityVpTree = new VpTree();
                Progress progress(gdInt::pGenerativeData->getNormalizedSize());
                delete gdInt::pDensityVpTreeData;
                gdInt::pDensityVpTreeData = new VpGenerativeData(*gdInt::pGenerativeData);
                delete gdInt::pDensityLpDistance;
                gdInt::pDensityLpDistance = new L2Distance;

                gdInt::pDensityVpTree->build(gdInt::pDensityVpTreeData, gdInt::pDensityLpDistance, &progress);
            }
        }

        float d = 0;
        if(useSearchTree) {
            Density density(*gdInt::pGenerativeData, gdInt::pDensityVpTree, gdInt::nNearestNeighbors, 0);
            d = density.calculateDensityValue(numberVector);
        } else {
            VpGenerativeData vpGenerativeData(*gdInt::pGenerativeData);
            L2Distance l2Distance;
            VpTree vpTree(&vpGenerativeData, &l2Distance, 0);
            Density density(*gdInt::pGenerativeData, &vpTree, gdInt::nNearestNeighbors, 0);
            d = density.calculateDensityValue(numberVector);
        }
        return d;
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

//' Calculate density value quantile
//'
//' Calculate density value quantile for a percent value.
//'
//' @param percent Percent value
//'
//' @return Normalized density value quantile number
//' @export
//'
//' @examples
//' \dontrun{
//' gdRead("gd.bin")
//' gdDensityValueQuantile(50)}
// [[Rcpp::export]]
float gdDensityValueQuantile(float percent) {
    try {
        if(gdInt::pGenerativeData == 0) {
            throw string("No generative data");
        }

        if(gdInt::pGenerativeData->getDensityVector()->getNormalizedValueVector().size() == 0) {
            throw string(cNoDensities);
        }

        VpGenerativeData vpGenerativeData(*gdInt::pGenerativeData);
        L2Distance l2Distance;
        VpTree vpTree(&vpGenerativeData, &l2Distance, 0);

        Density density(*gdInt::pGenerativeData, &vpTree, gdInt::nNearestNeighbors, 0);
        float q = density.calculateQuantile(percent);
        return q;
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

//' Calculate inverse density value quantile
//'
//' Calculate inverse density value quantile for a density value.
//'
//' @param densityValue Normalized density value
//'
//' @return Percent value
//' @export
//'
//' @examples
//' \dontrun{
//' gdRead("gd.bin")
//' gdDensityValueInverseQuantile(0.5)}
// [[Rcpp::export]]
float gdDensityValueInverseQuantile(float densityValue) {
    try {
        if(gdInt::pGenerativeData == 0) {
            throw string("No generative data");
        }

        if(gdInt::pGenerativeData->getDensityVector()->getNormalizedValueVector().size() == 0) {
            throw string(cNoDensities);
        }

        Density density(*gdInt::pGenerativeData, 0, 0, 0);
        return density.calculateInverseQuantile(densityValue);
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
std::string gdBuildFileName(const std::string& fileName, float niveau) {
    try {
        return BuildFileName()(GetFileName()(fileName), GetExtension()(fileName));
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

//' Search for k nearest neighbors
//'
//' Search for k nearest neighbors in normalized generative data for a data record.
//' When the data record contains NA values only the non-NA values are considered in search.
//' By default a linear search is performed. When a search tree is used search is performed on a tree
//' which is built once in the first function call.
//' Building a tree is also triggered when NA values in data records change in subsequent function calls.
//'
//' @param dataRecord List containing an unnormalized data record
//' @param k Number of nearest neighbors
//' @param useSearchTree Boolean value indicating if a search tree should be used.
//'
//' @return A list of denormalized rows in normalized generative data
//' @export
//'
//' @examples
//' \dontrun{
//' gdRead("gd.bin")
//' gdKNearestNeighbors(list(5.1, 3.5, 1.4, 0.2), 3)}
// [[Rcpp::export]]
List gdKNearestNeighbors(List dataRecord, int k = 1, bool useSearchTree = false) {
    try {
        if(gdInt::pGenerativeData == 0) {
            throw string("No generative data");
        }

        if(!(k >= 1)) {
            throw string("k must be greater than or equal to 1");
        }

        vector<Column*>& columnVector = gdInt::pGenerativeData->getColumnVector();
        if((int)columnVector.size() != dataRecord.length()) {
            throw string("Invalid length of data record");
        }

        vector<float> numberVector;
        int isnanCount = 0;

        //Function f("message");
        for(int i = 0; i < (int)columnVector.size(); i++) {
            Column::COLUMN_TYPE columnType = columnVector[i]->getColumnType();
            if(columnType == Column::NUMERICAL) {
                float number = (float)as<double>(dataRecord[i]);
                numberVector.push_back(number);
                if(isnan(number)) {
                    isnanCount++;
                }
            } else if(columnType == Column::NUMERICAL_ARRAY) {
                NumberArrayColumn* pNumberArrayColumn = dynamic_cast<NumberArrayColumn*>(columnVector[i]);

                float number;
                try {
                    number = (float)as<double>(dataRecord[i]);
                }
                catch(...) {
                    ;
                }

                wstring value;
                if(isnan(number)) {
                    value = cNA;
                } else {
                    value = as<wstring>(dataRecord[i]);
                }

                vector<float> columnNumberVector = pNumberArrayColumn->getNormalizedNumberVector(value);
                if(value == cNA) {
                    for (int j = 0; j < (int)columnNumberVector.size(); j++) {
                        columnNumberVector[j] = nan("");
                    }
                    isnanCount++;
                }
                numberVector.insert(numberVector.end(), columnNumberVector.begin(), columnNumberVector.end());
            } else {
                throw string(cInvalidColumnType);
            }
        }

        if(gdInt::pGenerativeData->getDimension() != (int)numberVector.size()) {
            throw string(cInvalidDimension);
        }
        if(isnanCount == (int)columnVector.size()) {
            return dataRecord;
        }

        NormalizeData normalizeData;
        vector<float> normalizedNumberVector = normalizeData.getNormalizedNumberVector(*gdInt::pGenerativeData, numberVector);

        if(useSearchTree) {
            bool build = false;
            if(gdInt::pVpTree == 0) {
                build = true;
            } else {
                if(gdInt::pVpTree->isBuilt()) {
                    L2DistanceNanIndexed l2DistanceNanIndexed = dynamic_cast<L2DistanceNanIndexed&>(gdInt::pVpTree->getLpDistance());
                    for(int i = 0; i < (int)numberVector.size(); i++) {
                        if(isnan(numberVector[i]) != isnan(l2DistanceNanIndexed._distance[i])) {
                            build = true;
                            break;
                        }
                    }
                } else {
                    build = true;
                }
            }

            if(build) {
                delete gdInt::pVpTree;
                gdInt::pVpTree = new VpTree();
                Progress progress(gdInt::pGenerativeData->getNormalizedSize());
                delete gdInt::pVpTreeData;
                gdInt::pVpTreeData = new VpGenerativeData(*gdInt::pGenerativeData);
                delete gdInt::pLpDistance;
                gdInt::pLpDistance = new L2DistanceNanIndexed(numberVector);

                gdInt::pVpTree->build(gdInt::pVpTreeData, gdInt::pLpDistance, &progress);
            }
        }

        vector<VpElement> nearestNeighbours;
        if(useSearchTree) {
            gdInt::pVpTree->search(normalizedNumberVector,  k, nearestNeighbours);
        } else {
            VpGenerativeData vpGenerativeData(*gdInt::pGenerativeData);
            L2DistanceNanIndexed l2DistanceNanIndexed(numberVector);
            VpTree vpTree(&vpGenerativeData, &l2DistanceNanIndexed, 0);
            vpTree.linearSearch(normalizedNumberVector,  k, nearestNeighbours);
        }

        List completeDataRecordList;
        if(nearestNeighbours.size() == 0) {
            return completeDataRecordList;
        }

        for(int i = 0; i < (int) nearestNeighbours.size(); i++) {
            List completeDataRecord;
            for(int j = 0; j < (int)columnVector.size(); j++) {
                Column::COLUMN_TYPE columnType = columnVector[j]->getColumnType();
                if(columnType == Column::NUMERICAL) {
                    vector<float> numberVector = columnVector[j]->getDenormalizedNumberVector(nearestNeighbours[i].getIndex());
                    float value = numberVector[0];
                    completeDataRecord.insert(completeDataRecord.end(), value);
                } else if(columnType == Column::NUMERICAL_ARRAY) {
                    NumberArrayColumn* pNumberArrayColumn = dynamic_cast<NumberArrayColumn*>(columnVector[j]);
                    wstring value = pNumberArrayColumn->getMaxValue(nearestNeighbours[i].getIndex());
                    completeDataRecord.insert(completeDataRecord.end(), value);
                    if(value == cNA) {
                        isnanCount++;
                    }
                } else {
                    throw string(cInvalidColumnType);
                }
            }
            completeDataRecordList.insert(completeDataRecordList.end(), completeDataRecord);
        }

        return completeDataRecordList;
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

//' Complete incomplete data record
//'
//' Search for first nearest neighbor in normalized generative data for incomplete data record containing NA values.
//' Found row in generative data is then used to replace NA values in inccomplete data record. This function calls
//' gdKNearestNeighbors() with parameter k equal to 1.
//'
//' @param dataRecord List containing an incomplete unnormalized data record
//' @param useSearchTree Boolean value indicating if a search tree should be used.
//'
//' @return List containing completed denormalized data record
//' @export
//'
//' @examples
//' \dontrun{
//' gdRead("gd.bin")
//' gdComplete(list(5.1, 3.5, 1.4, NA))}
// [[Rcpp::export]]
List gdComplete(List dataRecord, bool useSearchTree = false) {
    try {
        List nearestNeighbors = gdKNearestNeighbors(dataRecord, 1, useSearchTree);

        if(nearestNeighbors.size() != 1) {
            throw(string(cInvalidNearestNeighborsSize));
        }
        List nearestNeighbor = as<List>(nearestNeighbors[0]);
        if(nearestNeighbor.size() != dataRecord.size()) {
            throw(string(cDifferentListSizes));
        }

        List completedList;
        vector<Column*>& columnVector = gdInt::pGenerativeData->getColumnVector();
        if((int)columnVector.size() != dataRecord.length()) {
            throw string("Invalid length of data record");
        }

        for(int i = 0; i < (int)columnVector.size(); i++) {
            Column::COLUMN_TYPE columnType = columnVector[i]->getColumnType();
            if(columnType == Column::NUMERICAL) {
                float number = (float)as<double>(dataRecord[i]);
                float nearestNeighborNumber = (float)as<double>(nearestNeighbor[i]);
                if(isnan(number)) {
                    completedList.insert(completedList.end(), nearestNeighborNumber);
                } else {
                    completedList.insert(completedList.end(), number);
                }
            } else if(columnType == Column::NUMERICAL_ARRAY) {
                float number;
                try {
                    number = (float)as<double>(dataRecord[i]);
                }
                catch(...) {
                    ;
                }

                wstring value;
                if(isnan(number)) {
                    value = cNA;
                } else {
                    value = as<wstring>(dataRecord[i]);
                }
                wstring nearestNeighborValue = as<wstring>(nearestNeighbor[i]);
                if(value == cNA) {
                    completedList.insert(completedList.end(), nearestNeighborValue);
                } else {
                    completedList.insert(completedList.end(), value);
                }
            } else {
                throw string(cInvalidColumnType);
            }
        }

        return completedList;
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
int gdGenerativeModelGetNumberOfTrainingIterations() {
    try {
        if(gdInt::pGenerativeModel == 0) {
            throw string("No generative model");
        }

       return gdInt::pGenerativeModel->getNumberOfTrainingIterations();
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
void gdGenerativeModelSetNumberOfTrainingIterations(int numberOfTrainingIterations) {
    try {
        if(gdInt::pGenerativeModel == 0) {
            throw string("No generative model");
        }

        gdInt::pGenerativeModel->setNumberOfTrainingIterations(numberOfTrainingIterations);
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
int gdGenerativeModelGetNumberOfInitializationIterations() {
    try {
        if(gdInt::pGenerativeModel == 0) {
            throw string("No generative model");
        }

        return gdInt::pGenerativeModel->getNumberOfInitializationIterations();
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
void gdGenerativeModelSetNumberOfInitializationIterations(int numberOfInitializationIterations) {
    try {
        if(gdInt::pGenerativeModel == 0) {
            throw string("No generative model");
        }

        gdInt::pGenerativeModel->setNumberOfInitializationIterations(numberOfInitializationIterations);
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
int gdGenerativeModelGetNumberOfHiddenLayerUnits() {
    try {
        if(gdInt::pGenerativeModel == 0) {
            throw string("No generative model");
        }

        return gdInt::pGenerativeModel->getNumberOfHiddenLayerUnits();
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
void gdGenerativeModelSetNumberOfHiddenLayerUnits(int numberOfHiddenLayerUnits) {
    try {
        if(gdInt::pGenerativeModel == 0) {
            throw string("No generative model");
        }

        gdInt::pGenerativeModel->setNumberOfHiddenLayerUnits(numberOfHiddenLayerUnits);
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
float gdGenerativeModelGetLearningRate() {
    try {
        if(gdInt::pGenerativeModel == 0) {
            throw string("No generative model");
        }

        return gdInt::pGenerativeModel->getLearningRate();
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
void gdGenerativeModelSetLearningRate(float learningRate) {
    try {
        if(gdInt::pGenerativeModel == 0) {
            throw string("No generative model");
        }

        gdInt::pGenerativeModel->setLearningRate(learningRate);
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
float gdGenerativeModelGetDropout() {
    try {
        if(gdInt::pGenerativeModel == 0) {
            throw string("No generative model");
        }

        return gdInt::pGenerativeModel->getDropout();
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
void gdGenerativeModelSetDropout(float dropout) {
    try {
        if(gdInt::pGenerativeModel == 0) {
            throw string("No generative model");
        }

        gdInt::pGenerativeModel->setDropout(dropout);
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}

// [[Rcpp::export]]
bool gdDataSourceHasActiveStringColumn() {
    try {
        if(gdInt::pDataSource == 0) {
            throw string("No datasource");
        }

        bool activeStringColumn = false;
        vector<Column*> const & columnVector = gdInt::pDataSource->getColumnVector();
        for(int j = 0; j < (int)columnVector.size(); j++) {
            Column::COLUMN_TYPE type = columnVector[j]->getColumnType();
            if(type == Column::STRING && columnVector[j]->getActive()) {
                activeStringColumn= true;
            }
        }

        return activeStringColumn;
    } catch (const string& e) {
        ::Rf_error("%s", e.c_str());
    } catch(...) {
        ::Rf_error("C++ exception (unknown reason)");
    }
}
