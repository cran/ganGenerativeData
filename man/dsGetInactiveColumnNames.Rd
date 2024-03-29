% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/RcppExports.R
\name{dsGetInactiveColumnNames}
\alias{dsGetInactiveColumnNames}
\title{Get inactive column names}
\usage{
dsGetInactiveColumnNames()
}
\value{
Vector of names of inactive columns
}
\description{
Get inactive column names of a data source
}
\examples{
\donttest{
dsCreateWithDataFrame(iris)
dsDeactivateColumns(c(5))
dsGetInactiveColumnNames()}
}
