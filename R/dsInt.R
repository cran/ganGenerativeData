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
