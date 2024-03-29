% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/RcppExports.R
\name{dsDeactivateColumns}
\alias{dsDeactivateColumns}
\title{Deactivate columns}
\usage{
dsDeactivateColumns(columnVector)
}
\arguments{
\item{columnVector}{Vector of column indices}
}
\value{
None
}
\description{
Deactivate columns of a data source in order to exclude them in generation of generative data.
Note that in this version only columns of type R-class numeric and R-type double can be used in generaton of generative data.
All columns of other type have to be deactivated.
}
\examples{
\donttest{
dsCreateWithDataFrame(iris)
dsDeactivateColumns(c(5))
dsGetInactiveColumnNames()}
}
