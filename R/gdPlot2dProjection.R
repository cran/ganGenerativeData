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
#' \donttest{gdRead("gd.bin", "iris4d.bin")}
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
  
  #plot(c(), c(), pch = 1, main = title, cex.main = 1.0, font.main = 1, xlim = c(minX, maxX), ylim = c(minY, maxY), col = "blue", xlab = numberVectorIndexNames[1], ylab = numberVectorIndexNames[2], cex.lab = 1.0, cex.axis = 1.0)
  plot(c(), c(), pch = 1, main = title, cex.main = 2.5, font.main = 1, xlim = c(minX, maxX), ylim = c(minY, maxY), col = "blue", xlab = numberVectorIndexNames[1], ylab = numberVectorIndexNames[2], cex.lab = 2.5, cex.axis = 2.5)
}

gdGenerativeDatadataPoints <- function(numberOfRandomGdPoints, batchSize, dataDimension, columnIndices, niveau, gdColour, greaterEqual, withDensityValues) {
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
        points(gdvX, gdvY, pch = 1, col = ifelse(gdd >= niveau, gdColour, rgb(0, 0, 0, alpha = 0.0)), cex = 1.0)
      } else {
        points(gdvX, gdvY, pch = 1, col = ifelse(gdd < niveau, gdColour, rgb(0, 0, 0, alpha = 0.0)), cex = 1.0)
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
      points(gdX, gdY, pch = 1, col = gdColour, cex = 1.0)
    }
  
    i <- i + batchSize
  }
}

gdDataSourcePoints <- function(numberOfRandomDsPoints, batchSize, dataDimension, columnIndices, dsColour) {
  if(gdGetDataSourceFileName() != "") {
    i <- 0
    while(numberOfRandomDsPoints > 0 && i <= numberOfRandomDsPoints) {
      ds <- gdDataSourceGetDataRandom(batchSize)
      ds <- array_reshape(ds, c(batchSize, dataDimension))
      dsX <- ds[,columnIndices[1]]
      dsY <- NULL
      if(dataDimension == 1) {
        dsY <- array(0, c(batchSize))
      } else {
        dsY <- ds[,columnIndices[2]]
      }
      points(dsX, dsY, pch = 1, col = dsColour, cex = 1.1)
      #points(dsX, dsY, pch = 1, col = dsColour, cex = 2.0)
    
      i <- i + batchSize
    }
  }
}

gdLegend <- function(densityValues, colours, dsColour) {
  if(length(densityValues) > 0) {
    gdText <- c(paste("generative data, density value <", densityValues[1]))
    pchv <- c(1)
    for(i in 1:length(densityValues)) {
      gdText <- append(gdText, paste("generative data, density value >=", densityValues[i]))
      pchv <- append(pchv, 1)
    }
    
    if(gdGetDataSourceFileName() != "") {
      gdText <- append(gdText, "data source")
      colours <- append(colours, dsColour)
      pchv <- append(pchv, 1)
      
      #legend("topleft", legend = gdText, col = colours, pch = pchv, bty = "n", horiz = FALSE, cex = 1.0)
      legend("topleft", legend = gdText, col = colours, pch = pchv, bty = "n", horiz = FALSE, cex = 2.5)
    } else {
      #legend("topleft", legend = gdText,  col = colours, pch = pchv, bty = "n", horiz = FALSE, cex = 1.0)
      legend("topleft", legend = gdText,  col = colours, pch = pchv, bty = "n", horiz = FALSE, cex = 2.5)
    }
  } else {
    gdText <- c("generative data")
    pchv <- c(1)
    if(gdGetDataSourceFileName() != "") {
      gdText <- append(gdText, "data source")
      colours <- append(colours, dsColour)
      pchv <- append(pchv, 1)
      #legend("topleft", legend = gdText, col = colours, pch = pchv, bty = "n", horiz = FALSE, cex = 1.0)
      legend("topleft", legend = gdText, col = colours, pch = pchv, bty = "n", horiz = FALSE, cex = 2.5)
    } else {
      #legend("topleft", legend = gdText, col = colours, pch = pchv, bty = "n", horiz = FALSE, cex = 1.0)
      legend("topleft", legend = gdText, col = colours, pch = pchv, bty = "n", horiz = FALSE, cex = 2.5)
    } 
  }
}

