\name{ganGenerativeData-package}
\alias{ganGenerativeData-package}
\alias{ganGenerativeData}
\docType{package}
\title{
  Generate generative data for a data source
}
\description{
Generative Adversarial Networks are applied to generate generative data for a data source. A generative model consisting of a generator and a discriminator network is trained. During iterative training the distribution of generated data is converging to that of the data source. Generated data can be written to a file in training and after finished training in a separate generation step. First method accumulates generative data using a dynamic model, second method generates generative data using a static model.

Inserted images show two-dimensional projections of generative data for the iris dataset:\cr

\figure{gd34d.png}

\figure{gd12d.png}

\figure{gd34ddv.png}

\figure{gd12ddv.png}

\if{latex}{
\strong{Method of missing data completion:}

Without loss of generality we consider the case of two random variables \eqn{X,Y}. Let \eqn{g_{XY}(x,y)} be the continuous probability density function of joint distribution \eqn{X,Y} and \eqn{G=((x_i,y_i))_{i=1}^{n}} a sequence of occurrences of \eqn{X,Y}.

In order to complete missing data for a given \eqn{X=x_0} we consider the conditional probability density function \eqn{g_{Y|X=x_0}(y)}.

We compute the mean of \eqn{g_{XY}(x,y)} over interval \eqn{[x_0-\epsilon,x_0+\epsilon]} and then the limit by applying the rule of de l’Hospital
\deqn{\lim \limits_{\epsilon \to 0} 1/2\epsilon \int \limits_{x_0-\epsilon}^{x_0+\epsilon} g_{XY}(x,y) dx=g_{Y|X=x_0}(y)}
So the mean of \eqn{g_{XY}(x,y)} over interval \eqn{[x_0-\epsilon,x_0+\epsilon]} converges to \eqn{g_{Y|X=x_0}(y)} for \eqn{\epsilon \to 0} and the subsequence of occurrences of \eqn{G} in the interval is given by \eqn{G_{x_0,\epsilon}=((x_{i_j},y_{i_j}))} with \eqn{1<=i_j<=n,x_{i_j} \in[x_0-\epsilon,x_0+\epsilon]}.

A candidate for missing \eqn{Y=y_0} for a given \eqn{X=x_0} is then \eqn{y_k} of a uniform randomly selected \eqn{(x_k,y_k)} from \eqn{G_{x_0,\epsilon}}.

This is applied for generated generative data as follows:

In function \code{gdComplete()} the nearest neighbor for a given incomplete data record is searched with respect to present values and then utilized to complete the incomplete data record. Also function \code{gdKNearestNeighbors()} can be called. A nearest neighbor uniform randomly selected from returned \eqn{k} nearest neighbors can then be used for data record completion.

For illustration inserted images show \eqn{k} nearest neighbors for a sepal width value:\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\figure{gd34dnn.png}

\figure{gd12dnn.png}
}

\if{latex}{
\strong{Accelerated training of generative models:}

Training of generative models can be accelerated by using graphics processing units for computation. For this kind of training a software service is available. For access details contact the author of the package and see in the documentation in section Examples how training can be executed. The software service also supports trainining of generative models for mixed numerical and categorical variables.

\strong{Method of data classification:}

With support of training of generative models for mixed numerical and categorical variables data classification can be done for a given labeled dataset and unlabeled data as follows:

1. Considering class labels as a categorical variable train a generative model for joint distribution of data records and class labels.

2. Apply method of missing data completion to determine class labels for unlabeled data records.

For illustration inserted images show generative data for the iris dataset with data records consisting of numerical variables sepal length, sepal width, petal length, petal width and assigned class labels:
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\cr
\figure{gd34cnn.png}

\figure{gd12cnn.png}
}
}

