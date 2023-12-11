# Copyright 2021 Werner Mueller
# Released under the GPL (>= 2)

library(Rcpp)

Sys.setenv("PKG_CXXFLAGS"="-std=c++17")
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
#' \dontrun{
#' gdCalculateDensityValues("gd.bin")}
gdCalculateDensityValues <- function(generativeDataFileName) {
  start <- Sys.time()
  
  gdReset()
  if(!is.null(generativeDataFileName) && nchar(generativeDataFileName) > 0) {
    if(!gdGenerativeDataRead(generativeDataFileName)) {
       error <- append("File ", generativeDataFileName)
       error <- append(error, " could not be opened\n")
       message(error)
       return()
    }
  } else {
      message("No generateDataFileName specified")
      return()
  }
  
  gdIntCalculateDensityValues()
  gdGenerativeDataWrite(generativeDataFileName)
  
  end <- Sys.time()
  message(round(difftime(end, start, units = "secs"), 3), " seconds")
}

