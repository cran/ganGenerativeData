\name{ganGenerativeData-package}
\alias{ganGenerativeData-package}
\alias{ganGenerativeData}
\docType{package}
\title{
  Generate generative data for a data source
}
\description{
Generative Adversarial Networks are applied to generate generative data for a data source. In iterative training steps the distribution of generated data converges to that of the data source. Two-dimensional projections of generative data for the iris dataset are shown in the inserted images:\cr
\if{html}{\figure{gd34d.svg}}
\if{latex}{\figure{gd34d.png}}
\cr
\if{html}{\figure{gd12d.svg}}
\if{latex}{\figure{gd12d.png}}
}
\details{
The API includes functions for topics "definition of data source" and "generation of generative data". Main function of first topic is \strong{\code{dsCreateWithDataFrame()}} which creates a data source with passed data frame. Main function of second topic is \strong{\code{gdGenerate()}} which generates generative data for a data source.\cr

\strong{1. Definition of data source}\cr

\strong{\code{dsCreateWithDataFrame()}} Create a data source with passed data frame.\cr

\strong{\code{dsActivateColumns()}} Activate columns of a data source in order to include them in generation of generative data. By default columns are active.\cr

\strong{\code{dsDeactivateColumns()}} Deactivate columns of a data source in order to exclude them in generation of generative data. Note that in this version only columns with values of type double or float can be used in generation of generative data. All columns with values of other type have to be deactivated.\cr

\strong{\code{dsGetActiveColumnNames()}} Get names of active columns of a data source.\cr

\strong{\code{dsGetInactiveColumnNames()}} Get names of inactive columns of a data source.\cr

\strong{\code{dsWrite()}} Write created data source including settings of active columns to a file in binary format. This file will be used as input in functions of topic "generation of generative data".\cr

\strong{\code{dsRead()}} Read a data source from a file that was written with \code{dsWrite()}.\cr

\strong{\code{dsGetNumberOfRows()}} Get number of rows in a data source.\cr

\strong{\code{dsGetRow()}} Get a row in a data source.\cr

\strong{2. Generation of generative data}\cr

\strong{\code{gdGenerate()}} Read a data source from a file, generate generative data for the data source in iterative training steps and write generated generative data to a file in binary format.\cr

\strong{\code{gdRead()}} Read generative data and data source from corresponding files. Read in generative data and data source are accessed in \code{gdPlot2dProjection()}, generative data in \code{gdGetRow()}.\cr

\strong{\code{gdPlot2dProjection()}} Create an image showing a two-dimensional projection of generative data and data source and write it to a file.\cr

\strong{\code{gdGetNumberOfRows()}} Get number of rows in generative data.\cr

\strong{\code{gdGetRow()}} Get a row in generative data.
}
\author{
Werner Mueller

Maintainer: Werner Mueller <werner.mueller5@chello.at>
}
\references{
Ian J. Goodfellow, Jean Pouget-Abadie, Mehdi Mirza, Bing Xu, David Warde-Farley, Sherjil Ozair, Aaron Courville, Yoshua Bengio (2014), \emph{"Generative Adversarial Nets"}, <arXiv:1406.2661v1>
}
\keyword{ package }
\examples{
# Generate generative data for iris dataset

# Load and install library tensorflow
\dontrun{
library(tensorflow)
install_tensorflow()}

# Load library
library(ganGenerativeData)

# 1. Definition of data source for iris dataset

# Create a data source with built in iris dataframe.
dsCreateWithDataFrame(inDataFrame = iris)

# Deactivate column for index 5 in order to exclude it in generation of generative data.
dsDeactivateColumns(c(5))

# Get active columns Sepal.Length, Sepal.Width, Petal.Length, Petal.Width.
dsGetActiveColumnNames()

# Write data source including settings of active columns to file "iris4d.bin" in binary format.
\dontshow{iris4d <- tempfile("iris4d")
dsWrite(iris4d)}\donttest{dsWrite("iris4d.bin")}

# 2. Generation of generative data for iris data source

# Read data source from file "iris4d.bin",
# generate generative data in iterative training steps and
# write generated generative data to file "gd.bin".
\donttest{gdGenerate("iris4d.bin", "gd.bin", 2500, 0.95, c(1, 2))}

# Read generative data from file "gd.bin" and data source from "iris4d.bin"
\donttest{gdRead("gd.bin", "iris4d.bin")}

# Create an image showing a two-dimensional projection of generative data and
# data source for column indices 1, 2 and
# write it to file "gd12d.png"
\donttest{gdPlot2dProjection("gd12d.png", c(1, 2), 55000, 2500)}

# Create an image showing a two-dimensional projection of generative data and
# data source for column indices 3, 4 and
# write it to file "gd34d.png"
\donttest{gdPlot2dProjection("gd34d.png", c(3, 4), 55000, 2500)}

# Get number of rows in generative data
\donttest{gdGetNumberOfRows()}

# Get first row in generative data
\donttest{gdGetRow(1)}
}

