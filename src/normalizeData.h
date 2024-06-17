// Copyright 2021 Werner Mueller
// Released under the GPL (>= 2)

#ifndef NORMALIZE_DATA
#define NORMALIZE_DATA

#include <cmath>

#include "dataSource.h"

#define GD_RCPP

#ifdef GD_RCPP
#include <Rcpp.h>
#endif

using namespace std;

class NormalizeData {
public:
	NormalizeData() {}

	void normalize(DataSource& dataSource, bool calculateMinMax = true) {
		for(int i = 0; i < (int)dataSource.getColumnVector().size(); i++) {
		    if((dataSource.getColumnVector()[i])->getActive()) {
		        normalize(dataSource.getColumnVector()[i], calculateMinMax);
		    }
        }
		dataSource.setNormalized(true);
	}

    void normalize(Column* pColumn, bool calculateMinMax) {
        Column::COLUMN_TYPE type = pColumn->getColumnType();
        if(type == Column::NUMERICAL) {
            NumberColumn* pNumberColumn = dynamic_cast<NumberColumn*>(pColumn);
            NumberColumn::SCALE_TYPE scaleType = pNumberColumn->getScaleType();
            float fMax = numeric_limits<float>::min();
            float fMin = numeric_limits<float>::max();
            for(int i = 0; i < (int)pNumberColumn->getValueVector().size(); i++) {
                float number = pNumberColumn->getValueVector()[i];
                if(isnan(number)) {
                    continue;    
                }
                
                if(number > fMax) {
                    fMax = number;
                }
                if(number < fMin) {
                    fMin = number;
                }
            }
            if(calculateMinMax) {
                if(fMax == numeric_limits<float>::min()) {
                    fMax = 1.0;
                }
                if(fMin == numeric_limits<float>::max()) {
                    fMin = 0.0;
                }
                pNumberColumn->setMax(fMax);
                pNumberColumn->setMin(fMin);
            }
            
            pNumberColumn->getNormalizedValueVector().resize(pNumberColumn->getValueVector().size(), 0);
            for(int j = 0; j < (int)pNumberColumn->getValueVector().size(); j++) {
                if(isnan(pNumberColumn->getValueVector()[j])) {
                    pNumberColumn->getNormalizedValueVector()[j] = pNumberColumn->getValueVector()[j];
                    continue;    
                }
                
                float normalizedNumber = 0;
                if(scaleType == NumberColumn::LINEAR) {
                    if(pNumberColumn->getMax() - pNumberColumn->getMin() > 0) {
                        normalizedNumber = (pNumberColumn->getValueVector()[j] - pNumberColumn->getMin()) /
                            (pNumberColumn->getMax() - pNumberColumn->getMin());
                    } else {
                        if(pNumberColumn->getMax() > 0) {
                            //normalizedNumber = pNumberColumn->getMax();
                            normalizedNumber = 1;
                        }
                    }
                } else if(scaleType == NumberColumn::LOGARITHMIC) {
                    if(pNumberColumn->getMax() - pNumberColumn->getMin() > 0) {
                        normalizedNumber = log(pNumberColumn->getValueVector()[j] - pNumberColumn->getMin() + 1) /
                            log(pNumberColumn->getMax() - pNumberColumn->getMin() + 1);
                    } else {
                        if(pNumberColumn->getMax() > 0) {
                            //normalizedNumber = pNumberColumn->getMax();
                            normalizedNumber = 1;
                        }
                    }
                } else {
                    throw cInvalidScaleType;
                }
                pNumberColumn->getNormalizedValueVector()[j] = normalizedNumber;
            }
        } else if(type == Column::STRING) {
            /*
            string invalidColumnType = cInvalidColumnPrefix + " ";
            for(int i = 0; i < pColumn->getName().length(); i++) {
                char c = static_cast<char>(pColumn->getName()[i]);
                invalidColumnType += c;
            }
            invalidColumnType += " " + cInvalidTypeSuffix;;
                
            throw string(invalidColumnType);
            */
            ;
        } else {
            string invalidColumnType = cInvalidColumnPrefix + " ";
            for(int i = 0; i < (int)pColumn->getName().length(); i++) {
                char c = static_cast<char>(pColumn->getName()[i]);
                invalidColumnType += c;
            }
            invalidColumnType += " " + cInvalidTypeSuffix;;
            
            throw string(invalidColumnType);
        }
    }
    
