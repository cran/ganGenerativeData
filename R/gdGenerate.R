# Copyright 2021 Werner Mueller
# Released under the GPL (>= 2)

library(tensorflow)
library(Rcpp)

Sys.setenv("PKG_CXXFLAGS"="-std=c++11")
sourceCpp("src/gdInt.cpp")

generate <- function(outGenerativeDataFileName, numberOfIterations, inKeepProbability, columnIndices) {
  gdTf <- tf$compat$v1
  gdTf$disable_v2_behavior()
  
  cNumberOfBatchesPerEpoch <- 10
  cWriteEpochsModulo <- 250
  cInitEpochs <- -100

  batchSize <- gdGetBatchSize();
  dataDimension <- gdGetDataSourceDimension()
  
  if(dataDimension > 1 && length(columnIndices) != 2) {
    message("size of vector columnIndices must be equal to two\n")
    return()
  } else if (dataDimension == 1 && length(columnIndices) != 1) {
    message("size of vector columnIndices must be equal to one\n")
    return()
  }
  
  columnNames <- gdGetNumberVectorIndexNames(columnIndices)
  
  cOutputLayerSize <- gdGetDataSourceDimension()
  cLr <- 0.00005
  cLrg <- 0.00005

  gdTf$reset_default_graph()

  x <- gdTf$placeholder(gdTf$float32, shape = c(batchSize, cOutputLayerSize))
  noiseInput <- gdTf$placeholder(gdTf$float32, shape = c(batchSize, cOutputLayerSize))
  keepProbability <- gdTf$placeholder(gdTf$float32)

  cGeneratorHiddenLayerSizes <- c(1024, 1024)
  cDiscriminatorHiddenLayerSizes <- c(1024, 1024)

  generator <- function(noiseInput, outputLayerSize, hiddenLayerSizes = cGeneratorHiddenLayerSizes, reuse = gdTf$AUTO_REUSE) {
    with (gdTf$variable_scope('Gan/Generator', reuse = reuse), {
      hiddenLayer1 <- gdTf$layers$dense(inputs = noiseInput, units = hiddenLayerSizes[1], activation = gdTf$nn$leaky_relu)
      hiddenLayer1Dropout <- gdTf$nn$dropout(hiddenLayer1, keepProbability)
      hiddenLayer2 <- gdTf$layers$dense(inputs = hiddenLayer1Dropout, units = hiddenLayerSizes[2], activation = gdTf$nn$leaky_relu)
      hiddenLayer2Dropout <- gdTf$nn$dropout(hiddenLayer2, keepProbability)
      logits <- gdTf$layers$dense(inputs = hiddenLayer2Dropout, units = outputLayerSize)
    })
    logits
  }

  discriminator <- function(input, hiddenLayerSizes = cDiscriminatorHiddenLayerSizes, reuse = gdTf$AUTO_REUSE) {
    with (gdTf$variable_scope('Gan/Discriminator', reuse = reuse), {
      hiddenLayer1 <- gdTf$layers$dense(inputs = input, units = hiddenLayerSizes[1], activation = gdTf$nn$leaky_relu)
      hiddenLayer1Dropout <- gdTf$nn$dropout(hiddenLayer1, keepProbability)
      hiddenLayer2 <- gdTf$layers$dense(inputs = hiddenLayer1Dropout, units = hiddenLayerSizes[2], activation = gdTf$nn$leaky_relu)
      hiddenLayer2Dropout <- gdTf$nn$dropout(hiddenLayer2, keepProbability)
      logits <- gdTf$layers$dense(inputs = hiddenLayer2Dropout, units = 1)
    })
    logits
  }

  gFake <- generator(noiseInput, cOutputLayerSize)
  dLogitsReal <- discriminator(x, reuse = gdTf$AUTO_REUSE)
  dLogitsFake <- discriminator(gFake, reuse = TRUE)

  lossFunc <- function(logitsIn, labelsIn) {
    r <- gdTf$reduce_mean(gdTf$nn$sigmoid_cross_entropy_with_logits(logits = logitsIn, labels = labelsIn))
  }

  dRealLoss <- lossFunc(dLogitsReal, gdTf$ones_like(dLogitsReal))
  dFakeLoss <- lossFunc(dLogitsFake, gdTf$zeros_like(dLogitsFake))
  dLoss <- dRealLoss + dFakeLoss

  pred_func <- function(logits_in,labels_in) {
    predicted <- gdTf$nn$sigmoid(logits_in)
    correct_predicted <- gdTf$equal(gdTf$round(predicted), labels_in)
    predicted
  }
  D_pred <- pred_func(dLogitsFake, gdTf$zeros_like(dLogitsReal))

  dVars = gdTf$get_collection(gdTf$GraphKeys$GLOBAL_VARIABLES, scope="Gan/Discriminator")
  gVars = gdTf$get_collection(gdTf$GraphKeys$GLOBAL_VARIABLES, scope="Gan/Generator")

  dTrainer <- gdTf$train$RMSPropOptimizer(cLr)$minimize(dLoss, var_list = dVars)
  gTrainer <- gdTf$train$RMSPropOptimizer(cLrg)$minimize(-dLoss, var_list = gVars)

  epochs <- numberOfIterations
  init <- gdTf$global_variables_initializer()
  session <- gdTf$Session()
  session$run(init)

  gGenerator <- generator(noiseInput, outputLayerSize = cOutputLayerSize, reuse=TRUE)

  epoch <- 1
  for(epoch in cInitEpochs:epochs) {

    samples <- NULL
    i <- 1
    for(i in 1:cNumberOfBatchesPerEpoch) {
      samples <- gdDataSourceGetNormalizedDataRandom(batchSize)
      samples <- array_reshape(samples, c(batchSize, dataDimension))
      noise <- array_reshape(runif(batchSize * dataDimension, -1.0, 1.0), c(batchSize, dataDimension))

      if(epoch < 1) {
        samples <- array_reshape(runif(batchSize * dataDimension, 0.0, 1.0), c(batchSize, dataDimension))
      }

      session$run(dTrainer, feed_dict = dict(x = samples, noiseInput = noise, keepProbability = inKeepProbability))
      session$run(gTrainer, feed_dict= dict(noiseInput = noise, keepProbability = inKeepProbability))
    }
    message(epoch)

    noise <- array_reshape(runif(batchSize * dataDimension, -1.0, 1.0), c(batchSize, dataDimension))

    r <- session$run(list(D_pred, gGenerator), feed_dict = dict(x = samples, noiseInput = noise, keepProbability = inKeepProbability))
    gs <- r[[2]]
    gp <- array(1, c(batchSize))
    gp <- array_reshape(r[[1]], c(batchSize))
    k <- 0
    for(j in 1:batchSize) {
      if(gp[j] >= 0.5) {
        k <- k + 1
      }
    }
    gsp <- array(0, c(k, dataDimension))
    l <- 1
    for(j in 1:batchSize) {
      if(gp[j] >= 0.5) {
        gsp[l,] = gs[j,]
        l <- l + 1
      }
    }
    gsp <- array_reshape(gsp, c(k * dataDimension))
    
    if(epoch >= 1) {
      gdAddValueRows(gsp)
    }
    if(dataDimension == 1) {
      plot(gs[, columnIndices[1]], array(0, batchSize), main = "gdGenerate", cex.main = 1.0, font.main = 1, xlim = c(0.0, 1.0), ylim = c(0.0, 1.0), col = ifelse(gp >= 0.5, "green", "red"), xlab = columnNames[1], ylab = columnNames[2])
    } else {
      plot(gs[, columnIndices[1]], gs[, columnIndices[2]], main = "gdGenerate", cex.main = 1.0, font.main = 1, xlim = c(0.0, 1.0), ylim = c(0.0, 1.0), col = ifelse(gp >= 0.5, "green", "red"), xlab = columnNames[1], ylab = columnNames[2])      
    }
    legend("topleft", legend = c("gd positive", "gd negative", "ds"), col = c("green", "red", "blue"), pch = c(1, 1, 1), bty = "n", horiz = TRUE)
    if(dataDimension == 1) {
      points(samples[, columnIndices[1]], array(0, batchSize), col = "blue")
    } else {
      points(samples[, columnIndices[1]], samples[, columnIndices[2]], col = "blue")
    }
    
    if(epoch >= 1) {
      if(epoch %% cWriteEpochsModulo == 0) {
        gdGenerativeDataWrite(outGenerativeDataFileName)
      }
    }
  }

  gdGenerativeDataWrite(outGenerativeDataFileName)
}

