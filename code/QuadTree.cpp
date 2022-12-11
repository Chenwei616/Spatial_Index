#include "QuadTree.h"
#include <set>

namespace hw6
{

    /*
     * QuadNode
     */
    void QuadNode::split(size_t capacity)
    {
        for (int i = 0; i < 4; ++i)
        {
            delete children[i];
            children[i] = nullptr;
        }

        // Task construction
        // TODO
        // ���Ϊ4���ӽڵ�
        children[0] = new QuadNode(Envelope(bbox.getMinX(), bbox.getMaxX() - bbox.getWidth() / 2, bbox.getMinY(), bbox.getMaxY() - bbox.getHeight() / 2));
        children[1] = new QuadNode(Envelope(bbox.getMinX() + bbox.getWidth() / 2, bbox.getMaxX(), bbox.getMinY(), bbox.getMaxY() - bbox.getHeight() / 2));
        children[2] = new QuadNode(Envelope(bbox.getMinX(), bbox.getMaxX() - bbox.getWidth() / 2, bbox.getMinY() + bbox.getHeight() / 2, bbox.getMaxY()));
        children[3] = new QuadNode(Envelope(bbox.getMinX() + bbox.getWidth() / 2, bbox.getMaxX(), bbox.getMinY() + bbox.getHeight() / 2, bbox.getMaxY()));
        // children[0]->bbox.print();
        // children[1]->bbox.print();
        // children[2]->bbox.print();
        // children[3]->bbox.print();
        // std::cout << std::endl;

        // ����Ҫ��
        for (auto i : features)
        {
            for (auto j = 0; j < 4; ++j)
            {
                if (children[j]->getEnvelope().intersect(i.getEnvelope()))
                    children[j]->features.push_back(i);
            }
        }

        // ɾ��Ҫ������
        features.clear();

        // ����ϸ��
        for (auto i = 0; i < 4; ++i)
        {
            // std::cout << children[i]->features.size() << std::endl;
            if (children[i]->features.size() > capacity)
                children[i]->split(capacity);
        }
    }

    void QuadNode::countNode(int &interiorNum, int &leafNum)
    {
        if (isLeafNode())
        {
            ++leafNum;
        }
        else
        {
            ++interiorNum;
            for (int i = 0; i < 4; ++i)
                children[i]->countNode(interiorNum, leafNum);
        }
    }

    int QuadNode::countHeight(int height)
    {
        ++height;
        if (!isLeafNode())
        {
            int cur = height;
            for (int i = 0; i < 4; ++i)
            {
                height = std::max(height, children[i]->countHeight(cur));
            }
        }
        return height;
    }

    void QuadNode::rangeQuery(const Envelope &rect,
                              std::vector<Feature> &features)
    {
        if (!bbox.intersect(rect))
            return;

        // Task range query
        // TODO
    }

    QuadNode *QuadNode::pointInLeafNode(double x, double y)
    {
        // Task NN query
        // TODO

        return nullptr;
    }

    void QuadNode::draw()
    {
        if (isLeafNode())
        {
            bbox.draw();
        }
        else
        {
            for (int i = 0; i < 4; ++i)
                children[i]->draw();
        }
    }

    /*
     * QuadTree
     */
    bool QuadTree::constructTree(const std::vector<Feature> &features)
    {
        if (features.empty())
            return false;

        // Task construction
        // TODO
        bbox = features[0].getEnvelope();
        for (auto i : features)
        {
            bbox = bbox.unionEnvelope(i.getEnvelope());
        }
        root = new QuadNode(bbox);
        root->add(features);
        root->split(capacity);
        // bbox.print();
        // std::cout << std::endl;
        // bbox = Envelope(-74.1, -73.8, 40.6, 40.8); // ע����д�����Ҫ����Ϊfeatures�İ�Χ�У�����ڵ�İ�Χ��

        return true;
    }

    void QuadTree::countNode(int &interiorNum, int &leafNum)
    {
        interiorNum = 0;
        leafNum = 0;
        if (root)
            root->countNode(interiorNum, leafNum);
    }

    void QuadTree::countHeight(int &height)
    {
        height = 0;
        if (root)
            height = root->countHeight(0);
    }

    void QuadTree::rangeQuery(const Envelope &rect,
                              std::vector<Feature> &features)
    {
        features.clear();

        // Task range query
        // TODO

        // filter step (ѡ���ѯ�����뼸�ζ����Χ���ཻ�ļ��ζ���)

        // ע���Ĳ��������ѯ�����غ�ѡ������������hw6��rangeQuery�����
    }

    bool QuadTree::NNQuery(double x, double y, std::vector<Feature> &features)
    {
        if (!root || !(root->getEnvelope().contain(x, y)))
            return false;

        // Task NN query
        // TODO

        // filter step
        // (ʹ��maxDistance2Envelope��������ò�ѯ�㵽���ζ����Χ�е���̵������룬Ȼ�������ѯ��ú�ѡ��)

        const Envelope &envelope = root->getEnvelope();
        double minDist = std::max(envelope.getWidth(), envelope.getHeight());

        // ע���Ĳ����ڽ���ѯ�����غ�ѡ������������hw6��NNQuery�����

        return true;
    }

    void QuadTree::draw()
    {
        if (root)
            root->draw();
    }

} // namespace hw6
