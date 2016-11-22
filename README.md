N-Body
======

GPU N-Body simulation

Outline
=======

Data:

* Time Series
  * Universe Collection
    * Verse
      * Body Collection
        * Body
* Octree


The simulation defines a time series, which contains a collection of universe objects, one for each time step.
Each time step contains all information about the universe.

The octree divides space recursively into cubes. A cube is split into eight smaller cube if its population exceeds a limit.
The eight subcubes of a cube can be recombined if they themselves are not divided and if together they contain less bodies than the split limit.

