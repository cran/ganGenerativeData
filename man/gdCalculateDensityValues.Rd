% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/gdCalculateDensitiyValues.R
\name{gdCalculateDensityValues}
\alias{gdCalculateDensityValues}
\title{Calculate density values for generative data}
\usage{
gdCalculateDensityValues(generativeDataFileName)
}
\arguments{
\item{generativeDataFileName}{Name of generative data file name}
}
\value{
None
}
\description{
Read generative data from a file, calculate density values and write generative data with density values to original file.
Calculated density values are used to classiy generative data.
In function gdPlotParameters() density value thresholds with assigned colors can be passed
to draw generative data for different density value ranges.
}
\examples{
\dontrun{
gdCalculateDensityValues("gd.bin")}
}
