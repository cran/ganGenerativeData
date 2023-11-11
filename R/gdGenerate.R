# Copyright 2021 Werner Mueller
# Released under the GPL (>= 2)

library(tensorflow)
library(Rcpp)

Sys.setenv("PKG_CXXFLAGS"="-std=c++17")
sourceCpp("src/gdInt.cpp")
source("R/gdTrainGenerate.R")

#' Specify parameters for generation of generative data
#' 
#' Specify parameters for generation of generative data. These parameters are
#' passed to function gdGenerate().
#'
#' @param numberOfSamples Number of generated samples
#' @param dropout Value in the range of 0 to 1. Specifies the rate of hidden
#' units that are dropped. Dropout is a regularization method to prevent
#' overfitting. See function gdTrainParameters().
#' 
#' @return List of parameters for generation of generative data
#' @export
#' 
#' @examples
#' \dontrun{
#' generateParameters <- gdGenerateParameters(numberOfSamples = 100000)}
gdGenerateParameters <- function(numberOfSamples = 100000,
                                 dropout = 0.05) {
    parameters <- list(numberOfSamples = numberOfSamples,
                       dropout = dropout)
}

#' Generate generative data for a data source
#' 
#' Read a trained generative model for a data source, generate generative data
#' and write generated data to a file in binary format.
#'
#' @param generativeDataFileName Name of generative data file
#' @param generativeModelFileName Name of generative model file
#' @param generateParameters Generation of generative data parameters, see
#' function gdGenerateParameters().
#' 
#' @return None
#' @export
#'
#' @examples
#' \dontrun{
#' generateParameters <- gdGenerateParameters(numberOfSamples = 10000)
#' gdGenerate("gd.bin", "gm.bin", generateParameters)}
gdGenerate <- function(generativeDataFileName,
                       generativeModelFileName,
                       generateParameters = gdGenerateParameters(numberOfSamples = 10000,
                                                                 dropout = 0.05)) {
    start <- Sys.time()
    
    gdReset()
    
    if(!is.null(generativeModelFileName) && nchar(generativeModelFileName) > 0) {
        generativeModelRead <- gdReadGenerativeModel(generativeModelFileName)
        if(generativeModelRead) {
            gdCreateDataSourceFromGenerativeModel()
            gdCreateGenerativeData()
        } else {
            error <- append("File ", generativeModelFileName)
            error <- append(error, " could not be opened\n")
            message(error)
            return()
        }
    } else {
        message("No generateModelFileName specified")
        return()
    }

    if(!is.null(generativeModelFileName) && nchar(generativeModelFileName) > 0) {
        generativeModelRead <- gdReadGenerativeModel(generativeModelFileName)
    } else {
        message("No generateModelFileName specified")
        return()
    }
    
    gdTrainGenerate(generativeModelFileName, generativeDataFileName, NULL, NULL, generateParameters, generativeModelRead)
    
    end <- Sys.time()
    message(round(difftime(end, start, units = "secs"), 3), " seconds")
}
