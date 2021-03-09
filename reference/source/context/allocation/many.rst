#######
context
#######
**********
allocation
**********
====
many
====
-----
basic
-----

In order to define a non-singular allocation one must explicitly define a size for the type.  This size is always relative to the contexts parent so for every instance of the parent there will be that many instances of the type.

+------------------------+------------+----------+----------+
| Header row, column 1   | Header 2   | Header 3 | Header 4 |
| (header rows optional) |            |          |          |
+========================+============+==========+==========+
| body row 1, column 1   | column 2   | column 3 | column 4 |
+------------------------+------------+----------+----------+
| body row 2             | ...        | ...      |          |
+------------------------+------------+----------+----------+


