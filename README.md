# Spatial_Index

## How to build

- Install [CMake](https://cmake.org/download/), version greater than or equal to 3.16
  - If you are using Visual Studio as your IDE, you can also use the built-in CMake.
- If you are using VS Code, you need to install the CMake plugin for VS Code and follow the online tutorial to configure environment variables, C/C++ compiler, etc.
- Open this folder directly with the IDE and the project will usually be loaded automatically.
  - If you are using Visual Studio, click "Edit JSON" in `Project / CMake settings for hw6 project` and change `generator` to the appropriate version for your machine (e.g. "Visual Studio 16 2019 Win64") If you use the default "Ninja" as `generator`, there may be unknown problems.
  - If you are using Mac OS or Linux, please use VS Code or other IDE to open it.
- No additional dependencies need to be installed, CMake can be run after a successful build

## core code

### Geometry.cpp

```cpp
double Point::distance(const LineString *line) const;
double Point::distance(const Polygon *polygon) const;

// Implement the inner ring geometry data storage of Polygon and modify the Euclidean distance calculation from Point to Polygon

double LineString::distance(const LineString *line) const
double LineString::distance(const Polygon *polygon) const

// Add the MultiPoint, MultiLineString, and MultiPolygon classes, and give a proof of correctness

bool Envelope::contain(const Envelope &envelope) const;
bool Envelope::intersect(const Envelope &envelope) const;
Envelope Envelope::unionEnvelope(const Envelope &envelope) const;
bool Polygon::intersects(const Envelope &rect) const
```

### QuadTree.cpp

```cpp
void QuadNode::split(size_t capacity);
bool QuadTree::constructTree(const std::vector<Feature> &features);
void QuadNode::rangeQuery(const Envelope &rect,
                          std::vector<Feature> &features);
void QuadTree::rangeQuery(const Envelope &rect,
                          std::vector<Feature> &features);
bool QuadTree::NNQuery(double x, double y,
                       std::vector<Feature> &features);
QuadNode *QuadNode::pointInLeafNode(double x, double y);
QuadNode *QuadTree::pointInLeafNode(double x, double y);

// Add an interface that implements Spatial Join
```

### QuadTreeTest.cpp

- For testing and performance analysis of `QuadTree`

```cpp
[[optional]]
void QuadTree::analyse(); // For comparison with R-tree
```

### RTreeSrc.h

- The `RTree` class is a template class, so the implementation is written in the header file
- Before coding, load the template parameters using VS's template instantiation feature ("Add all existing instantiations" > select "M=8") to enable code completion and other features in VS.
- In practice, M can be changed in `Common.h` and defaults to 8

```cpp
virtual bool hw6::RTree<M>::constructTree(const std::vector<Feature>& features)
void hw6::RNode<M>::rangeQuery(const Envelope & rect,
                               std::vector<Feature>& features);
virtual bool hw6::RTree<M>::NNQuery(double x, double y,
                                    std::vector<Feature>& features)
RNode<M>* hw6::RNode<M>::pointInLeafNode(double x, double y);

// Add an interface to implement Spatial Join
```

### RTreeTest.h

- For testing and performance analysis of `RTree`

```cpp
static void hw6::RTree<M>::analyse(); // For comparison with quadtrees, selecting this extension requires implementing quadtree analyse() as well
```

### Note

- The code contains Chinese comments. All Chinese comments are GB-encoded and need to be opened with an editor that supports GB-encoding. If you are running the program on Mac or Linux, you may need to transcode it first
- Whether the runtime uses a quadtree or an R-tree is determined by the presence or absence of the defined macro `USE_RTREE`. `USE_RTREE` is located in `Common.h` and defaults to using a quadtree

### Assignment content


1. based on the provided hierarchy of Geometry, Point, LineString (dash) and Polygon spatial data types

   1. implement the Euclidean distance calculation between Point to LineString and Polygon
   2. implement the inner-loop geometry data storage of Polygon and modify the Euclidean distance calculation from Point to Polygon
   3. implement the Euclidean distance calculation from LineString to LineString and Polygon
   4. implement MultiPoint, MultiLineString and MultiPolygon classes


   The distance calculation from Point to LineString is decomposed into the distance calculation from Point to each line segment, i.e. the shortest distance from point P(x, y) to the line segment [P1(x1, y1), P2(y2, y2)] in two-dimensional Cartesian space. The basic idea is to calculate the projection point of P on a line by the inner product of the (P2 - P1) normalized vector and the (P - P1) vector, determine whether the projection point is on the line segment, and if it is on the line segment, calculate the distance from the projection point to P. If it is not on the line segment, calculate the minimum value of the distance from P to the end point of the line segment.


   The key to calculate the distance from Point to Polygon is to determine whether the Point is inside the Polygon, which can be judged by the ray method, refer to https://www.cnblogs.com/luxiaoxun/p/3722358.html , if the Point is inside the Polygon, the distance is 0, otherwise, calculate the Point and Polygon boundary distance. Polygon definition is the same as PostGIS, the first point and the last point of the outer ring is the same point.


   ! [1](https://github.com/Chenweigenius/Spatial_Index/raw/main/src/1.gif)


2. Based on the Envelope class provided

   1. contain, intersect and unionEnvelope functions
   2. implement the Polygon and Envelope intersection judgment intersects function.


   Spatial index is created based on the envelope box of geometric features, first by judging whether the envelope box of geometric features intersects with the query area, reducing the number of complex spatial relationship calculations of geometric features. contain meaning is different from the contain in PostGIS, can be the same envelope box.

3. implement quadtree creation and query based on the provided QuadNode and QuadTree classes

   1. implement constructQuadTree and split function to create a quadtree

      Quadtree creation input a set of geometric features, split the node into four child nodes, each feature added to the enclosing box overlapping child nodes (i.e., a feature may be in more than one node), delete the geometric feature record of the current node (i.e., all features are stored in the leaf node only), if the number of geometric features of child nodes is greater than capacity, recursively generate child nodes.

      ! [3](https://github.com/Chenweigenius/Spatial_Index/raw/main/src/3.jpg)

   2. Implement the rangeQuery function to complete the region query

      If the region rect intersects with the enclosing box bbox of the current node, recursively traverse the quadtree and query which geometric features' enclosing boxes intersect with the query region (filter); then get the candidate geometric features that may intersect with the query region, and precisely determine whether the geometric features intersect with the query region (filter). After getting the candidate geometric features that may intersect with the query area, we can precisely determine whether the geometric features intersect with the query area (refinement).

      By selecting the query area with the mouse, the area query is verified on the site and road data.


      ! [4](https://github.com/Chenweigenius/Spatial_Index/raw/main/src/4.jpg)


   3. Implement the NNQuery and pointInLeafNode functions to complete the nearest neighbor geometric feature query


      Nearest Neighbor Geometric Feature Query (NN) inputs the query point (x, y) and returns the geometric feature that is closest to that point, stored in FEATURE. first, query the leaf node where the point (x, y) is located by pointInLeafNode, and calculate the minimum minDist of the maximum distance between the query point (x, y) and the geometric feature enclosing the box within that leaf node, i.e., by enclosing box instead of the original geometric acceleration minimum distance calculation; then, construct the query region (x - minDist, x + minDist, y - minDist, y + minDist), query the geometric features whose enclosing boxes intersect with this region (filter), and then query the geometric features with the closest distance to the query point (x , y) and the closest geometric feature (refinement).


      The nearest geometric feature (site and road) is selected by mouse movement to verify the nearest geometric feature query.


      ! [5](https://github.com/Chenweigenius/Spatial_Index/raw/main/src/5.jpg)


4. Distance-based Spatial Join (Spatial Join)


      Distance-based Spatial Join inputs quadtrees of two types of geometric features and spatial distance constraints, and outputs all pairs of geometric features that satisfy the spatial distance condition, verified by public bike stops and roads.


   5. Quadtree performance analysis


      The quadtree performance is analyzed using NYTAXI boarding point data. The performance of the quadtree is determined by the parameter capacity, which is the maximum number of geometric features per leaf node, too many values, the number of judgments per leaf node is too large, too small values, the tree level is too high. When the capacity is in [70, 200], the height of the quadtree and the number of leaf nodes are calculated and the time of 100000 random nearest-neighbor geometric feature queries is evaluated. What conclusions can be drawn from analyzing the performance with different parameters?


4. R-Tree creation and querying


1. R-Tree creation


      The R-Tree can be constructed by inserting geometric features one by one, selecting nodes based on the principle that the smaller the new area of nodes is, the better, and grouping geometric features based on the quadratic split algorithm when the maximum geometric features that nodes can store are exceeded, selecting the two leftmost and rightmost geometric features as seed points. The R-Tree can also be constructed recursively like a quadratic tree. 


   2. Range Query


   3. Nearest Neighbor Query

   4. Distance-based Spatial Join
   5. data directory provides New York City road (highway), bicycle station (station) and cab pick-up and drop-off location (taxi) data, analyze the performance difference between quadtree and R-Tree under different data. 5.

5. If the relevant functions of polygons have been implemented, especially the enclosing box and Polygon intersection functions, test the region query and nearest neighbor query of polygon dataset.
