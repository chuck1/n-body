# n-body

C++ n-body simulation attempting to use OpenCL

[video](https://youtu.be/QUeIE-22BSI)

The original purpose of this project was to teach myself how to use OpenCL.
I was able to get OpenCL working with my code and running simulations.
At some point during development, OpenCL stopped working (possibly an NVidia driver issue).

## Branches

- an oct-tree structure is used to store objects
- reduces computation because clusters of objects can sometimes be treated as one in force-pair calculations

### steps for updating branches

1. for each branch at lowest level: check for bodies outside branch. send those to parent
2. ...


