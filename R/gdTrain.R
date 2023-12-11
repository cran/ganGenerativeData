# Copyright 2021 Werner Mueller
# Released under the GPL (>= 2)

library(tensorflow)
library(Rcpp)

Sys.setenv("PKG_CXXFLAGS"="-std=c++17")
sourceCpp("src/gdInt.cpp")
source("R/gdTrainGenerate.R")

#' Specify parameters for training of generative model
#' 
#' Specify parameters for training of neural networks used for generation of
#' generative data. These parameters are passed to function gdTrain().
#'
#' @param numberOfIterations Number of training iterations
#' @param numberOfHiddenLayerUnits Number of hidden layer units
#' @param learningRate Learning rate for training of neural networks
#' @param dropout Value in the range of 0 to 1. Specifies the rate of hidden
#' units that are dropped. Dropout is a regularization method to prevent
#' overfitting.
#' 
#' @return List of parameters for training of generative model
#' @export
#' 
#' @examples
#' \dontrun{
#' generateParameters <- gdGenerateParameters(numberOfIterations = 5000)}
gdTrainParameters <- function(numberOfIterations = 10000,
                              numberOfHiddenLayerUnits = 1024,
                              learningRate = 0.00007,
                              dropout = 0.05) {
    parameters <- list(numberOfIterations = numberOfIterations,
                       numberOfHiddenLayerUnits = numberOfHiddenLayerUnits,
                       learningRate = learningRate,
                       dropout = dropout)
}

#' Train a generative model for a data source
#' 
#' Read a data source from a file, train a generative model that generates 
#' generative data for the data source in iterative training steps, write
#' trained generative model and generated data in training steps to a file
#' in binary format. When a higher number of iterations is used the
#' distribution of generated data will get closer to that of the data source.
#' When a name of an existing generative model file is passed training will be
#' continued.
#'
#' @param generativeModelFileName Name of generative model file
#' @param generativeDataFileName Name of generative data file. When name is NULL
#' or empty string generated data will not be written to a file.
#' @param dataSourceFileName Name of data source file
#' @param columnIndices Vector of two column indices that are used to plot
#' two-dimensional projections of normalized generated generative data and data
#' source for a training step. Indices refer to indices of active columns of
#' data source. Plotting can be disabled by passing NULL or an empty vector.
#' @param trainParameters Generative model training parameters, see
#' function gdTrainParameters().
#' 
#' @return None
#' @export
#'
#' @examples
#' \dontrun{
#' trainParameters <- gdTrainParameters(numberOfIterations = 10000)
#' gdTrain("gm.bin", "gd.bin", "ds.bin", c(1, 2), trainParameters)}
gdTrain <- function(generativeModelFileName,
                    generativeDataFileName,
                    dataSourceFileName,
                    columnIndices,
                    trainParameters = gdTrainParameters(numberOfIterations = 10000,
                                                        numberOfHiddenLayerUnits = 1024,
                                                        learningRate = 0.00007,
                                                        dropout = 0.05)) {
    if(trainParameters$numberOfIterations > gdGetMaxSize() / gdGetBatchSize()) {
        message("Max number of iterations exceeded")
        return()
    }
    
    start <- Sys.time()
    
    gdReset()
    gdDataSourceRead(dataSourceFileName)
    
    generativeDataRead <- FALSE
    if(!is.null(generativeDataFileName) && nchar(generativeDataFileName) > 0) {
        generativeDataRead <- gdGenerativeDataRead(generativeDataFileName)
        if(generativeDataRead) {
            gdResetDensitiyValues();
        } else {
            gdCreateGenerativeData()
        }
    } else {
        gdCreateGenerativeData()
    }
    
    generativeModelRead <- FALSE
    if(!is.null(generativeModelFileName) && nchar(generativeModelFileName) > 0) {
        generativeModelRead <- gdReadGenerativeModel(generativeModelFileName)
    } else {
        message("No generateModelFileName specified")
        return()
    }
    gdTrainGenerate(generativeModelFileName, generativeDataFileName, columnIndices, trainParameters, NULL, generativeModelRead)
    
    end <- Sys.time()
    message(round(difftime(end, start, units = "secs"), 3), " seconds")
}
