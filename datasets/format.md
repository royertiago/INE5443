Dataset format
==============

The datasets are stored in plain text,
as CSV (comma-separated values),
with some additional metadata in the beginning of the file.

The file header (the metadata)
is a list of lines that give information about the fields
of the data entries.

Every header line is in the format

    c [information]

`c` is a character that defines what the line does,
and `information` is the parameter.

-   `#`: Commentary.
    These lines should be ignored.
    These lines (if exist) shall appear
    only in the begginning of the file.

-   `n`: Number of columns.
    Declares that the data entries contains exactly `information` lines.
    Shall appear before any `c` or `f` directive.

-   `a`: Attribute.
    Declares that the next column is an attribute
    that should be measured for proximity
    when trying to classify an entry.
    `information` is the name of that field (like a caption).
    The field name is optional.

-   `c`: Clategory.
    Declares that the next column will say
    in which category the respective dataset entry falls.
    `information` is again a caption.

Note that the order of the `a` and `c` directives is important.
The number of these directives shall be the number given in the `d` directive.

Following the header there should be an empty line,
followed by all the entries in the dataset.
Each entry is a comma-separated list of values,
in the order presented by the `f` and `c` directives.
Each `f` directive correspond to a floating point value readable by scanf,
and each `c` directive correspond to a string without commas or line breaks.
There shall not have any whitespace surrounding the comma.
Each entry is followed by a newline character.

[An example](iris_flower.data) might help clarify this format.
