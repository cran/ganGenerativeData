# Copyright 2021 Werner Mueller
# Released under the GPL (>= 2)

library(httr)

source("R/gdTrain.R")

cPort <- 3000

#' Send a request to software service for accelerated training of generative
#' models to train a generative model for a data source
#' 
#' Send a request to software service to train a generative model. A data source
#' file will be uploaded and a job for training will be generated. A job id for
#' the generated job will be returned which has to be used in related requests.
#' The job will be processed as soon as other waiting jobs are processed. When
#' a name of an existing generative model file is passed the file will be also
#' uploaded and the job will continue the training. See also functions
#' gdTrain(), gdServiceGetGenerativeModel(), gdServiceGetGenerativeData(),
#' gdServiceGetStatus(), gdServiceDelete().
#'
#' @param url URL of software service for accelerated training of generative
#' models
#' @param accessKey Unique key for access to software service
#' @param generativeModelFileName Name of generative model file
#' @param generativeDataFileName Name of generative data file. If name is NULL
#' or empty string generated data will not be written to a file.
#' @param dataSourceFileName Name of data source file
#' @param trainParameters Generative model training parameters, see
#' function gdTrainParameters().
#' 
#' @return Job Id number
#' @export
#'
#' @examples
#' \dontrun{
#' url <- "http://xxx.xxx.xxx.xxx/gdService"
#' accessKey <- "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
#' trainParameters <- gdTrainParameters(numberOfTrainingIterations = 10000)
#' jobId <- gdServiceTrain(url, accessKey, "gm.bin", "gd.bin", "ds.bin", trainParameters)}
gdServiceTrain <- function(url,
                    accessKey,
                    generativeModelFileName,
                    generativeDataFileName,
                    dataSourceFileName,
                    trainParameters = gdTrainParameters(numberOfTrainingIterations = 10000,
                                                        numberOfInitializationIterations = 2500,
                                                        numberOfHiddenLayerUnits = 1024,
                                                        learningRate = 0.00007,
                                                        dropout = 0.05)) {
    response <- NULL                                                                                                                    
    
    result = tryCatch({
        response <- GET(modify_url(url = url, port = 3000, query = list(accessKey = accessKey)))
    }, warning = function(warning) {
        message(warning)
    }, error = function(error) {
        message(error, "")
        message("This error can occur if used URL is invalid or software service is not active.")
    })
    
    if(is.null(response)) {
        #return()
        stop("No response")
    }
    
    if(http_error(response)) {
        stop(rawToChar(response$content))
    }
    
    jobId <- rawToChar(response$content)
    
    if(file.exists(generativeModelFileName)) {
        response <- POST(modify_url(url = paste(url, "/gm", sep = ""), port = cPort, query = list(accessKey = accessKey, jobId = jobId, fileName = generativeModelFileName)), body = list(gm = upload_file(generativeModelFileName)), encode = "multipart", config = progress())
    } else {
        response <- POST(modify_url(url = paste(url, "/gm", sep = ""), port = cPort, query = list(accessKey = accessKey, jobId = jobId, fileName = generativeModelFileName)))
    }
    if(http_error(response)) {
        stop(rawToChar(response$content))
    }
    
    if(file.exists(generativeDataFileName)) {
        response <- POST(modify_url(url = paste(url, "/gd", sep = ""), port = cPort, query = list(accessKey = accessKey, jobId = jobId, fileName = generativeDataFileName)), body = list(gd = upload_file(generativeDataFileName)), encode = "multipart", config = progress())
    } else {
        response <- POST(modify_url(url = paste(url, "/gd", sep = ""), port = cPort, query = list(accessKey = accessKey, jobId = jobId, fileName = generativeDataFileName)))
    }
    if(http_error(response)) {
        stop(rawToChar(response$content))
    }
    
    response <- POST(modify_url(url = paste(url, "/ds", sep = ""), port = cPort, query = list(accessKey = accessKey, jobId = jobId, fileName = dataSourceFileName)), body = list(ds = upload_file(dataSourceFileName)), encode = "multipart", config = progress())
    if(http_error(response)) {
        stop(rawToChar(response$content))
    }
    
    #response <- POST(modify_url(url = paste(url, "/parameter", sep = ""), port = cPort, query = list(accessKey = accessKey, jobId = jobId, numberOfIterations = trainParameters$numberOfIterations, numberOfHiddenLayerUnits = trainParameters$numberOfHiddenLayerUnits, learningRate = trainParameters$learningRate, droput = trainParameters$dropout)))
    response <- POST(modify_url(url = paste(url, "/parameter", sep = ""), port = cPort, query = list(accessKey = accessKey, jobId = jobId, numberOfTrainingIterations = trainParameters$numberOfTrainingIterations, numberOfInitializationIterations = trainParameters$numberOfInitializationIterations, numberOfHiddenLayerUnits = trainParameters$numberOfHiddenLayerUnits, learningRate = trainParameters$learningRate, droput = trainParameters$dropout)))
    if(http_error(response)) {
        stop(rawToChar(response$content))
    }
    
    response <- POST(modify_url(url = paste(url, "/status", sep = ""), port = cPort, query = list(accessKey = accessKey, jobId = jobId, status = "UPLOADED")))
    if(http_error(response)) {
        stop(rawToChar(response$content))
    }
    
    return(as.numeric(jobId))
}

