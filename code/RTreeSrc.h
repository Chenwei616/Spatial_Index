#ifndef RTREE_SRC_H_INCLUDED
#define RTREE_SRC_H_INCLUDED

#include "Geometry.h"
#include "Tree.h"

#include <algorithm>
#include <array>
#include <queue>
#include <string>
#include <vector>

#include "CMakeIn.h"

namespace hw6
{

	/// <summary>
	/// </summary>
	/// <param name="M">Maximum number of children of each nodes.</param>
	template <uint8_t M>
	class RNode
	{
	private:
		RNode<M> *parent = nullptr;
		std::array<RNode<M> *, M> children = []()
		{
			decltype(children) ret;
			for (decltype(M) i = 0; i < M; ++i)
				ret[i] = nullptr;
			return ret;
		}();
		decltype(M) childrenNum = 0;
		Envelope bbox;
		std::vector<Feature> features;

	public:
		RNode() = delete;
		RNode(const Envelope &box) : bbox(box) {}

		inline bool isLeafNode() { return childrenNum == 0; }

		inline const Envelope &getEnvelope() { return bbox; }

		inline RNode<M> *getParent() { return parent; }

		inline void setEnvelope(const Envelope &box) { bbox = box; }

		inline RNode<M> *getChildNode(size_t i)
		{
			return i < childrenNum ? children[i] : nullptr;
		}

		inline const RNode<M> *getChildNode(size_t i) const
		{
			return i < childrenNum ? children[i] : nullptr;
		}

		inline decltype(M) getChildNum() const { return childrenNum; }

		inline size_t getFeatureNum() const { return features.size(); }

		inline const Feature &getFeature(size_t i) const { return features[i]; }

		inline const std::vector<Feature> &getFeatures() const { return features; }

		inline void add(const Feature &f) { features.push_back(f); }

		inline void add(RNode<M> *child)
		{
			children[childrenNum] = child;
			child->parent = this;
			++childrenNum;
		}

		inline void remove(const Feature &f)
		{
			auto where = [&]()
			{
				for (auto itr = features.begin(); itr != features.end(); ++itr)
					if (itr->getName() == f.getName())
						return itr;
			}();
			features.erase(where);
			if (features.empty())
				features.shrink_to_fit(); // free memory unused but allocated
		}

		inline void remove(RNode<M> *child)
		{
			for (decltype(M) i = 0; i < childrenNum; ++i)
				if (children[i] == child)
				{
					--childrenNum;
					std::swap(children[i], children[childrenNum]);
					children[childrenNum] = nullptr;
					break;
				}
		}

		inline Feature popBackFeature()
		{
			auto ret = features.back();
			features.pop_back();
			return ret;
		}

		inline RNode<M> *popBackChildNode()
		{
			--childrenNum;
			auto ret = children[childrenNum];
			children[childrenNum] = nullptr;
			return ret;
		}

		void countNode(int &interiorNum, int &leafNum)
		{
			if (isLeafNode())
			{
				++leafNum;
			}
			else
			{
				++interiorNum;
				for (decltype(M) i = 0; i < childrenNum; ++i)
					children[i]->countNode(interiorNum, leafNum);
			}
		}

		int countHeight(int height)
		{
			++height;
			if (!isLeafNode())
			{
				int cur = height;
				for (decltype(M) i = 0; i < childrenNum; ++i)
					height = max(height, children[i]->countHeight(cur));
			}
			return height;
		}

		inline void draw()
		{
			if (isLeafNode())
			{
				bbox.draw();
			}
			else
				for (decltype(M) i = 0; i < childrenNum; ++i)
				{
					bbox.draw();
					children[i]->draw();
				}
		}

