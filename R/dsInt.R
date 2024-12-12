# Copyright 2021 Werner Mueller
# Released under the GPL (>= 2)

library(Rcpp)

Sys.setenv("PKG_CXXFLAGS"="-std=c++17")
sourceCpp("src/dsInt.cpp")

#' Create a data source with passed data frame
#'
#' Create a data source with passed data frame.
#'
#' @param dataFrame Name of data frame
#'
#' @return None
#' @export
#'
#' @examples
#' \donttest{
#' dsCreateWithDataFrame(iris)}
dsCreateWithDataFrame <- function (dataFrame) {
    if(!is.data.frame(dataFrame)) {
        stop("Passed object is not a data frame\n")
    }
    columnTypes <- sapply(dataFrame, typeof)
    columnNames <- colnames(dataFrame)
  
    dsCreate(columnTypes, columnNames)
    for(i in 1:nrow(dataFrame)) {
        values <- sapply(dataFrame[i,], as.character)
        dsAddValueRow(values);
    }
}

#' Calculate density values for data source
#' 
#' Read a data source from a file, calculate density values and write the data source with density values to original file.
#' Calculated density values are used to evaluate a data source.
#'
#' @param dataSourceFileName Name of data source file name
#' @param nNearestNeighbors number of used nearest neighbors
#'
#' @return None
#' @export
#'
#' @examples
#' \dontrun{
#' dsCalculateDensityValues("ds.bin")}
dsCalculateDensityValues <- function(dataSourceFileName, nNearestNeighbors) {
    start <- Sys.time()
    
    #dsReset()
    if(!is.null(dataSourceFileName) && nchar(dataSourceFileName) > 0) {
        if(!dsRead(dataSourceFileName)) {
            error <- append("File ", dataSourceFileName)
            error <- append(error, " could not be opened\n")
            message(error)
            stop(error)
            return()
        }
    } else {
        stop("No dataSourceFileName specified")
    }
    
    dsIntCalculateDensityValues(nNearestNeighbors)
    dsWrite(dataSourceFileName)
    
    end <- Sys.time()
    message(round(difftime(end, start, units = "secs"), 3), " seconds")
}
