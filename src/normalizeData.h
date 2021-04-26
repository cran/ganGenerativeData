// Copyright 2021 Werner Mueller
// Released under the GPL (>= 2)

#ifndef NORMALIZE_DATA
#define NORMALIZE_DATA

#include <math.h>
//#include <boost/accumulators/accumulators.hpp>
//#include <boost/accumulators/statistics.hpp>

#include "dataSource.h"

using namespace std;

class NormalizeData {
public:
	NormalizeData() {}

	void normalize(DataSource& dataSource, bool calculateMinMax = true) {
		for(int i = 0; i < (int)dataSource.getColumnVector().size(); i++) {
		    if((dataSource.getColumnVector()[i])->getActive()) {
    		    Column::COLUMN_TYPE type = (dataSource.getColumnVector()[i])->getColumnType();
    		    if(type == Column::NUMERICAL) {
                    NumberColumn* pNumberColumn = dynamic_cast<NumberColumn*>(dataSource.getColumnVector()[i]);
    		        NumberColumn::SCALE_TYPE scaleType = pNumberColumn->getScaleType();
    			    float fMax = numeric_limits<float>::min();
    			    float fMin = numeric_limits<float>::max();
    			    for(int i = 0; i < pNumberColumn->getValueVector().size(); i++) {
    			        float value = pNumberColumn->getValueVector()[i];
    			        if(value > fMax) {
    			            fMax = value;
    			        }
    			        if(value < fMin) {
    			            fMin = value;
    			        }
    			    }
    			    if(calculateMinMax) {
    			        pNumberColumn->setMax(fMax);
    			        pNumberColumn->setMin(fMin);
    			    }
    		    
    			    pNumberColumn->getNormalizedValueVector().resize(pNumberColumn->getValueVector().size(), 0);
    			    for(int j = 0; j < (int)pNumberColumn->getValueVector().size(); j++) {
    				    float normalizedValue = 0;
    				    if(pNumberColumn->getMax() - pNumberColumn->getMin() > 0) {
    				        if(scaleType == NumberColumn::LINEAR) {
      					        normalizedValue = (pNumberColumn->getValueVector()[j] - pNumberColumn->getMin()) /
    	  					        (pNumberColumn->getMax() - pNumberColumn->getMin());
    				        } else {
    					        throw cInvalidScaleType;
    				        }
    				    } else {
    					    normalizedValue = pNumberColumn->getMax();
    				    }
    				    pNumberColumn->getNormalizedValueVector()[j] = normalizedValue;
    			    }
    		    } else if(type == Column::STRING) {
    		        wstring columnName = (dataSource.getColumnVector()[i])->getName();
    		        string invalidActiveColumn(columnName.begin(), columnName.end());
    		        invalidActiveColumn = cInvalidActiveColumn + " " + invalidActiveColumn;
    		        throw string(invalidActiveColumn);
    		    } else {
    		        throw string(cInvalidColumnType);
    		    }
		    }
        }
		
		dataSource.setNormalized(true);
	}
};

#endif