		void rangeQuery(const Envelope &rect, std::vector<Feature> &features)
		{
			// TODO
			int h = countHeight(0);
			std::vector<RNode<M> *> nodes, tmpNodes;
			for (int i = 0; i < getChildNum(); i++)
				nodes.push_back(getChildNode(i));
			for (int i = 0; i < h - 2; i++)
			{
				for (int j = 0; j < nodes.size(); j++)
				{
					if ((nodes[j]->getEnvelope()).intersect(rect))
						for (int k = 0; k < nodes[j]->getChildNum(); k++)
							tmpNodes.push_back(nodes[j]->getChildNode(k));
				}
				nodes = tmpNodes;
				tmpNodes.clear();
			}
			for (int i = 0; i < nodes.size(); i++)
			{
				const std::vector<Feature> &allFeatures = nodes[i]->getFeatures();
				for (int j = 0; j < allFeatures.size(); j++)
					if (allFeatures[j].getEnvelope().intersect(rect))
						features.push_back(allFeatures[j]);
			}
		}

		int depth;
		RNode<M>* res;
		void dfs(double x, double y, RNode<M>* node, int d)
		{
			if (node->isLeafNode())
				res = node;
			bool flag = true;
			for (int i = 0; i < node->getChildNum(); i++)
			{
				RNode<M>* tmpNode = node->getChildNode(i);
				if(tmpNode->getEnvelope().contain(x, y))
				{
					flag = false;
					dfs(x, y, tmpNode, d + 1);
				}
			}
			if (flag && d >= depth)
			{
				depth = d;
				res = node;
			}
		}


		RNode<M> *pointInLeafNode(double x, double y)
		{
			// TODO
			RNode<M> *node = this;
			int h = countHeight(0);
			depth = 0;
			res = nullptr;
			dfs(x, y, node, 1);
			return res;
		}
	};

	template <uint8_t M>
	class RTree : public Tree
	{
	private:
		// Vars here MAY be useful, but it's ok to delete them {
		constexpr static auto m = M >> 1;
		constexpr static auto M_minus_m = M - m;
		constexpr static double EQ_ERR = 0.0000000005;
		// }

		RNode<M> *root = nullptr;

	public:
		RTree() : Tree(M) { static_assert(M >= 4); }
		~RTree()
		{
			if (root != nullptr)
				delete root;
			root = nullptr;
		}

		virtual void setCapacity(int capacity) override
		{
			// DO NOTHING, since capacity is immutable in R tree
		}

