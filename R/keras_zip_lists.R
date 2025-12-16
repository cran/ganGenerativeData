# Copyright 2021 Werner Mueller
# Released under the GPL (>= 2)

# Copied from keras

# zip lists
#
# This is conceptually similar to `zip()` in Python, or R functions
# `purrr::transpose()` and `data.table::transpose()` (albeit, accepting
# elements in `...` instead of a single list), with one crucial difference: if
# the provided objects are named, then matching is done by names, not
# positions.
#
# All arguments supplied must be of the same length. If positional matching is
# required, then all arguments provided must be unnamed. If matching by names,
# then all arguments must have the same set of names, but they can be in
# different orders.
#
# @param ... R lists or atomic vectors, optionally named.
#
# @return A inverted list
# @export
#
# @examples
# gradients <- list("grad_for_wt_1", "grad_for_wt_2", "grad_for_wt_3")
# weights <- list("weight_1", "weight_2", "weight_3")
# str(zip_lists(gradients, weights))
# str(zip_lists(gradient = gradients, weight = weights))
#
# names(gradients) <- names(weights) <- paste0("layer_", 1:3)
# str(zip_lists(gradients, weights[c(3, 1, 2)]))
#
# names(gradients) <- paste0("gradient_", 1:3)
# try(zip_lists(gradients, weights)) # error, names don't match
# # call unname directly for positional matching
# str(zip_lists(unname(gradients), unname(weights)))
keras_zip_lists <- function(...) {
  dots <- list(...)
  if(length(dots) == 1)
    dots <- dots[[1L]]

  nms1 <- names(dots[[1L]])

  if (is.character(nms1))
    if (!anyDuplicated(nms1) && !anyNA(nms1) && !all(nzchar(nms1)))
      stop("All names must be unique. Call `unname()` if you want positional matching")

  for(i in seq_along(dots)[-1L]) {
    d_nms <- names(dots[[i]])
    if(identical(nms1, d_nms))
      next
    if(setequal(nms1, d_nms)) {
      dots[[i]] <- dots[[i]][nms1]
      next
    }
    stop("All names of arguments provided to `zip_lists()` must match.",
         " Call `unname()` on each argument if you want positional matching")
  }

  ans <- .mapply(list, dots, NULL)
  names(ans) <- nms1
  ans
}
