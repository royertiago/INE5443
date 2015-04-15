User manual
===========

Introduction
------------

Most of the tools are written in C++ and must be compiled before used.
Run

    make

in the command line to build all tools.
These programs use some features of the new C++14 standard,
so you must have a recent compiler to compile the programs.
(I have tested with `gcc 4.9.1` and `clang 3.5.0`.)

Only the most important command-line options are given here.
All tools described here
answer correctly to the option `--help`,
so the full functionality can be obtained this way.

Most tools read things from `stdin` and write to `stdout`;
this should be controlled with pipes and I/O redirection.

The first section of this manual is about dataset utilities.
The part about Mahalanobis distance can be found in the "Image Patterns" section.


Dataset Utilities
=================

### Short demonstration

    $ make
    ...
    $ datatools/generate_spiral > spiral.data
    $ ./influence_areas < spiral.data
    # Might take several seconds
    $ eog output.png


Datasets
--------

All programs of these repository work arond "datasets".
A dataset is a file that have a header, containing metadata,
and the data, in a comma-separated format.

The exact format of the dataset can be found in
[`datasets/format.md`](datasets/format.md).
This information is needed only if you want to manipulate the datasets manually
or want to extend some program.

As a dataset example, we have the
[Iris flower dataset](datasets/iris_flower.data).


Classification
--------------

    ./classify --dataset <dataset>

This is the main program of this repository.
It will read the dataset specified in the command line
and a set of entries to be classified from the standard input.
Each entry is a comma-separated list of floating-point values;
for instance, for the iris flower dataset,
each entry must be a list of four numbers.

    0.5,2.5,3,7

is a valid entry for that dataset.
The program then will try to classify it according to some algorithm
(which algorithm is used can be controlled via command-line).
The result is printed to standard output.
For instance, the entry above is classified as

    Iris-virginica

by the default algorithm.

There should be one such entry per line.

Command line options:

    --hamming
    --manhattan
    --euclidean
Control which distance type will be used.
Note that `--hamming` and `--manhattan` are the same.

    --neighbors <N>
Chose the `k` in the kNN algorithm.


Visualization
-------------

    datatools/generate_svg

It reads a dataset in stdin and writes an image,
in the `svg` format, to stdout.

It, however, only support bidimensional datasets
with a single category --- that is,
no entry can have two different categories.

For specific jobs, like visualizing influence areas,
have dedicated (and better) tools.

Areas of Influence
---------------

    ./influence_areas

This program reads a dataset from stdin and writes,
to output.png, the influence areas of each class.

To compute the influence areas, the program starts with a blank image
(whose dimensions can be specified with `--width` and `--height` in the command-line)
and adjust the proportions of the input dataset
so that it fits on that image size.

Then, it classify each pixel of that image against the dataset,
and paint that pixel to inform about the classification.
The larger the image, the greater the resolution, the longer the run time.

The program pass the following options to the classifier:
    --manhattan
    --hamming
    --euclidean
    --neighbors <N>
    --normalize-tolerance <F>

Generating datasets
-------------------

There are two tools to generate datasets.
The first,

    datatools/subdataset

will produce a subset of the dataset given in standard input.

This subset will have a random selection
of the entries of the original dataset.

An interesting option is `--remaining <file>`:
this will create a file with the data entries
that were not chosen to be part of the subdataset,
but without the categories.

This file can be used to test the classifying algorithm,
since its answers can be compared against the solution.
The program

    ./test_classifier

automates this test. Run `./test_classifier --help` for more information.

The other tool to generate datasets is

    datatools/generate_spiral

It will generate a spiral dataset, with some noise.
Several aspects of the algorithm can be tweaked,
like the number of spirals, the noise intensity
and the number of points.
For more information, run `datatools/generate_spiral --help`.


Image Patterns
==============

### Short demonstration

Run

    ./image_pattern some_image.png

Chose some pixels on the image, and press any key.
The program will classify the other pixels in the screen
for proximity with the chosen pixels using the Mahalanobis distance.
The second image contains the classificationn result.
Pixels closer to white were classified as close to the chosen pixels;
darker pixels were classified as distant to the chosen pixels.

Pixel Choosing
--------------

    mahalanobis/pixel_chooser some_image.png

This program opens a window with `some_image.png` as background,
and lets the user select some pixels in the screen, using the mouse.
Each selected pixel is written to stdout,
in the format "column,row" --- in CSV format.

A black circle is painted around each selected pixel,
to provide some form of viusal feedback.

The `pixel_chooser` cares to not write the same coordinate to stdout twice,
even if the user clicked in the same pixel twice.

The program supports all image format supported by OpenCV.
It has no interesting command-line options.

Pixel Classifying
-----------------

    manalanobis/pixel_classifier some_image.png

This program reads a comma separated list of values from stdin,
corresponding to pixel coordinates of the image (generated with `pixel_chooser`),
and classify each pixel of the image against the input set.

How much close some pixel was classified is represented using gray scale.
The darker the pixel in the screen, the farther the original pixel was considered.

Command line options:

    --ouput <file>
Writes the generated image, with the classified pixels, to the chosen file.

    --mahalanobis
    --euclidean
    --manhattan
    --hamming
Chose the metric. Mahalanobis is the default.

You can run the program several times
to see the result of classifying with different norms.

The progam `image_pattern` merely concatenate the two programs.
Run `image_pattern --help` for more information.