		virtual bool constructTree(const std::vector<Feature> &features) override
		{
			// TODO
			std::vector<Feature> existFeatures;
			std::vector<RNode<M> *> leafNodes;

			// 叶子节点获取
			for (auto it = features.begin(); it != features.end(); ++it)
			{
				double minAreaAdd = 1000000;
				Envelope featureEnvelope = (*it).getEnvelope(), markEnvelope;
				RNode<M> *mark;
				std::vector<RNode<M> *>::iterator markIter;
				int markPos;
				if (leafNodes.size() == 0)
				{
					RNode<M> *firstNode = new RNode<M>(featureEnvelope);
					firstNode->add((*it));
					leafNodes.push_back(firstNode);
					continue;
				}

				// 与每个叶子节点比
				for (auto rIter = leafNodes.begin(); rIter != leafNodes.end(); ++rIter)
				{
					Envelope nodeEnvelope = (*rIter)->getEnvelope();
					double minX = std::min(featureEnvelope.getMinX(), nodeEnvelope.getMinX());
					double minY = std::min(featureEnvelope.getMinY(), nodeEnvelope.getMinY());
					double maxX = std::max(featureEnvelope.getMaxX(), nodeEnvelope.getMaxX());
					double maxY = std::max(featureEnvelope.getMaxY(), nodeEnvelope.getMaxY());
					Envelope newEnvelope(minX, maxX, minY, maxY);
					double areaAdd = newEnvelope.getArea() - nodeEnvelope.getArea();
					if (areaAdd < minAreaAdd)
					{
						minAreaAdd = areaAdd;
						mark = *rIter;
						markEnvelope = newEnvelope;
						markIter = rIter;
					}
				}

				if (mark->getFeatureNum() < M)
				{
					mark->add(*it);
					mark->setEnvelope(markEnvelope);
				}
				// 二次分裂
				else
				{
					std::vector<Feature> allFeatures = mark->getFeatures();
					Feature seed1, seed2, insertFeature = *it;
					std::vector<Feature>::iterator seed1Iter, seed2Iter;
					int seed1pos, seed2pos;
					allFeatures.push_back(insertFeature);
					double left = 1000000, right = -1000000;
					for (auto fIter = allFeatures.begin(); fIter != allFeatures.end(); ++fIter)
					{
						if (left > fIter->getEnvelope().getMinX())
						{
							left = fIter->getEnvelope().getMinX();
							seed1 = *fIter;
							seed1Iter = fIter;
						}
						if (right < fIter->getEnvelope().getMaxX())
						{
							right = fIter->getEnvelope().getMaxX();
							seed2 = *fIter;
							seed2Iter = fIter;
						}
					}
					Envelope env1 = seed1.getEnvelope(), env2 = seed2.getEnvelope();
					std::vector<Feature> seed1Features, seed2Features;
					seed1Features.push_back(seed1);
					seed2Features.push_back(seed2);

					for (auto fIter = allFeatures.begin(); fIter != allFeatures.end();)
					{
						if (fIter->getName() == seed1.getName())
						{
							fIter = allFeatures.erase(fIter);
							break;
						}
						else
							fIter++;
					}
					for (auto fIter = allFeatures.begin(); fIter != allFeatures.end();)
					{
						if (fIter->getName() == seed2.getName())
						{
							fIter = allFeatures.erase(fIter);
							break;
						}
						else
							fIter++;
					}
					for (auto fIter = allFeatures.begin(); fIter != allFeatures.end(); ++fIter)
					{
						double addArea1, addArea2;
						double minX = std::min(fIter->getEnvelope().getMinX(), env1.getMinX());
						double minY = std::min(fIter->getEnvelope().getMinY(), env1.getMinY());
						double maxX = std::max(fIter->getEnvelope().getMaxX(), env1.getMaxX());
						double maxY = std::max(fIter->getEnvelope().getMaxY(), env1.getMaxY());
						Envelope newEnvelope1, newEnvelope2;
						newEnvelope1 = Envelope(minX, maxX, minY, maxY);
						addArea1 = newEnvelope1.getArea() - env1.getArea();
						minX = std::min(fIter->getEnvelope().getMinX(), env2.getMinX());
						minY = std::min(fIter->getEnvelope().getMinY(), env2.getMinY());
						maxX = std::max(fIter->getEnvelope().getMaxX(), env2.getMaxX());
						maxY = std::max(fIter->getEnvelope().getMaxY(), env2.getMaxY());
						newEnvelope2 = Envelope(minX, maxX, minY, maxY);
						addArea2 = newEnvelope2.getArea() - env2.getArea();
						if (addArea1 < addArea2)
						{
							seed1Features.push_back(*fIter);
							env1 = newEnvelope1;
						}
						else
						{
							seed2Features.push_back(*fIter);
							env2 = newEnvelope2;
						}
					}
					RNode<M> *seedNode1 = new RNode<M>(env1);
					RNode<M> *seedNode2 = new RNode<M>(env2);
					for (auto fIter = seed1Features.begin(); fIter != seed1Features.end(); ++fIter)
						seedNode1->add(*fIter);
					for (auto fIter = seed2Features.begin(); fIter != seed2Features.end(); ++fIter)
						seedNode2->add(*fIter);
					leafNodes.erase(markIter);
					leafNodes.push_back(seedNode1);
					leafNodes.push_back(seedNode2);
				}
			}

			std::vector<RNode<M> *> childNodes;
			std::vector<RNode<M> *> parentNodes;
			RNode<M> *firstNode;
			std::vector<std::vector<RNode<M>>> treeNodes;
			for (int i = 0; i < leafNodes.size(); i++)
				childNodes.push_back(leafNodes[i]);
			bool flag = true;
			while (flag)
			{
				// 其余节点获取
				std::vector<RNode<M> *>::iterator markIter;
				firstNode = new RNode<M>(childNodes[0]->getEnvelope());
				firstNode->add(childNodes[0]);
				parentNodes.push_back(firstNode);
				for (auto childIter = childNodes.begin() + 1; childIter != childNodes.end(); ++childIter)
				{
					Envelope leafEnvelope = (*childIter)->getEnvelope(), markEnvelope;
					double minAddArea = 1000000;
					for (auto parentIter = parentNodes.begin(); parentIter != parentNodes.end(); ++parentIter)
					{
						Envelope parentEnvelope = (*parentIter)->getEnvelope();
						double minX = std::min(leafEnvelope.getMinX(), parentEnvelope.getMinX());
						double minY = std::min(leafEnvelope.getMinY(), parentEnvelope.getMinY());
						double maxX = std::max(leafEnvelope.getMaxX(), parentEnvelope.getMaxX());
						double maxY = std::max(leafEnvelope.getMaxY(), parentEnvelope.getMaxY());
						Envelope newEnvelope = Envelope(minX, maxX, minY, maxY);
						double addArea = newEnvelope.getArea() - parentEnvelope.getArea();
						if (minAddArea > addArea)
						{
							minAddArea = addArea;
							markIter = parentIter;
							markEnvelope = newEnvelope;
						}
					}
					if ((*markIter)->getChildNum() < M)
					{
						(*markIter)->add(*childIter);
						(*markIter)->setEnvelope(markEnvelope);
					}
					// 二次分裂
					else
					{
						std::vector<RNode<M>> allNodes;
						std::vector<RNode<M> *> allNodesPointers;
						for (int i = 0; i < (*markIter)->getChildNum(); i++)
							allNodesPointers.push_back((*markIter)->getChildNode(i));
						allNodesPointers.push_back(*childIter);
						std::vector<RNode<M>>::iterator seed1Iter, seed2Iter;
						RNode<M> *seed1Pointer(nullptr), *seed2Pointer(nullptr);

						double left = 1000000, right = -1000000;
						for (int i = 0; i < allNodesPointers.size(); i++)
						{
							if (left > allNodesPointers[i]->getEnvelope().getMinX() && allNodesPointers[i] != seed2Pointer)
							{
								left = allNodesPointers[i]->getEnvelope().getMinX();
								seed1Pointer = allNodesPointers[i];
							}
							if (right < allNodesPointers[i]->getEnvelope().getMaxX() && allNodesPointers[i] != seed1Pointer)
							{
								right = allNodesPointers[i]->getEnvelope().getMaxX();
								seed2Pointer = allNodesPointers[i];
							}
						}
						RNode<M> *seed1 = new RNode<M>(seed1Pointer->getEnvelope()), *seed2 = new RNode<M>(seed2Pointer->getEnvelope());
						seed1->add(seed1Pointer);
						seed2->add(seed2Pointer);
						Envelope env1 = seed1->getEnvelope(), env2 = seed2->getEnvelope();

						for (int i = 0; i < allNodesPointers.size(); i++)
						{
							RNode<M> *nodePointer = allNodesPointers[i];
							if (nodePointer == seed1Pointer)
								continue;
							if (nodePointer == seed2Pointer)
								continue;
							double addArea1, addArea2;
							double minX = std::min(nodePointer->getEnvelope().getMinX(), env1.getMinX());
							double minY = std::min(nodePointer->getEnvelope().getMinY(), env1.getMinY());
							double maxX = std::max(nodePointer->getEnvelope().getMaxX(), env1.getMaxX());
							double maxY = std::max(nodePointer->getEnvelope().getMaxY(), env1.getMaxY());
							Envelope newEnvelope1, newEnvelope2;
							newEnvelope1 = Envelope(minX, maxX, minY, maxY);
							addArea1 = newEnvelope1.getArea() - env1.getArea();
							minX = std::min(nodePointer->getEnvelope().getMinX(), env2.getMinX());
							minY = std::min(nodePointer->getEnvelope().getMinY(), env2.getMinY());
							maxX = std::max(nodePointer->getEnvelope().getMaxX(), env2.getMaxX());
							maxY = std::max(nodePointer->getEnvelope().getMaxY(), env2.getMaxY());
							newEnvelope2 = Envelope(minX, maxX, minY, maxY);
							addArea2 = newEnvelope2.getArea() - env2.getArea();
							if (addArea1 < addArea2)
							{
								seed1->add(nodePointer);
								env1 = newEnvelope1;
								seed1->setEnvelope(env1);
							}
							else
							{
								seed2->add(nodePointer);
								env2 = newEnvelope2;
								seed2->setEnvelope(env2);
							}
						}
						parentNodes.erase(markIter);
						parentNodes.push_back(seed1);
						parentNodes.push_back(seed2);
					}
				}
				if (parentNodes.size() <= M)
				{
					flag = false;
					Envelope env = parentNodes[0]->getEnvelope();
					for (int i = 1; i < parentNodes.size(); i++)
					{
						double minX = std::min(parentNodes[i]->getEnvelope().getMinX(), env.getMinX());
						double minY = std::min(parentNodes[i]->getEnvelope().getMinY(), env.getMinY());
						double maxX = std::max(parentNodes[i]->getEnvelope().getMaxX(), env.getMaxX());
						double maxY = std::max(parentNodes[i]->getEnvelope().getMaxY(), env.getMaxY());
						env = Envelope(minX, maxX, minY, maxY);
					}
					root = new RNode<M>(env);
					bbox = env;
					for (int i = 0; i < parentNodes.size(); i++)
						root->add(parentNodes[i]);
				}
				else
				{
					childNodes = parentNodes;
					parentNodes.clear();
				}
			}
			return true;
		}

