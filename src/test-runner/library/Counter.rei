/**
 * Copyright 2004-present Facebook. All Rights Reserved.
 */
type t;

/**
 * Creates a new counter starting at 0.
 */
let create: unit => t;

/**
 * Creates a new counter starting at the given integer.
 */
let startingAt: int => t;

/**
 * Gets the next value of the counter. This function will never return the same
 * value for a particular counter twice. (Unless you overflow ints...).
 */
let next: t => int;
