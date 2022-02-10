# Copyright 2021 Werner Mueller
# Released under the GPL (>= 2)

library(Rcpp)

Sys.setenv("PKG_CXXFLAGS"="-std=c++11")
sourceCpp("src/gdInt.cpp")

#' Calculate density values for generative data
#' 
#' Read generative data from a file, calculate density values and write generative data with density values to original file.
#' Calculated density values are used to classiy generative data.
#' In function gdPlotParameters() density value thresholds with assigned colors can be passed
#' to draw generative data for different density value ranges.
#'
#' @param generativeDataFileName Name of generative data file name
#'
#' @return None
#' @export
#'
#' @examples
#' \dontrun{gdCalculateDensityValues("gd.bin")}
gdCalculateDensityValues <- function(generativeDataFileName) {
  gdReset()
  
  gdGenerativeDataRead(generativeDataFileName)
  gdIntCalculateDensityValues()
  gdGenerativeDataWrite(generativeDataFileName)
}