\details{
The API includes functions for topics "definition of data source" and "generation of generative data". Main function of first topic is \strong{\code{dsCreateWithDataFrame()}} which creates a data source with passed data frame. Main functions of second topic are \strong{\code{gdTrain()}} which trains a generative model for a data source and \strong{\code{gdGenerate()}} which uses a trained generative model to generate generative data. Additionally a software service for accelerated training of generative models is available.\cr

\strong{1. Definition of data source}\cr

\strong{\code{dsCreateWithDataFrame()}} Create a data source with passed data frame.\cr

\strong{\code{dsActivateColumns()}} Activate columns in a data source in order to include them in training of generative models.
By default columns are active.\cr

\strong{\code{dsDeactivateColumns()}} Deactivate columns in a data source in order to exclude them from training of generative models.
Note that the training function in the package supports only columns of type R-class numeric, R-type double.
All columns of other type have to be deactivated.
The training function in the software service for accelerated training of generative models supports columns of any type.\cr

\strong{\code{dsGetActiveColumnNames()}} Get names of active columns of a data source.\cr

\strong{\code{dsGetInactiveColumnNames()}} Get names of inactive columns of a data source.\cr

\strong{\code{dsWrite()}} Write created data source including settings of active columns to a file in binary format. This file will be used as input in functions of topic "generation of generative data".\cr

\strong{\code{dsRead()}} Read a data source from a file that was written with \code{dsWrite()}.\cr

\strong{\code{dsGetNumberOfRows()}} Get number of rows in a data source.\cr

\strong{\code{dsGetRow()}} Get a row in a data source.\cr

\strong{2. Training of generative model and generation of generative data}\cr

\strong{\code{gdTrainParameters()}} Specify parameters for training of generative model.\cr

\strong{\code{gdTrain()}} Read a data source from a file, train a generative model that generates generative data for the data source in iterative training steps, write trained generative model and generated data in training steps to a file in binary format..\cr

\strong{\code{gdGenerateParameters()}} Specify parameters for generation of generative data.\cr

\strong{\code{gdGenerate()}} Read a generative model and a data source from a file, generate generative data for the data source and write generated data to a file in binary format.\cr

\strong{\code{gdCalculateDensityValues()}} Read generative data from a file, calculate density values and write generative data with density values to original file.\cr

\strong{\code{gdRead()}} Read generative data and data source from specified files.\cr

\strong{\code{gdPlotParameters()}} Specify plot parameters for generative data.\cr

\strong{\code{gdPlotDataSourceParameters()}} Specify plot parameters for data source.\cr

\strong{\code{gdPlotProjection()}} Create an image file containing two-dimensional projections of generative data and data source.\cr

\strong{\code{gdGetNumberOfRows()}} Get number of rows in generative data.\cr

\strong{\code{gdGetRow()}} Get a row in generative data.\cr

\strong{\code{gdCalculateDensityValue()}} Calculate density value for a data record.\cr

\strong{\code{gdCalculateDensityValueQuantile()}} Calculate density value quantile for a percent value.\cr

\strong{\code{gdKNearestNeighbors()}} Search for k nearest neighbors in generative data.\cr

\strong{\code{gdComplete()}} Complete incomplete data record.\cr

\strong{\code{gdWriteSubset()}} Write subset of generative data.\cr

\strong{3. Software service for accelerated training of generative models}\cr

\strong{\code{gdServiceTrain()}} Send a request to software service to train a generative model.\cr

\strong{\code{gdServiceGetGenerativeData()}} Get generated generative data from software service.\cr

\strong{\code{gdServiceGetGenerativeModel()}} Get trained generative model from software service.\cr

\strong{\code{gdServiveGetStatus()}} Get status of generated job from software service.\cr

\strong{\code{gdServiceDelete()}} Delete generated job from software service.\cr
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
# Environment used for execution of examples:

# Operating system: Ubuntu 22.04.1
# Compiler: g++ 11.3.0 (supports C++17 standard)
# R applications: R 4.1.2, RStudio 2022.02.2
# Installed packages: 'Rcpp' 1.0.10, 'tensorflow' 2.11.0,
# 'ganGenerativeData' 2.0.0

# Package 'tensorflow' provides an interface to machine learning framework
# TensorFlow. To complete the installation function install_tensorflow() has to
# be called.
\dontrun{
library(tensorflow)
install_tensorflow()}

# Generate generative data for the iris dataset

# Load library
library(ganGenerativeData)

# 1. Definition of data source for the iris dataset

# Create a data source with iris data frame.
dsCreateWithDataFrame(iris)

# Deactivate the column with name Species and index 5 in order to exclude it in 
# trainng of generative model.
dsDeactivateColumns(c(5))

# Get the active column names: Sepal.Length, Sepal.Width, Petal.Length,
# Petal.Width.
dsGetActiveColumnNames()

# Write the data source including settings of active columns to file
# "ds.bin" in binary format.
\dontshow{
ds <- tempfile("ds")
dsWrite(ds)}\dontrun{
dsWrite("ds.bin")}

# 2. Training of generative model and generation of generative data for the iris
# data source

# Read data source from file "ds.bin", train a generative model in iterative
# training steps (used number of iterations in tests is in the range of 10000 to
# 50000), write trained generative model and generated data in training steps to
# files "gm.bin" and "gd.bin".
\dontrun{
gdTrain("gm.bin", "gd.bin", "ds.bin", c(1, 2),
gdTrainParameters(numberOfTrainingIterations = 1000))}

# Read generative data from file "gd.bin", calculate density values and
# write generative data with density values to original file.
\dontrun{
gdCalculateDensityValues("gd.bin")}

# Read generative data from file "gd.bin" and data source from "ds.bin". Read in
# data will be accessed in following function calls.
\dontrun{
gdRead("gd.bin", "ds.bin")}

# Create an image showing two-dimensional projections of generative data and
# data source for column indices 3, 4 and write it to file "gd34d.png".
\dontrun{
gdPlotProjection("gd34d.png",
"Generative Data for the Iris Dataset",
c(3, 4),
gdPlotParameters(50),
gdPlotDataSourceParameters(100))}

# Create an image showing two-dimensional projections of generative data and 
# data source for column indices 3, 4 with density value threshold 0.71 and
# write it to file "gd34ddv.png".
\dontrun{
gdPlotProjection("gd34ddv.png",
"Generative Data with a Density Value Threshold for the Iris Dataset",
c(3, 4),
gdPlotParameters(50, c(0.71), c("red", "green")),
gdPlotDataSourceParameters(100))}

# Get number of rows in generative data
\dontrun{
gdGetNumberOfRows()}

# Get row with index 1000 in generative data
\dontrun{
gdGetRow(1000)}

# Calculate density value for a data record
\dontrun{
gdCalculateDensityValue(list(6.1, 2.6, 5.6, 1.4))}

# Calculate density value quantile for 50 percent
\dontrun{
gdCalculateDensityValueQuantile(50)}

# Search for k nearest neighbors for a data record 
\dontrun{
gdKNearestNeighbors(list(5.1, 3.5, 1.4, 0.2), 3)}

# Complete incomplete data record containing an NA value
\dontrun{
gdComplete(list(5.1, 3.5, 1.4, NA))}

# Write subset containing 50 percent of randomly selected rows of
# generative data
\dontrun{
gdRead("gd.bin")
gdWriteSubset("gds.bin", 50)}

# 3. Usage of software service for accelerated training of a generative
# model

# Initialize variables for URL and access key.
\dontrun{
url <- "http://xxx.xxx.xxx.xxx/gdService"
accessKey <- "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"}

# Send a request to software service to train a generative model for a data
# source. A job id will be returned.
\dontrun{
trainParameters <- gdTrainParameters(numberOfTrainingIterations = 10000,
numberOfInitializationIterations = 2500)
jobId <- gdServiceTrain(url, accessKey, "gmService.bin", "gdService.bin", "ds.bin",
trainParameters)}

# Get status of generated job from software service. When job is processed
# successfully status will be set to TRAINED.
\dontrun{
gdServiceGetStatus(url, accessKey, jobId)}

# Get generated generative data from software service for processed job
\dontrun{
gdServiceGetGenerativeData(url, accessKey, jobId, "gdService.bin")}

# Get trained generative model from software service for processed job
\dontrun{
gdServiceGetGenerativeModel(url, accessKey, jobId, "gmService.bin")}
}