    vector<float> getNormalizedNumberVector(DataSource& dataSource, vector<float>& numberVector) {
        if(dataSource.getDimension() != (int)numberVector.size()) {
            throw string(cInvalidDimension);
        }
        
        vector<float> normalizedNumberVector;
        int j = 0;
        vector<Column*>& columnVector = dataSource.getColumnVector();
        for(int i = 0; i < (int)columnVector.size(); i++) {
            if((dataSource.getColumnVector()[i])->getActive()) {
                Column::COLUMN_TYPE type = columnVector[i]->getColumnType();
                if(type == Column::NUMERICAL) {
                    float normalizedNumber = getNormalizedNumber(columnVector[i], numberVector[j]);
                    j++;
                    normalizedNumberVector.push_back(normalizedNumber);
                } else if(type == Column::NUMERICAL_ARRAY) {
                    vector<float> columnNumberVector;
                    columnNumberVector.insert(columnNumberVector.begin(), numberVector.begin() + j, numberVector.begin() + j + columnVector[i]->getDimension());
                    j += columnVector[i]->getDimension();
                    normalizedNumberVector.insert(normalizedNumberVector.end(), columnNumberVector.begin(), columnNumberVector.end());
                } else{
                    throw string(cInvalidColumnType);
                }
            }
        }
        return normalizedNumberVector;
    }
    float getNormalizedNumber(Column* pColumn, float number, bool limit = false) {
        if(isnan(number)) {
            return number;    
        }
        
        Column::COLUMN_TYPE type = pColumn->getColumnType();
        if(type == Column::NUMERICAL) {
            NumberColumn* pNumberColumn = dynamic_cast<NumberColumn*>(pColumn);
            NumberColumn::SCALE_TYPE scaleType = pNumberColumn->getScaleType();
            
            if(limit) {
                if(number < pNumberColumn->getMin()) {
                    number = pNumberColumn->getMin();
                }
                if(number > pNumberColumn->getMax()) {
                    number = pNumberColumn->getMax();
                }
            }
            
            float normalizedNumber = 0;
            if(scaleType == Column::LINEAR) {
                if(pNumberColumn->getMax() - pNumberColumn->getMin() > 0) {
                    normalizedNumber = (number - pNumberColumn->getMin()) / (pNumberColumn->getMax() - pNumberColumn->getMin());
                } else {
                    if(pNumberColumn->getMax() > 0) {
                        //normalizedNumber = pNumberColumn->getMax();
                        normalizedNumber = 1;
                    }
                }
            }else if(scaleType == NumberColumn::LOGARITHMIC) {
                if(pNumberColumn->getMax() - pNumberColumn->getMin() > 0) {
                    normalizedNumber = log(number - pNumberColumn->getMin() + 1) /
                        log(pNumberColumn->getMax() - pNumberColumn->getMin() + 1);
                } else {
                    if(pNumberColumn->getMax() > 0) {
                        //normalizedNumber = pNumberColumn->getMax();
                        normalizedNumber = 1;
                    }
                }
            } else {
                throw cInvalidScaleType;
            }
            return normalizedNumber;
        } else {
            throw string(cInvalidColumnType);
        }
    }
    float getDenormalizedNumber(Column* pColumn, float number) {
        if(isnan(number)) {
            return number;
        }
        
        Column::COLUMN_TYPE type = pColumn->getColumnType();
        if(type == Column::NUMERICAL) {
            NumberColumn* pNumberColumn = dynamic_cast<NumberColumn*>(pColumn);
            NumberColumn::SCALE_TYPE scaleType = pNumberColumn->getScaleType();
        
            float denormalizedNumber;
            if(scaleType == Column::LINEAR) {
                denormalizedNumber = pNumberColumn->getMin() + (pNumberColumn->getMax() - pNumberColumn->getMin()) * number;
            } else if(scaleType == NumberColumn::LOGARITHMIC) {
                denormalizedNumber = pNumberColumn->getMin() - 1 + exp(number * log(pNumberColumn->getMax() - pNumberColumn->getMin() + 1));
            } else {
                throw cInvalidScaleType;
            }
            return denormalizedNumber;
        } else {
            throw string(cInvalidColumnType);
        }
    }
};

#endif
