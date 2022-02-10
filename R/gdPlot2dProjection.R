# Copyright 2021 Werner Mueller
# Released under the GPL (>= 2)

library(tensorflow)
library(Rcpp)

Sys.setenv("PKG_CXXFLAGS"="-std=c++11")
sourceCpp("src/gdInt.cpp")

#' Read generative data and data source
#'
#' Read generative data and data source from specified files. Read in generative data and data source are accessed in gdPlot2dProjection(),
#' generative data is accessed in gdGetRow(), gdCalculateDensityValue() and gdCalculateDensityValueQuantile().
#'
#' @param generativeDataFileName Name of generative data file
#' @param dataSourceFileName Name of data source file
#'
#' @return None
#' @export
#'
#' @examples
#' \dontrun{gdRead("gd.bin", "iris4d.bin")}
gdRead <- function(generativeDataFileName, dataSourceFileName = "") {
  gdReset()
  
  gdGenerativeDataRead(generativeDataFileName)
  if(dataSourceFileName != "") {
    gdDataSourceRead(dataSourceFileName)
  }
}

gdPlot <- function(title, dataDimension, columnIndices) {
  numberVectorIndexNames <- gdGetNumberVectorIndexNames(columnIndices)
  minX <- gdGetMin(columnIndices[1])
  maxX <- gdGetMax(columnIndices[1])
  minY <- NULL
  maxY <- NULL
  if(dataDimension == 1) {
    minY <- 0
    maxY <- 1
  } else {
    minY <- gdGetMin(columnIndices[2])
    maxY <- gdGetMax(columnIndices[2])
  }
  
  plot(c(), c(), pch = 1, main = title, cex.main = 2.5, font.main = 1, xlim = c(minX, maxX), ylim = c(minY, maxY), col = "blue", xlab = numberVectorIndexNames[1], ylab = numberVectorIndexNames[2], cex.lab = 2.5, cex.axis = 2.5)
}

gdGenerativeDatadataPoints <- function(numberOfRandomGdPoints, batchSize, dataDimension, columnIndices, niveau, gdColor, greaterEqual, withDensityValues) {
  i <- 0
  while(numberOfRandomGdPoints > 0 && i <= numberOfRandomGdPoints) {
    if(withDensityValues) {
      gd <- gdGenerativeDataGetDenormalizedDataRandomWithDensities(batchSize)
      gdv <- array_reshape(gd[1], c(batchSize, dataDimension))
      gdd <- array_reshape(gd[2], c(batchSize))
      gdvX <- gdv[,columnIndices[1]]
      gdvY <- NULL
      if(dataDimension == 1) {
        gdvY <- array(0, c(batchSize))
      } else {
        gdvY <- gdv[,columnIndices[2]]
      }
      
      if(greaterEqual) {
        points(gdvX, gdvY, pch = 1, col = ifelse(gdd >= niveau, gdColor, rgb(0, 0, 0, alpha = 0.0)), cex = 1.0)
      } else {
        points(gdvX, gdvY, pch = 1, col = ifelse(gdd < niveau, gdColor, rgb(0, 0, 0, alpha = 0.0)), cex = 1.0)
      }
    } else {
      gd <- gdGenerativeDataGetDenormalizedDataRandom(batchSize)
      gd <- array_reshape(gd, c(batchSize, dataDimension))
      gdX <- gd[,columnIndices[1]]
      gdY <- NULL
      if(dataDimension == 1) {
        gdY <- array(0, c(batchSize))
      } else {
        gdY <- gd[,columnIndices[2]]
      }
      points(gdX, gdY, pch = 1, col = gdColor, cex = 1.0)
    }
  
    i <- i + batchSize
  }
}

