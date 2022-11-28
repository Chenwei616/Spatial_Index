# 空间索引实现

## 如何运行代码

- 安装[CMake](https://cmake.org/download/)，版本大于等于3.16
  - 如果使用的 IDE 为 Visual Studio，也可用内置的 CMake
- 对于使用 VS Code 的同学，需要安装 VS Code 的 CMake 插件，并按网上教程配置环境变量、C/C++编译器等
- 直接用 IDE 打开此文件夹，一般会自动加载项目
  - 使用 Visual Studio 的同学，注意在 `项目 / hw6 project的CMake设置` 中点击”编辑JSON“，更改 `generator` 为本机适合的版本（如 "Visual Studio 16 2019 Win64" ），使用默认的 "Ninja" 作为 `generator` 可能有未知问题
  - 使用 Mac OS 或 Linux 的同学请使用 VS Code 或其他 IDE 打开
- 无需安装其他依赖，CMake 构建成功后可运行

## 需要补全的代码

- 标记为 `[[optional]]` 的是扩展要求

### Geometry.cpp

```cpp
double Point::distance(const LineString *line) const;
double Point::distance(const Polygon *polygon) const;

// [[optional]] 实现 Polygon 的内环几何数据存储，并修改 Point 到 Polygon 的欧式距离计算

[[optional]]
double LineString::distance(const LineString *line) const
[[optional]]
double LineString::distance(const Polygon *polygon) const

// [[optional]] 添加 MultiPoint、MultiLineString 和 MultiPolygon 类，同时给出正确性的证明

bool Envelope::contain(const Envelope &envelope) const;
bool Envelope::intersect(const Envelope &envelope) const;
Envelope Envelope::unionEnvelope(const Envelope &envelope) const;
[[optional]]
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

// [[optional]] 添加接口，实现 Spatial Join，同时给出正确性的证明
```

### QuadTreeTest.cpp

- 用于对 `QuadTree` 进行测试和性能分析

```cpp
[[optional]]
void QuadTree::analyse(); // 用于与R树进行比较，选择此扩展项需要同时实现R树的analyse()
```

### RTreeSrc.h

- `RTree` 类是模板类，所以实现写在头文件里
- 编码前，使用 VS 的模板实例化功能载入模板参数（“添加所有的现有实例化” > 选择“M=8”），才能在 VS 中启用代码补全等功能
- 实际使用时，M可以在 `Common.h` 中进行改变，默认为8

```cpp
virtual bool hw6::RTree<M>::constructTree(const std::vector<Feature>& features)
void hw6::RNode<M>::rangeQuery(const Envelope & rect,
                               std::vector<Feature>& features);
virtual bool hw6::RTree<M>::NNQuery(double x, double y,
                                    std::vector<Feature>& features)
RNode<M>* hw6::RNode<M>::pointInLeafNode(double x, double y);

// [[optional]] 添加接口，实现 Spatial Join，同时给出正确性的证明
```

### RTreeTest.h

- 用于对 `RTree` 进行测试和性能分析

```cpp
[[optional]]
static void hw6::RTree<M>::analyse(); // 用于与四叉树进行比较，选择此扩展项需要同时实现四叉树的analyse()
```

### 注意

- 代码中包含中文注释。所有中文注释都是 GB 编码的，需使用支持 GB 编码的编辑器打开。如果你在 Mac 或 Linux 上运行程序，可能需要先进行转码
- 运行时使用 四叉树 还是 R树 由有无定义宏 `USE_RTREE` 决定。`USE_RTREE` 位于 `Common.h` 中，默认为使用四叉树

## 作业要求

### 时间安排

1. 2022.11.22前完成组队，钉钉群登记组队信息。

2. 2022.12.20前上交代码和作业报告，在学在浙大系统提交文档，每位同学都需要提交。

3. 2022.12.20和2022.12.27课堂报告，每组5分钟报告+3分钟提问，提问的同学有加分，每个问题1分。报告内容包括实现功能列表、算法改进或新功能(如有)、查询性能分析、程序演示等，课堂报告后将报告PPT提交到学在浙大，截止日期2021.12.27，每位同学都需要提交。

### 作业目的

理解空间数据类型层次结构，熟悉包围盒Envelope在空间查询中作用，熟悉常见的空间计算方法，掌握四叉树和R-Tree的创建，掌握区域查询和最邻近查询方法，理解空间数据查询的过滤和精炼步。

### 作业内容

1. 基于提供的Geometry、Point、LineString (折线)和Polygon空间数据类型层次结构

   **基本要求(必须完成)：**

   1. 实现Point到LineString和Polygon之间的欧式距离计算

   **扩展要求(至少完成一个)：**

   2. 实现Polygon的内环几何数据存储，并修改Point到Polygon的欧式距离计算
   3. 实现LineString到LineString和Polygon的欧式距离计算
   4. 实现MultiPoint、MultiLineString和MultiPolygon类

   Point到LineString的距离计算分解为Point到每个线段的距离计算，即点P(x, y)到线段[P1(x1, y1), P2(y2, y2)]在二维笛卡尔空间的最短距离。基本思路是通过(P2 - P1)归一化向量和(P - P1)向量的内积，计算P在直线上的投影点，判断该投影点是否在线段上，如果在线段上，计算投影点到P的距离，不在线段上，计算P到线段端点距离的最小值。

   Point到Polygon的距离计算关键是判断Point是否在Polygon内部，可以通过射线法判断，参考https://www.cnblogs.com/luxiaoxun/p/3722358.html，如果Point在Polygon内部，距离为0，否则计算Point与Polygon边界距离。Polygon定义与PostGIS相同，外环第一个点和最后一个点为同一个点。

   ![1](src\1.gif)

2. 基于提供的Envelope类

   **基本要求(必须完成)：**

   1. contain、intersect和unionEnvelope函数

   **扩展要求(可选)：**

   2. 实现Polygon与Envelope相交判断intersects函数。

   空间索引是基于几何特征的包围盒创建，首先通过判断几何特征的包围盒是否和查询区域相交，降低复杂的几何特征空间关系计算的次数。contain含义和PostGIS中的contain不同，可以是相同的包围盒。

   ![2](src\2.jpg)

3. 基于提供的QuadNode和QuadTree类实现四叉树创建与查询

   **基本要求(必须完成)：**

   1. 实现constructQuadTree和split函数，创建四叉树

      四叉树创建输入一组几何特征，将节点分裂为四个子节点，每个特征加到包围盒重叠的子节点中(即一个特征可能在多个节点中)，删除当前节点的几何特征记录(即所有特征只存储在叶节点中)，如果子节点的几何特征个数大于capacity，递归生成子节点。

      ![3](src\3.jpg)

   2. 实现rangeQuery函数，完成区域查询

      区域查询输入区域rect，查询与区域rect相交的几何特征，存储在features。区域rect如果与当前节点的包围盒bbox相交，递归遍历四叉树，查询哪些几何特征的包围盒和查询区域相交(filter)；再获得可能和查询区域相交的候选几何特征后，精确判断几何特征是否与查询区域相交(refinement)。

      通过鼠标选择查询区域，在站点和道路数据上，验证区域查询。

      ![4](src\4.jpg)

   3. 实现NNQuery和pointInLeafNode函数，完成最邻近几何特征查询

      最邻近几何特征查询(NN)输入查询点(x, y)，返回与该点最邻近的几何特征，存储在feature。首先，通过pointInLeafNode查询点(x, y)所在的叶节点，计算查询点(x, y)与该叶节点内的几何特征包围盒的最大距离的最小值minDist，即通过包围盒而非原始几何加速最小距离计算；然后，构造查询区域 (x – minDist, x + minDist, y – minDist, y + minDist)，查询几何特征的包围盒与该区域相交的几何特征(filter)，再查询与查询点(x, y)距离最近的几何特征(refinement)。

      通过鼠标移动选择离鼠标最近的几何特征(站点和道路)，验证最邻近几何特征查询。

      ![5](src\5.jpg)

   **扩展要求(至少完成一个)：**

   4. 基于距离的空间关联 (Spatial Join)

      基于距离的空间关联输入两类几何特征的四叉树和空间距离约束条件，输出满足空间距离条件的所有几何特征对，通过公共自行车站点和道路进行验证。

   5. 四叉树性能分析

      使用纽约taxi上车点数据分析四叉树性能。四叉树的性能决定于参数capacity，即每个叶节点最多的几何特征数目，数值太多，每个叶节点判断次数太大，数值太小，树的层次太高。当capacity在[70, 200]时，计算四叉树的高度和叶节点数目，评估100000次随机最邻近几何特征查询的时间。分析不同参数下的性能，可以得出什么结论？

4. R-Tree的创建与查询

   **基本要求(必须完成)：**

   1. R-Tree创建

      R-Tree构建可以采用几何特征逐个插入，基于节点新增面积越小越好的原则选择节点，当超过节点所能存储的最大几何特征时，基于二次分裂(quadratic split)算法，选择最左和最右两个几何特征作为种子点，对几何特征进行分组。R-Tree也可以采用类似四叉树的递归构造方法。 

   2. 区域查询 (Range Query)

   3. 最邻近几何特征查询 (Nearest Neighbor)

   **扩展要求(至少完成一个)：**

   4. 基于距离的空间关联 (Spatial Join)
   5. data目录提供了纽约城市道路(highway)、自行车站点(station)和出租车上下车位置(taxi)数据，分析四叉树与R-Tree在不同数据下的性能差异。

5. **扩展要求(可选)：**如已实现多边形的相关函数，特别是包围盒与Polygon相交函数，测试多边形数据集的区域查询和最邻近查询。

### 作业步骤

1. 根据README文档环境配置，完成各任务的代码，并进行测试验证。
2. 完成作业报告，包括原理、实现、测试、分析等，并提供成员分工情况。

### 考核标准

基本要求和扩展要求功能实现正确性、程序测试完整性、空间查询高效性、课堂报告问题回答正确性等。
