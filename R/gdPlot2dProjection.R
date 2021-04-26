# Copyright 2021 Werner Mueller
# Released under the GPL (>= 2)

library(tensorflow)
library(Rcpp)

Sys.setenv("PKG_CXXFLAGS"="-std=c++11")
sourceCpp("src/gdInt.cpp")

#' Read generative data and data source
#'
#' Read generative data and data source from corresponding files. Read in generative data and data source are accessed in gdPlot2dProjection(),
#' generative data in gdGetRow().
#'
#' @param inGenerativeDataFileName Name of generative data file
#' @param inDataSourceFileName Name of data source file
#'
#' @return None
#' @export
#'
#' @examples
#' \donttest{gdRead("gd.bin", "iris4d.bin")}
gdRead <- function(inGenerativeDataFileName, inDataSourceFileName = "") {
  gdReset()
  
  gdGenerativeDataRead(inGenerativeDataFileName)
  if(inDataSourceFileName != "") {
    gdDataSourceRead(inDataSourceFileName)
  }
}

#' Create an image file for generative data and data source
#'
#' Create an image file containing a two-dimensional projection for randomly selected rows in generative data and
#' data source. Data points of data source are drawn above data points of generative data.
#'
#' @param outImageFileName Name of image file
#' @param columnIndices Vector of two column indices that are used for the two-dimensional projection. The indices refer to indices of active columns of data source.
#' @param numberOfRandomGdPoints Number of randomly selected rows in generative data
#' @param numberOfRandomDsPoints Number of randomly selected rows in data source 
#'
#' @return None
#' @export
#' 
#' @examples
#' \donttest{gdRead("gd.bin", "iris4d.bin")
#' gdPlot2dProjection("gd34d.png", c(3, 4), 55000, 2500)
#' gdPlot2dProjection("gd12d.png", c(1, 2), 55000, 2500)}
gdPlot2dProjection <- function(outImageFileName, columnIndices, numberOfRandomGdPoints, numberOfRandomDsPoints) {
  if(length( columnIndices) != 2) {
    message("length of vector of column indices must be two\n")
    return()
  }
  
  batchSize <- gdGetBatchSize()
  dataDimension <- gdGetGenerativeDataDimension()
  if(dataDimension < 2) {
    message("dimension of generative data must be greater or equal to two\n")
    return()
  }

  numberVectorIndexNames <- gdGetColumnNames(columnIndices)
  minX = gdGetMin(columnIndices[1])
  maxX = gdGetMax(columnIndices[1])
  minY = gdGetMin(columnIndices[2])
  maxY = gdGetMax(columnIndices[2])
  
  pngfileName <- outImageFileName
  png(pngfileName, width = 2000, height = 2000, units = "px")
  
  title <- ""
  if(gdGetDataSourceFileName() != "") {
    title <- paste("gdPlot2dProjection, ", gdGetGenerativeDataFileName(), ", ", gdGetDataSourceFileName(), sep = "")
  } else {
    title <- paste("gdPlot2dProjection, ", gdGetGenerativeDataFileName(), sep = "")
  }
  
  sessionPar <- par(no.readonly = TRUE)
  on.exit(par(sessionPar))
  par(mar = c(6, 6, 6, 6))
  
  plot(c(), c(), pch = 1, main = title, cex.main = 2.5, font.main = 1, xlim = c(minX, maxX), ylim = c(minY, maxY), col = "blue", xlab = numberVectorIndexNames[1], ylab = numberVectorIndexNames[2], cex.lab = 2.5, cex.axis = 2.5)

  i <- 0
  while(numberOfRandomGdPoints > 0 && i <= numberOfRandomGdPoints) {
    gd <- gdGenerativeDataGetDenormalizedDataRandom(batchSize)
    gd <- array_reshape(gd, c(batchSize, dataDimension))
    points(gd[,columnIndices[1]], gd[,columnIndices[2]], pch = 1, col = "red")
    
    i <- i + batchSize
  }
  
  if(gdGetDataSourceFileName() != "") {
    i <- 0
    while(numberOfRandomDsPoints > 0 && i <= numberOfRandomDsPoints) {
      ds <- gdDataSourceGetDataRandom(batchSize)
      ds <- array_reshape(ds, c(batchSize, dataDimension))
      points(ds[,columnIndices[1]], ds[,columnIndices[2]], pch = 1, col = "blue", cex = 1.1)
    
      i <- i + batchSize
    }
    
    legend("topleft", legend = c("generative data", "data source"), col = c("red", "blue"), pch = c(1, 1), bty = "n", horiz = TRUE, cex = 2.5)
  } else {
    legend("topleft", legend = c("generative data"), col = c("red"), pch = c(1), bty = "n", horiz = TRUE, cex = 2.5)
  }
  
  dev.off()
}
