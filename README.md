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

Time Series
===========

The simulation defines a time series, which contains a collection of universe objects, one for each time step.
Each time step contains all information about the universe.

Octree
======

The octree divides space recursively into cubes. A cube is split into eight smaller cube if its population exceeds a limit.
The eight subcubes of a cube can be recombined if they themselves are not divided and if together they contain less bodies than the split limit.

The octree data class also stores three lists: the near pairs list, the far pairs list, and the near-far pair list.
The near pairs list is a list of pair of regions where the individual body-body forces must be calculated.
The far pairs list...
The near-far pairs list...

Whenever the octree is reorganized, 

Calculation
===========

* Reset acceleration
* Add acceleration
  * Iterate over distant regions
    * Iterate

