# Islandora Delft Demo Module

## Introduction

A sample "solution pack" that takes basic images (gif, png, jpg) as `OBJ`, supplies a default ingest form, and creates a derivative (with DSID `BLUE`) that's coloured blue (inspired by images on Delftware pottery). This `BLUE` derivative is not displayed; displaying it in a viewer that looks like a piece of Delft pottery is left as an exercise to the reader.

## Requirements

This module requires the following modules/libraries:

* [Islandora](https://github.com/islandora/islandora)
* [ImageMagick](https://drupal.org/project/imagemagick)

## Installation

Install as usual, see [this](https://drupal.org/documentation/install/modules-themes/modules-7) for further information. This module installs new Fedora objects. If enabling through Drush, use the administrative user (e.g. `drush en -u 1 islandora_delft`).

## Configuration

At this time, there is no configuration for this module.

## Documentation

This module was created in three commits, to isolate aspects of the solution pack:
* [Initial module creation](https://github.com/mjordan/islandora_dev101/pull/6/commits/acd61b46c9cf894f5a188dcb254578a51af68d0a)
* [Add ingest forms.](https://github.com/mjordan/islandora_dev101/pull/6/commits/c0d91ec13952ac44784a8b392db4518de94c570a)
* [Add derivative generation.](https://github.com/mjordan/islandora_dev101/pull/6/commits/021bcd60f475d8da5002ea887eb501b4aff70c5d)

## Troubleshooting/Issues

Having problems or solved a problem? Check out the Islandora google groups for a solution.

* [Islandora Group](https://groups.google.com/forum/?hl=en&fromgroups#!forum/islandora)
* [Islandora Dev Group](https://groups.google.com/forum/?hl=en&fromgroups#!forum/islandora-dev)

## FAQ

Q. What does this module do?

A. Demonstrates how to make a module, that's it.

## Development

If you would like to contribute to this module, please check out [CONTRIBUTING.md](CONTRIBUTING.md). In addition, we have helpful [Documentation for Developers](https://github.com/Islandora/islandora/wiki#wiki-documentation-for-developers) info, as well as our [Developers](http://islandora.ca/developers) section on the [Islandora.ca](http://islandora.ca) site.

## License

This module is released as part of [Islandora Dev 101](https://github.com/mjordan/islandora_dev101), and is therefore covered by the [GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)