#' Generate generative data for a data source
#' 
#' Read a data source from a file, generate generative data for the data source in iterative training steps and
#' write generated data to a file in binary format. When a higher number of iterations is used 
#' the distribution of generated data gets closer to that of the data source.
#'
#' @param dataSourceFileName Name of data source file
#' @param generativeDataFileName Name of generative data file
#' @param numberOfIterations Number of iterations.
#' In this version the limit of number of iterations is set to 50000.
#' @param keepProbability Value in the range of 0 to 1 which is used in training of neural networks to train generalized networks.
#' @param columnIndices Vector of two column indices that are used to plot two-dimensional projections of normalized generated generative data and
#' data source for a training step. Indices refer to indices of active columns of data source. 
#'
#' @return None
#' @export
#'
#' @examples
#' \dontrun{gdGenerate("iris4d.bin", "gd.bin", 2500, 0.95, c(1, 2))}
gdGenerate <- function(dataSourceFileName, generativeDataFileName, numberOfIterations, keepProbability, columnIndices) {
  if(numberOfIterations > gdGetMaxSize() / gdGetBatchSize()) {
    message("Max number of iterations exceeded")
    return()
  }
  gdReset()
  
  gdDataSourceRead(dataSourceFileName)
  gdCreateGenerativeData()
  generate(generativeDataFileName, numberOfIterations, keepProbability, columnIndices)
}