gdDataSourcePoints <- function(numberOfRandomPoints, batchSize, dataDimension, columnIndices, dsColor) {
  if(numberOfRandomPoints == 0) {
    return()
  }
  
  if(gdGetDataSourceFileName() != "") {
    i <- 0
    while(numberOfRandomPoints > 0 && i <= numberOfRandomPoints) {
      ds <- gdDataSourceGetDataRandom(batchSize)
      ds <- array_reshape(ds, c(batchSize, dataDimension))
      dsX <- ds[,columnIndices[1]]
      dsY <- NULL
      if(dataDimension == 1) {
        dsY <- array(0, c(batchSize))
      } else {
        dsY <- ds[,columnIndices[2]]
      }
      points(dsX, dsY, pch = 1, col = dsColor, cex = 1.1)
    
      i <- i + batchSize
    }
  }
}

gdLegend <- function(densityValues, colors, dsColor, intervalGenerativeDataParameters) {
  if(length(densityValues) > 0) {
    gdText <- c(paste("generative data, density value <", densityValues[1]))
    pchv <- c(1)
    for(i in 1:length(densityValues)) {
      gdText <- append(gdText, paste("generative data, density value >=", densityValues[i]))
      pchv <- append(pchv, 1)
    }
    
    if(gdGetDataSourceFileName() != "") {
      gdText <- append(gdText, "data source")
      colors <- append(colors, dsColor)
      pchv <- append(pchv, 1)
    } 
    legend("topleft", legend = gdText, col = colors, pch = pchv, bty = "n", horiz = FALSE, cex = 2.5)
  } else {
    gdText <- c("generative data")
    pchv <- c(1)
    
    if(gdGetDataSourceFileName() != "") {
      gdText <- append(gdText, "data source")
      colors <- append(colors, dsColor)
      pchv <- append(pchv, 1)
    }
    legend("topleft", legend = gdText, col = colors, pch = pchv, bty = "n", horiz = FALSE, cex = 2.5)
  }
}

gdPng <- function(outImageFileName, title, columnIndices, generativeDataParameters, dataSourceParameters, batchSize, dataDimension) {
  png(outImageFileName, width = 2000, height = 2000, units = "px")
  
  sessionPar <- par(no.readonly = TRUE)
  on.exit(par(sessionPar))
  par(mar = c(6, 6, 6, 6))
  
  gdPlot(title, dataDimension, columnIndices)
  if(length(generativeDataParameters[[2]]) > 0) {
    gdGenerativeDatadataPoints(generativeDataParameters[[1]], batchSize, dataDimension, columnIndices, 0, generativeDataParameters[[3]][1], TRUE, TRUE)
    for(i in 1:length(generativeDataParameters[[2]])) {
      gdGenerativeDatadataPoints(generativeDataParameters[[1]], batchSize, dataDimension, columnIndices, generativeDataParameters[[2]][i], generativeDataParameters[[3]][i+1], TRUE, TRUE)
    }
    gdDataSourcePoints(dataSourceParameters[[1]], batchSize, dataDimension, columnIndices, dataSourceParameters[[2]])
  } else {
    gdGenerativeDatadataPoints(generativeDataParameters[[1]], batchSize, dataDimension, columnIndices, NULL, generativeDataParameters[[3]][1], FALSE, FALSE)
    gdDataSourcePoints(dataSourceParameters[[1]], batchSize, dataDimension, columnIndices, dataSourceParameters[[2]])
  }
  gdLegend(generativeDataParameters[[2]], generativeDataParameters[[3]], dataSourceParameters[[2]])
  
  dev.off()
}

#' Specify plot parameters for generative data
#' 
#' Specify plot parameters for generative data passed to function gdPlot2dProjection().
#' When density value thresholds with assigned colors are specified generative data is drawn for density value ranges in increasing order.
#'
#' @param numberOfRandomPoints Number of randomly selected rows in generative data
#' @param densityValueThresholds Vector of density value thresholds
#' @param densityValueColors Vector of colors assigned to density value thresholds. The size must be the size
#' of densityValueThresholds plus one.
#' 
#' @examples
#' \dontrun{gdPlotParameters(250000, c(0.75), c("red", "green"))}
gdPlotParameters <- function(numberOfRandomPoints = 0, densityValueThresholds = c(), densityValueColors = c("red")) {
  parameters <- list(numberOfRandomPoints = numberOfRandomPoints, densityValueThresholds = densityValueThresholds, densityValueColors = densityValueColors)
}

