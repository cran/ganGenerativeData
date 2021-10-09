# Copyright 2021 Werner Mueller
# Released under the GPL (>= 2)

library(Rcpp)
 
Sys.setenv("PKG_CXXFLAGS"="-std=c++11")
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
#' # Create a data source and with built in iris data frame.
#' \donttest{dsCreateWithDataFrame(dataFrame = iris)}
dsCreateWithDataFrame <- function (dataFrame) {
    if(!is.data.frame(dataFrame)) {
      message("Passed object is not a data frame\n")
      return()
    }
    columnTypes <- sapply(dataFrame, typeof)
    columnNames <- colnames(dataFrame)
  
    dsCreate(columnTypes, columnNames)
    for(i in 1:nrow(dataFrame)) {
        values <- sapply(dataFrame[i,], as.character)
        dsAddValueRow(values);
    }
}
