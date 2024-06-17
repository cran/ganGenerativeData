# Copyright 2021 Werner Mueller
# Released under the GPL (>= 2)

library(tensorflow)
library(Rcpp)

Sys.setenv("PKG_CXXFLAGS"="-std=c++17")
sourceCpp("src/gdInt.cpp")

utils::globalVariables(c("tape"))

gdTrainGenerate <- function(generativeModelFileName, generativeDataFileName, columnIndices, trainParameters, generateParameters, generativeModelRead) {
    showPlot = TRUE
    if(is.null(columnIndices) || length(columnIndices) == 0) {
        showPlot = FALSE  
    }
        
    batchSize <- gdGetBatchSize();
    dimension <- gdGetDataSourceDimension()
    
    columnNames <- NULL
    if(showPlot) {
        if(dimension > 1 && length(columnIndices) != 2) {
            stop("Size of vector columnIndices must be equal to two\n")
        } else if (dimension == 1 && length(columnIndices) != 1) {
            stop("Size of vector columnIndices must be equal to one\n")
        }
        columnNames <- gdGetNumberVectorIndexNames(columnIndices)
    }
    
    cWriteMessageModulo = 100
    cNumberOfBatchesPerIteration <- 10
    cWriteIterationsModulo <- 1000
    cResetIterations <- 100
    cEpsilon <- 1.0e-10
    cLevel <- 0.5

    numberOfHiddenLayerUnits <- NULL;
    learningRate <- NULL
    dropout <- NULL

    if(!is.null(trainParameters)) {
        if(generativeModelRead) {
            numberOfHiddenLayerUnits = gdGenerativeModelGetNumberOfHiddenLayerUnits()
            learningRate <- gdGenerativeModelGetLearningRate()
            dropout <- gdGenerativeModelGetDropout()
        } else {
            numberOfHiddenLayerUnits = trainParameters$numberOfHiddenLayerUnits
            dropout = trainParameters$dropout
            learningRate <- trainParameters$learningRate
        }
    } else if(!is.null(generateParameters)) {
        if(generativeModelRead) {
            numberOfHiddenLayerUnits = gdGenerativeModelGetNumberOfHiddenLayerUnits()
            learningRate <- gdGenerativeModelGetLearningRate()
            dropout <- generateParameters$dropout
        } else {
            return()
        }
    } else {
        return()
    }

    discriminatorHiddenLayer1 <- tf$keras$layers$Dense(units = numberOfHiddenLayerUnits, activation = tf$nn$leaky_relu)
    discriminatorHiddenLayer2 <- tf$keras$layers$Dense(units = numberOfHiddenLayerUnits, activation = tf$nn$leaky_relu)
    discriminatorLogits <- tf$keras$layers$Dense(units = 1)

    generatorHiddenLayer1 <- tf$keras$layers$Dense(units = numberOfHiddenLayerUnits, activation = tf$nn$leaky_relu)
    generatorHiddenLayer2 <- tf$keras$layers$Dense(units = numberOfHiddenLayerUnits, activation = tf$nn$leaky_relu)
    generatorLogits <- tf$keras$layers$Dense(units = dimension)

    discriminatorOptimizer <- tf$keras$optimizers$RMSprop(learning_rate = learningRate, epsilon = cEpsilon)
    generatorOptimizer <- tf$keras$optimizers$RMSprop(learning_rate = learningRate, epsilon = cEpsilon)

    checkPoint <- tf$train$Checkpoint(discriminatorHiddenLayer1 = discriminatorHiddenLayer1,
                                      discriminatorHiddenLayer2 = discriminatorHiddenLayer2,
                                      discriminatorLogits = discriminatorLogits,
                                      generatorHiddenLayer1 = generatorHiddenLayer1,
                                      generatorHiddenLayer2 = generatorHiddenLayer2,
                                      generatorLogits = generatorLogits,
                                      discriminatorOptimizer = discriminatorOptimizer,
                                      generatorOptimizer = generatorOptimizer)
    
    if(generativeModelRead) {
        checkPoint$read(gdGetFileName(generativeModelFileName))    
    }


    
    discriminatorNetwork <- function(input, dropout) {
        discriminatorHiddenLayer1Dropout <- tf$nn$dropout(discriminatorHiddenLayer1(input), dropout)
        discriminatorHiddenLayer2Dropout <- tf$nn$dropout(discriminatorHiddenLayer2(discriminatorHiddenLayer1Dropout), dropout)
        discriminatorLogits(discriminatorHiddenLayer2Dropout)
    } 

    generatorNetwork <- function(input, dropout) {
        generatorHiddenLayer1Dropout <- tf$nn$dropout(generatorHiddenLayer1(input), dropout)
        generatorHiddenLayer2Dropout <- tf$nn$dropout(generatorHiddenLayer2(generatorHiddenLayer1Dropout), dropout)
        logits <- generatorLogits(generatorHiddenLayer2Dropout)
    } 
        
    loss <- function(logitsIn, labelsIn) {
        loss <- tf$reduce_mean(tf$nn$sigmoid_cross_entropy_with_logits(labels = labelsIn, logits = logitsIn))
    }

    trainingCore <- tf_function(function(samples, noise, dropout) {
        with(tf$GradientTape(persistent = TRUE) %as% tape, {
            logitsReal <- discriminatorNetwork(samples, dropout)
            logitsFake <- discriminatorNetwork(generatorNetwork(noise, dropout), dropout)
            
            realLoss <- loss(logitsReal, tf$ones_like(logitsReal))
            fakeLoss <- loss(logitsFake, tf$zeros_like(logitsFake))
            discriminatorLoss <- realLoss + fakeLoss
            generatorLoss <- -discriminatorLoss
        })
        
        discriminatorVariables <- append(discriminatorHiddenLayer1$trainable_weights, discriminatorHiddenLayer2$trainable_weights)
        discriminatorVariables <- append(discriminatorVariables, discriminatorLogits$trainable_weights)
        discriminatorOptimizer$minimize(discriminatorLoss, discriminatorVariables, tape)

        generatorVariables <- append(generatorHiddenLayer1$trainable_weights, generatorHiddenLayer2$trainable_weights)
        generatorVariables <- append(generatorVariables, generatorLogits$trainable_weights)
        generatorOptimizer$minimize(generatorLoss, generatorVariables, tape)
    })

    generationCore <- tf_function(function(noise, dropout) {
        generatedSamples <- generatorNetwork(noise, dropout)
        logitsFake <- discriminatorNetwork(generatedSamples, dropout)
        predicted <- tf$nn$sigmoid(logitsFake)

        generatedData <- list()
        generatedData[[1]] <- generatedSamples
        generatedData[[2]] <- predicted
        
        return(generatedData)
    })
    
    trainingIteration <- function(iteration, train, step, weight, generate) {
        samples <- NULL
        noise <- NULL

        if(train) {
            for(i in 1:cNumberOfBatchesPerIteration) {
                samples <- array_reshape(gdDataSourceGetNormalizedDataRandom(batchSize), c(batchSize, dimension))
 
                if(step == "Reset") {
                    samples <- array(runif(batchSize * dimension, 0.0, 1.0), c(batchSize, dimension))
                }
                
                noise <- array(runif(batchSize * dimension, -1.0, 1.0), c(batchSize, dimension))
                
                if(step == "Initialize") {
                    noiseSamples <- array(runif(batchSize * dimension, 0.0, 1.0), c(batchSize, dimension))
                    samples <- noiseSamples + weight * (samples - noiseSamples)
                }
                
                trainingCore(samples, noise, dropout)
            }
        }
            
        noise <- array(runif(batchSize * dimension, -1.0, 1.0), c(batchSize, dimension))
        generatedData <- generationCore(noise, dropout)
            
        gs <- array(generatedData[[1]], c(batchSize, dimension))
        gp <- array(generatedData[[2]], c(batchSize))
            
        k <- 0
        for(j in 1:batchSize) {
            if(gp[j] >= cLevel) {
                k <- k + 1
            }
        }
        gsp <- array(0, c(k, dimension))
        l <- 1
        for(j in 1:batchSize) {
            if(gp[j] >= 0.5) {
                gsp[l,] = gs[j,]
                l <- l + 1
            }
        }
        gsp <- array_reshape(gsp, c(k * dimension))
        if((train && step == "Training") || generate) {
            gdAddValueRows(gsp)
        }
            
        if(train && showPlot) {
            if(dimension == 1) {
                plot(gs[, columnIndices[1]], array(0, batchSize), main = "gdTrain", cex.main = 1.0, font.main = 1, xlim = c(0.0, 1.0), ylim = c(0.0, 1.0), col = ifelse(gp >= 0.5, "green", "red"), xlab = columnNames[1], ylab = columnNames[2])
            } else {
                plot(gs[, columnIndices[1]], gs[, columnIndices[2]], main = "gdTrain", cex.main = 1.0, font.main = 1, xlim = c(0.0, 1.0), ylim = c(0.0, 1.0), col = ifelse(gp >= 0.5, "green", "red"), xlab = columnNames[1], ylab = columnNames[2])      
            }
            legend("topleft", legend = c("gd positive", "gd negative", "ds"), col = c("green", "red", "blue"), pch = c(1, 1, 1), bty = "n", horiz = TRUE)
            
            if(train) {
                if(dimension == 1) {
                    points(samples[, columnIndices[1]], array(0, batchSize), col = "blue")
                } else {
                    points(samples[, columnIndices[1]], samples[, columnIndices[2]], col = "blue")
                }
            }
        }
    }
    
    train <- function(){
        numberOfInitializationIterations <- trainParameters$numberOfInitializationIterations
        numberOfTrainingIterations <- trainParameters$numberOfTrainingIterations
        
        if(!generativeModelRead) {
            message("Initialization iterations")

            for(iteration in 1:numberOfInitializationIterations) {
                if(iteration <= cResetIterations) {
                    trainingIteration(iteration, TRUE, "Reset", -1, FALSE)
                } else {
                    weight <- (iteration - cResetIterations) / (numberOfInitializationIterations - cResetIterations)
                    trainingIteration(iteration, TRUE, "Initialize", weight, FALSE)
                }
                
                if(iteration %% cWriteMessageModulo == 0) {
                    message(iteration)
                }
            }
        }
        
        message("Training iterations")

        for(iteration in 1:numberOfTrainingIterations) {
            trainingIteration(iteration, TRUE, "Training", -1, FALSE)
            
            if(iteration %% cWriteMessageModulo == 0) {
                message(iteration)
            }
        }
        
        if(!is.null(generativeModelFileName) && nchar(generativeModelFileName) > 0) {
            if(!generativeModelRead) {
                gdCreateGenerativeModel()
                
                gdGenerativeModelSetNumberOfTrainingIterations(numberOfTrainingIterations)
                gdGenerativeModelSetNumberOfInitializationIterations(numberOfInitializationIterations)
                gdGenerativeModelSetNumberOfHiddenLayerUnits(numberOfHiddenLayerUnits)
                gdGenerativeModelSetLearningRate(learningRate)
                gdGenerativeModelSetDropout(dropout)
            } else {
                gdGenerativeModelSetNumberOfTrainingIterations(gdGenerativeModelGetNumberOfTrainingIterations() + numberOfTrainingIterations)
            }
            
            checkPoint$write(gdGetFileName(generativeModelFileName))
            gdWriteWithReadingTrainedModel(generativeModelFileName)
        }
        
        if(!is.null(generativeDataFileName) && nchar(generativeDataFileName) > 0) {
            gdGenerativeDataWrite(generativeDataFileName)
        }
    }
    
    generate <- function() {
        message("Number of samples")
        
        numberOfSamples <- generateParameters$numberOfSamples
        
        n <- 1
        while(n < numberOfSamples) {
            trainingIteration(n, FALSE, "", -1, TRUE)
            n <- gdGetNumberOfRows()
            
            message(n)
        }
        
        if(!is.null(generativeDataFileName) && nchar(generativeDataFileName) > 0) {
            gdGenerativeDataWrite(generativeDataFileName)
        }
    }
    
    if(!is.null(trainParameters)) {
        train()
    }
    
    if(!is.null(generateParameters)) {
        generate()
    }
}