#' Specify plot parameters for data source
#' 
#' Specify plot parameters for data source passed to function gdPlot2dProjection().
#'
#' @param numberOfRandomPoints Number of randomly selected rows in data source
#' @param color Colour for data points of data source
#' 
#' @examples
#' \dontrun{gdPlotDataSourceParameters(2500)}
gdPlotDataSourceParameters <- function(numberOfRandomPoints = 0, color = "blue") {
  parameters <- list(numberOfRandomPoints = numberOfRandomPoints, color = color)
}

#' Create an image file for generative data and data source
#'
#' Create an image file containing two-dimensional projections of generative data and data source.
#' Plot parameters for generative data and data source are passed by functions gdPlotParameters() and
#' gdPlotDataSourceParameters(). Data points of data source are drawn above data points of generative data.
#'
#' @param imageFileName Name of image file
#' @param title Title of image
#' @param columnIndices Vector of two column indices that are used for the two-dimensional projections. Indices refer to indices of active columns of data source.
#' @param generativeDataParameters Plot generative data parameters, see function gdPlotParameters().
#' @param dataSourceParameters Plot data source parameters, see function gdPlotDataSourceParameters().
#'
#' @return None
#' @export
#' 
#' @examples
#' \dontrun{gdRead("gd.bin", "iris4d.bin")
#' gdPlot2dProjection("gd12ddv.png",
#'  "Generative Data with a Density Value Threshold for the Iris Dataset", c(1, 2),
#' gdPlotParameters(250000, c(0.71), c("red", "green")),
#' gdPlotDataSourceParameters(2500))
#' gdPlot2dProjection("gd34ddv.png",
#' "Generative Data with a Density Value Threshold for the Iris Dataset", c(3, 4),
#' gdPlotParameters(250000, c(0.71), c("red", "green")),
#' gdPlotDataSourceParameters(2500))}
gdPlot2dProjection <- function(imageFileName, title, columnIndices, 
  generativeDataParameters = gdPlotParameters(numberOfRandomPoints = 0, densityValueThresholds = c(), densityValueColors = c("red")), 
  dataSourceParameters = gdPlotDataSourceParameters(numberOfRandomPoints = 0, color = "blue")) {
  batchSize <- gdGetBatchSize()
  dataDimension <- gdGetGenerativeDataDimension()
  
  if(dataDimension > 1 && length(columnIndices) != 2) {
    message("size of vector columnIndices must be equal to two\n")
    return()
  } else if (dataDimension == 1 && length(columnIndices) != 1) {
    message("size of vector columnIndices must be equal to one\n")
    return()
  }
  if((length(generativeDataParameters[[2]]) + 1) != length(generativeDataParameters[[3]])) {
    message("size of vector gdDensityValueColors must be equal to size of vector gdDensityValues plus one\n")
    return()
  }

  gdPng(imageFileName, title, columnIndices, generativeDataParameters, dataSourceParameters, batchSize, dataDimension)  
}

gdPlot2dProjectionSequence <- function(imageFileName, title, columnIndices, densityValues, numberOfRandomGdPoints, numberOfRandomDsPoints) {
  for(densityValue in densityValues) {
    densityValueImageFileName <- gdBuildFileName(imageFileName, densityValue)
    gdPlot2dProjection(densityValueImageFileName,
                       title,
                       columnIndices,
                       gdPlotParameters(numberOfRandomGdPoints, c(densityValue), c("red", "green")),
                       gdPlotDataSourceParameters(numberOfRandomDsPoints))
  }
}