		virtual void countNode(int &interiorNum, int &leafNum) override
		{
			interiorNum = leafNum = 0;
			if (root != nullptr)
				root->countNode(interiorNum, leafNum);
		}

		virtual void countHeight(int &height) override
		{
			height = 0;
			if (root != nullptr)
				height = root->countHeight(height);
		}

		virtual void rangeQuery(const Envelope &rect,
								std::vector<Feature> &features) override
		{
			features.clear();
			if (root != nullptr)
				root->rangeQuery(rect, features);
		}

		virtual bool NNQuery(double x, double y,
							 std::vector<Feature> &features) override
		{
			features.clear();
			// TODO
			RNode<M> *node = pointInLeafNode(x, y);
			if (node->isLeafNode())
			{
				std::vector<Feature> newFeatures = node->getFeatures();
				double dist = 1000000;
				for (auto it = newFeatures.begin(); it != newFeatures.end(); ++it)
				{
					Envelope env = it->getEnvelope();
					double d1 = env.getMinX() - x, d2 = env.getMinY() - y, d3 = env.getMaxX() - x, d4 = env.getMaxY() - y;
					double dist1 = sqrt(d1 * d1 + d2 * d2);
					double dist2 = sqrt(d1 * d1 + d4 * d4);
					double dist3 = sqrt(d3 * d3 + d2 * d2);
					double dist4 = sqrt(d3 * d3 + d4 * d4);
					dist = std::min(std::max(std::max(dist1, dist2), std::max(dist3, dist4)), dist);
				}
				const Envelope rect = Envelope(x - dist, x + dist, y - dist, y + dist);
				rangeQuery(rect, features);
			}
			else
			{
				std::vector<RNode<M> *> childs;
				for (int i = 0; i < node->getChildNum(); i++)
					childs.push_back(node->getChildNode(i));
				double dist = 1000000;
				for (auto it = childs.begin(); it != childs.end(); ++it)
				{
					RNode<M> *tmpNode = *it;
					Envelope env = tmpNode->getEnvelope();
					double d1 = env.getMinX() - x, d2 = env.getMinY() - y, d3 = env.getMaxX() - x, d4 = env.getMaxY() - y;
					double dist1 = sqrt(d1 * d1 + d2 * d2);
					double dist2 = sqrt(d1 * d1 + d4 * d4);
					double dist3 = sqrt(d3 * d3 + d2 * d2);
					double dist4 = sqrt(d3 * d3 + d4 * d4);
					dist = std::min(std::max(std::max(dist1, dist2), std::max(dist3, dist4)), dist);
				}
				const Envelope rect = Envelope(x - dist, x + dist, y - dist, y + dist);
				rangeQuery(rect, features);
			}
			return true;
		}

		RNode<M> *pointInLeafNode(double x, double y)
		{
			if (root != nullptr)
				return root->pointInLeafNode(x, y);
			else
				return nullptr;
		}

		virtual void draw() override
		{
			if (root != nullptr)
				root->draw();
		}

	public:
		static void test(int t);

		static void analyse();
	};

} // namespace hw6

#endif // !RTREE_SRC_H_INCLUDED