gdPng <- function(outImageFileName, title, columnIndices, numberOfRandomGdPoints, numberOfRandomDsPoints, batchSize, dataDimension, gdDensityValues, gdColours, dsColour) {
  png(outImageFileName, width = 2000, height = 2000, units = "px")
  #png(outImageFileName, width = 500, height = 500, units = "px")
  
  sessionPar <- par(no.readonly = TRUE)
  on.exit(par(sessionPar))
  par(mar = c(6, 6, 6, 6))
  
  gdPlot(title, dataDimension, columnIndices)
  if(length(gdDensityValues) > 0) {
    gdGenerativeDatadataPoints(numberOfRandomGdPoints, batchSize, dataDimension, columnIndices, gdDensityValues[1], gdColours[1], FALSE, TRUE)
    for(i in 1:length(gdDensityValues)) {
      gdGenerativeDatadataPoints(numberOfRandomGdPoints, batchSize, dataDimension, columnIndices, gdDensityValues[i], gdColours[i+1], TRUE, TRUE)
    }
    gdDataSourcePoints(numberOfRandomDsPoints, batchSize, dataDimension, columnIndices, dsColour)
  } else {
    gdGenerativeDatadataPoints(numberOfRandomGdPoints, batchSize, dataDimension, columnIndices, NULL, gdColours[1], FALSE, FALSE)
    gdDataSourcePoints(numberOfRandomDsPoints, batchSize, dataDimension, columnIndices, dsColour)
  }
  gdLegend(gdDensityValues, gdColours, dsColour)

  dev.off()
}

#' Create an image file for generative data and data source
#'
#' Create an image file containing two-dimensional projections for a number of randomly selected rows in generative data and
#' data source. Data points of data source are drawn above data points of generative data.
#' When density value thresholds with assigned colours are specified generative data is drawn for density value ranges in increasing order.
#'
#' @param imageFileName Name of image file
#' @param title Title of image
#' @param columnIndices Vector of two column indices that are used for the two-dimensional projections. Indices refer to indices of active columns of data source.
#' @param numberOfRandomGdPoints Number of randomly selected rows in generative data
#' @param numberOfRandomDsPoints Number of randomly selected rows in data source 
#' @param densityValueThresholds Vector of density value thresholds
#' @param densityValueColours Vector of colours assigned to density value thresholds. The size must be the size of densityValueThresholds plus one.
#' @param dsColour Colour for data points of data source
#'
#' @return None
#' @export
#' 
#' @examples
#' \donttest{gdRead("gd.bin", "iris4d.bin")
#' gdPlot2dProjection("gd34d.png", c(3, 4), "Generative Data for the Iris Dataset", 250000, 2500)
#' gdPlot2dProjection("gd34ddv.png",
#' "Generative Data with a Density Value Threshold for the Iris Dataset",
#' c(3, 4), 250000, 2500, c(0.04), c("red", "green"))}
gdPlot2dProjection <- function(imageFileName, title, columnIndices, numberOfRandomGdPoints, numberOfRandomDsPoints, densityValueThresholds = c(), densityValueColours = c("red"), dsColour = "blue") {
  batchSize <- gdGetBatchSize()
  dataDimension <- gdGetGenerativeDataDimension()
 
  if(dataDimension > 1 && length(columnIndices) != 2) {
    message("size of vector columnIndices must be equal to two\n")
    return()
  } else if (dataDimension == 1 && length(columnIndices) != 1) {
    message("size of vector columnIndices must be equal to one\n")
    return()
  }
  if((length(densityValueThresholds) + 1) != length(densityValueColours)) {
    message("size of vector gdDensityValueColours must be equal to size of vector gdDensityValues plus one\n")
    return()
  }
  
  gdPng(imageFileName, title, columnIndices, numberOfRandomGdPoints, numberOfRandomDsPoints, batchSize, dataDimension, densityValueThresholds, densityValueColours, dsColour)
}

