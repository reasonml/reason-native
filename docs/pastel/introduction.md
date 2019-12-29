---
id: introduction
title: Pastel Introduction
---

Pastel is a text formatting library that harnesses [Reason JSX](https://reasonml.github.io/docs/en/jsx) to provide intuitive terminal output. It's like React, but for your command line!

## Tips and Tricks

### Terminal Configuration Tips

This is a set of tips for configuring your terminal so that you take advantage
of the full expressivity of the various stylings that modern terminals provide.
If you build apps that use these features, keep in mind the kinds of
configuration users will have enabled.

These suggestions are not specific to `Pastel`, but will allow you to use all
of `Pastel`'s features to the fullest.

**Colors**:

- Ensure you have configured your terminal color scheme so that "bright" colors
  are noticeably different than non-bright colors.
- Background color isn't the same thing as "black". If using a dark color
  scheme, ensure that both "black" and "bright black" are different from
  "background", and different from each other.  Similarly, "white" is not the
  same as foreground color. Configure both "white" and "bright white" so that
  they are different from "foreground", and different from each other.
- Disable terminal features that try to render bold fonts as bright colors.
  There's no need to conflate the two - brightness and boldness are fully
  independent attributes which `Pastel` can manage.
- Make sure your terminal is set to render bold text in a bold font.
- Configure your terminal to allow italics.

Specific Terminal Configuration:

**iTerm**:

- iTerm often won't render underlines unless you enable ligatures (your font
  might need to declare support of ligatures).

The following shows the best way to configure iTerm:

![iTerm optimal configuration](/img/itermConfig.png)

A reference colorscheme for iTerm that works well is
[taste.itermcolors](/file/taste.itermcolors)

**Others**:

Please help populate this document with suggestions for other popular
terminals: Kitty, Mintty
