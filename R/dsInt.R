# Copyright 2021 Werner Mueller
# Released under the GPL (>= 2)

library(Rcpp)
 
Sys.setenv("PKG_CXXFLAGS"="-std=c++11")
sourceCpp("src/dsInt.cpp")

#' Create a data source with passed data frame
#'
#' Create a data source with passed data frame.
#'
#' @param inDataFrame Name of data frame
#'
#' @return None
#' @export
#'
#' @examples
#' # Create a data source and with built in iris data frame.
#' \donttest{dsCreateWithDataFrame(inDataFrame = iris)}
dsCreateWithDataFrame <- function (inDataFrame) {
    columnTypes <- sapply(inDataFrame, typeof)
    columnNames <- colnames(inDataFrame)
  
    dsCreate(columnTypes, columnNames)
    for(i in 1:nrow(inDataFrame)) {
        values <- sapply(inDataFrame[i,], as.character)
        dsAddValueRow(values);
    }
}