#' Get generative model from software service for accelerated training of
#' generative models for processed job
#' 
#' Download trained generative model from software service for processed job.
#' The status of the job has to be TRAINED.
#'
#' @param url URL of software service for accelerated training of generative
#' models
#' @param accessKey Unique key for access to software service
#' @param jobId Job id for generated job for training a generative model
#' @param generativeModelFileName Name of generative model file. If name is NULL
#' or empty string name of passed name to function gdServiceTrain() will be used.
#' 
#' @return None
#' @export
#'
#' @examples
#' \dontrun{
#' url <- "http://xxx.xxx.xxx.xxx/gdService"
#' accessKey <- "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
#' gdServiceGetGenerativeModel(url, accessKey, 1,  "gm.bin")}
gdServiceGetGenerativeModel <- function(url, accessKey, jobId, generativeModelFileName = NULL) {
    response <- NULL
    
    result = tryCatch({
        response <- GET(modify_url(url = paste(url, "/parameter/gm", sep = ""), port = cPort, query = list(accessKey = accessKey, jobId = jobId)))
    }, warning = function(warning) {
        message(warning)
    }, error = function(error) {
        message(error, "")
        message("This error can occur if used URL is invalid or software service is not active.")
    })
    
    if(is.null(response)) {
        #return()
        stop("No response")
    }
    
    if(http_error(response)) {
        stop(rawToChar(response$content))
    }
        
    if(is.null(generativeModelFileName) || nchar(generativeModelFileName) == 0) {
        generativeModelFileName <- rawToChar(response$content)
    }
        
    response <- GET(modify_url(url = paste(url, "/gm", sep = ""), port = cPort, query = list(accessKey = accessKey, jobId = jobId)), write_disk(generativeModelFileName, overwrite = TRUE), progress())
    if(http_error(response)) {
        stop(response)
    }
}

#' Get generative data from software service for accelerated training of
#' generative models for processed job
#' 
#' Download generated generative data from software service for processed job.
#' The status of the job has to be TRAINED.
#'
#' @param url URL of software service for accelerated training of generative
#' models
#' @param accessKey Unique key for access to software service
#' @param jobId Job id for generated job for training a generative model
#' @param generativeDataFileName Name of generative data file. If name is NULL
#' or empty string name of passed name to function gdServiceTrain() will be used.
#' 
#' @return None
#' @export
#'
#' @examples
#' \dontrun{
#' url <- "http://xxx.xxx.xxx.xxx/gdService"
#' accessKey <- "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
#' gdServiceGetGenerativeData(url, accessKey, 1, "gd.bin")}
gdServiceGetGenerativeData <- function(url, accessKey, jobId, generativeDataFileName = NULL) {
    response <- NULL
    
    result = tryCatch({
        response <- GET(modify_url(url = paste(url, "/parameter/gd", sep = ""), port = cPort, query = list(accessKey = accessKey, jobId = jobId)))
    }, warning = function(warning) {
        message(warning)
    }, error = function(error) {
        message(error, "")
        message("This error can occur if used URL is invalid or software service is not active.")
    })
    
    if(is.null(response)) {
        #return()
        stop("No response")
    }
    
    if(http_error(response)) {
        stop(rawToChar(response$content))
    }
        
    if(is.null(generativeDataFileName) || nchar(generativeDataFileName) == 0) {
        generativeDataFileName <- rawToChar(response$content)
    }
        
    response <- GET(modify_url(url = paste(url, "/gd", sep = ""), port = cPort, query = list(accessKey = accessKey, jobId = jobId)), write_disk(generativeDataFileName, overwrite = TRUE), progress())
    if(http_error(response)) {
        stop(response)
    }
}

#' Get status of generated job from software service for accelerated training of
#' generative models
#' 
#' Get status of generated job from software service. Defined status values are:
#' CREATED, UPLOADED, TRAINING, TRAINED, DELETED, ERROR.
#'
#' @param url URL of software service for accelerated training of generative
#' models
#' @param accessKey Unique key for access to software service
#' @param jobId Job id for generated job for training a generative model
#' 
#' @return List containing status information
#' @export
#'
#' @examples
#' \dontrun{
#' url <- "http://xxx.xxx.xxx.xxx/gdService"
#' accessKey <- "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
#' gdServiceGetStatus(url, accessKey, 1)}
gdServiceGetStatus <- function(url, accessKey, jobId) {
    response <- NULL
    
    result = tryCatch({
        response <- GET(modify_url(url = paste(url, "/status", sep = ""), port = cPort, query = list(accessKey = accessKey, jobId = jobId)))
    }, warning = function(warning) {
        message(warning)
    }, error = function(error) {
        message(error, "")
        message("This error can occur if used URL is invalid or software service is not active.")
    })
    
    if(is.null(response)) {
        #return()
        stop("No response")
    }
    
    if(http_error(response)) {
        stop(rawToChar(response$content))
    }
    
    status <- rawToChar(response$content)
    return(eval(parse(text = status)))
}

#' Delete a generated job from software service for accelerated training of
#' generative models
#' 
#' Delete a generated job from software service. If the job is currently
#' executed it it will be stopped.
#'
#' @param url URL of software service for accelerated training of generative
#' models
#' @param accessKey Unique key for access to software service
#' @param jobId Job id for generated job for training a generative model
#' 
#' @return None
#' @export
#'
#' @examples
#' \dontrun{
#' url <- "http://xxx.xxx.xxx.xxx/gdService"
#' accessKey <- "xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx"
#' gdServiceDelete(url, accessKey, 1)}
gdServiceDelete <- function(url, accessKey, jobId) {
    response <- NULL
    
    result = tryCatch({
        response <- POST(modify_url(url = paste(url, "/status", sep = ""), port = cPort, query = list(accessKey = accessKey, jobId = jobId, status = "DELETED")))
    }, warning = function(warning) {
        message(warning)
    }, error = function(error) {
        message(error, "")
        message("This error can occur if used URL is invalid or software service is not active.")
    })
    
    if(is.null(response)) {
        #return()
        stop("No response")
    }
    
    if(http_error(response)) {
        stop(rawToChar(response$content))
    }
}
