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
#' @param numberOfTrainingIterations Number of training iterations
#' @param numberOfInitializationIterations Number of initialization iterations
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
#' generateParameters <- gdGenerateParameters(numberOfTrainingIterations = 10000)}
gdTrainParameters <- function(numberOfTrainingIterations = 10000,
                              numberOfInitializationIterations = 1500,
                              numberOfHiddenLayerUnits = 1024,
                              learningRate = 0.00007,
                              dropout = 0.1) {
    parameters <- list(numberOfTrainingIterations = numberOfTrainingIterations,
                       numberOfInitializationIterations = numberOfInitializationIterations,
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
#' trainParameters <- gdTrainParameters(numberOfTrainingIterations = 10000)
#' gdTrain("gm.bin", "gd.bin", "ds.bin", c(1, 2), trainParameters)}
gdTrain <- function(generativeModelFileName,
                    generativeDataFileName,
                    dataSourceFileName,
                    columnIndices,
                    trainParameters = gdTrainParameters(numberOfTrainingIterations = 10000,
                                                        numberOfInitializationIterations = 1500,
                                                        numberOfHiddenLayerUnits = 1024,
                                                        learningRate = 0.00007,
                                                        dropout = 0.05)) {
    if(trainParameters$numberOfTrainingIterations > gdGetMaxSize() / gdGetBatchSize()) {
        stop("Max number of iterations above limit")
    }
    
    start <- Sys.time()
    
    gdReset()
    gdDataSourceRead(dataSourceFileName)
    
    if(gdDataSourceHasActiveStringColumn()) {
        stop("Data source contains columns that have not type R-class numeric, R-type double.\nFor training eihter these columns have to be deactivated in the data source or\nthe software service for accelerated training of generative models with support for mixed numerical and categorical variables has to be used.")
    }
    
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
        stop("No generateModelFileName specified")
    }
    gdTrainGenerate(generativeModelFileName, generativeDataFileName, columnIndices, trainParameters, NULL, generativeModelRead)
    
    end <- Sys.time()
    message(round(difftime(end, start, units = "secs"), 3), " seconds")
}


      