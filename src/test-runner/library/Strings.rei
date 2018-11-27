/**
 * Copyright (c) Facebook, Inc. Co and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */;
/**
 * This represents a part of the result from a string diff. The variant can
 * assume one of three values:
 *
 *   Old(value) => value only occurs in the old string
 *   New(value) => value only occurs in the new string
 *   Both(value) => value occurs in both the old and new strings
 */
type difference =
  | Old(string)
  | New(string)
  | Both(string);

/**
* Computes the difference between two strings. The returned list will contain
* string elements wrapped in the `difference` variant.
*
*   ```
*   /* [Both("foo "), Old("apple"), New("orange"), Both(" bar")] */
*   let result = diff("foo apple bar", "foo orange bar");
*   ```
* Filtering out all `New` parts and concatenating the remaining values will
* result in a string equivalent to the first argument, the old string.
*
* Filtering out all `Old` parts and concatenating the remaining values will
* result in a string equivalent to the second argument, the new string.
*
* When there is a difference at a certain position the Old element will appear
* before the New element in the list. For example the above code has a result
* of: `[..., Old("apple"), New("orange"), ...]` instead
* of: `[..., New("orange"), Old("apple"), ...]`
*
* The output will be combined such that no two consecutive items in the result
* will have the same variant type. `Both("foo"), Both(" bar")` would not appear
* instead the result would contain a single element: `Both("foo bar")`.
*
* Algorithm based on paper: https://dl.acm.org/citation.cfm?id=359460.359467
*/
let diff: (~delimeter: Str.regexp, string, string) => list(difference);

/**
* Performs the diff between words. Delimeter is set as word boundaries (\b).
*/
let diffWords: (string, string) => list(difference);

/**
* Performs the diff between lines. Delimeter is set as new lines (\n).
*/
let diffLines: (string, string) => list(difference);

/**
* This prints the list of differences computed between two strings.
*
* The default formatting uses ANSI terminal escape codes to add formatting to
* the string:
*
* - The parts in both will be de-emphasized with a gray color.
* - The parts in old will be emphasized with strikethrough and a red color.
* - The parts in new will be emphasized with underline and a green color.
*
* The style can be customized by changing oldFormatter, newFormatter, or
* bothFormatter.
*/
let printDiff: (
 ~oldFormatter: string => string=?,
 ~newFormatter: string => string=?,
 ~bothFormatter: string => string=?,
 list(difference)
) => string;

/**
* This prints the list of differences computed between two strings. It
* customizes the formatting to modify the text's background color rather than
* the texts color itself.
*/
let printDiffBg: (list(difference)) => string;

/**
* Returns the MD5 hash of a string. The output is a printable, hexadecimal
* representation of the hash.
*/
let md5: (string) => string;
