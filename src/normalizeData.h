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
            for(int i = 0; i < pNumberColumn->getValueVector().size(); i++) {
                float number = pNumberColumn->getValueVector()[i];
                if(number > fMax) {
                    fMax = number;
                }
                if(number < fMin) {
                    fMin = number;
                }
            }
            if(calculateMinMax) {
                pNumberColumn->setMax(fMax);
                pNumberColumn->setMin(fMin);
            }
            
            //Function f("message");
            //f("norm");
            //f(pNumberColumn->getMax());
            //f(pNumberColumn->getMin());
        
            pNumberColumn->getNormalizedValueVector().resize(pNumberColumn->getValueVector().size(), 0);
            for(int j = 0; j < (int)pNumberColumn->getValueVector().size(); j++) {
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
            throw string(cInvalidColumnType);
        } else {
            throw string(cInvalidColumnType);
        }
    }
    
    vector<float> getNormalizedNumberVector(DataSource& dataSource, vector<float>& numberVector) {
        if(dataSource.getDimension() != numberVector.size()) {
            throw string(cInvalidDimension);
        }
        
        vector<float> normalizedNumberVector;
        int j = 0;
        for(int i = 0; i < (int)dataSource.getColumnVector().size(); i++) {
            if((dataSource.getColumnVector()[i])->getActive()) {
                float normalizedNumber = getNormalizedNumber(dataSource.getColumnVector()[i], numberVector[j]);
                j++;
                normalizedNumberVector.push_back(normalizedNumber);
            }
        }
        return normalizedNumberVector;
    }
    float getNormalizedNumber(Column* pColumn, float number, bool limit = false) {
        if(isnan(number)) {
            return number;    
        }
        
        //Function f("message");
        //f("normalized");
            
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
                    //f(normalizedNumber);
                    //f(pNumberColumn->getMax());
                    //f(pNumberColumn->getMin());
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
        Column::COLUMN_TYPE type = pColumn->getColumnType();
        if(type == Column::NUMERICAL) {
            NumberColumn* pNumberColumn = dynamic_cast<NumberColumn*>(pColumn);
            NumberColumn::SCALE_TYPE scaleType = pNumberColumn->getScaleType();
        
            //Function f("message");
            //f("denormalized");
        
            float denormalizedNumber;
            if(scaleType == Column::LINEAR) {
                denormalizedNumber = pNumberColumn->getMin() + (pNumberColumn->getMax() - pNumberColumn->getMin()) * number;
            } else if(scaleType == NumberColumn::LOGARITHMIC) {
                denormalizedNumber = pNumberColumn->getMin() - 1 + exp(number * log(pNumberColumn->getMax() - pNumberColumn->getMin() + 1));
                //f(denormalizedNumber);
                //f(pNumberColumn->getMax());
                //f(pNumberColumn->getMin());
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
