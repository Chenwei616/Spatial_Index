#include "QuadTree.h"
#include "Common.h"

#include "CMakeIn.h"

using namespace hw6;

extern int mode;
extern std::vector<Geometry *> readGeom(const char *filename);
extern std::vector<std::string> readName(const char *filename);
extern void transformValue(double &res, const char *format);
extern void wrongMessage(Envelope e1, Envelope e2, bool cal);
extern void wrongMessage(const Point &pt1, const Point &pt2, double dis,
                         double res);
extern void wrongMessage(Envelope e1, Envelope e2, Envelope cal, Envelope res);
extern void wrongMessage(const hw6::Polygon &p, hw6::Envelope e, bool cal);

namespace hw6
{

    void QuadTree::analyse()
    {
        using namespace std;

        vector<Feature> features;
        vector<Geometry *> geom = readGeom(PROJ_SRC_DIR "/data/taxi");
        vector<string> name = readName(PROJ_SRC_DIR "/data/taxi");

        features.clear();
        features.reserve(geom.size());
        for (size_t i = 0; i < geom.size(); ++i)
            features.push_back(Feature(name[i], geom[i]));

        cout << "taxi number: " << geom.size() << endl;

        for (int cap = 70; cap <= 200; cap += 10)
        {
            QuadTree *qtree = new QuadTree();
            // Task �����Ĳ���������Ĳ����Ľڵ���Ŀ�͸߶�
            // TODO
            qtree->setCapacity(cap);
            clock_t start_time = clock();
            qtree->constructTree(features);
            clock_t end_time = clock();
            // cout << "Height: ";

            int height = 0, interiorNum = 0, leafNum = 0;
            qtree->countHeight(height);
            qtree->countNode(interiorNum, leafNum);

            cout << "Capacity " << cap << "\n";
            cout << "Height: " << height
                 << " \tInterior node number: " << interiorNum
                 << " \tLeaf node number: " << leafNum << "\n";
            cout << "Construction time: " << (end_time - start_time) / 1000.0 // << "s"
                 << endl;

            double x, y;
            Feature nearestFeature;
            vector<Feature> candidateFeatures;
            srand(0);

            start_time = clock();
            for (int i = 0; i < 100000; ++i)
            {
                x = -((rand() % 225) / 10000.0 + 73.9812);
                y = (rand() % 239) / 10000.0 + 40.7247;
                qtree->NNQuery(x, y, candidateFeatures);
                double dist = 1000000;
                for (auto it = candidateFeatures.begin(); it != candidateFeatures.end(); ++it)
                {
                    double tmpDist = it->getGeom()->distance(&Point(x, y));
                    if (tmpDist < dist && tmpDist)
                    {
                        dist = tmpDist;
                        nearestFeature = *it;
                    }
                }
                candidateFeatures.clear();
            }
            end_time = clock();
            cout << "NNQuery time: " << (end_time - start_time) / 1000.0 // << "s"
                 << endl
                 << endl;

            delete qtree;
        }
    }

    void QuadTree::test(int t)
    {
        using namespace std;

        cout << "*********************Start*********************" << endl;
        if (t == TEST1)
        {
            cout << "����1: Envelope Contain, Intersect, and Union" << endl;

            int failedCase = 0;
            Envelope e1(-1, 1, -1, 1);
            vector<Envelope> tests;
            tests.push_back(Envelope(-0.5, 0.5, -0.5, 0.5));
            tests.push_back(Envelope(-0.5, 0.5, 0.5, 1.5));
            tests.push_back(Envelope(0.5, 1.5, -0.5, 0.5));
            tests.push_back(Envelope(-1.5, -0.5, -1.5, -0.5));
            tests.push_back(Envelope(-2, -1, -0.5, 0.5));
            tests.push_back(Envelope(1, 1.5, 1, 1.5));
            tests.push_back(Envelope(-2, -1.5, -0.5, 0.5));
            tests.push_back(Envelope(-0.5, 0.5, 1.5, 2));
            tests.push_back(Envelope(-2, -1.5, 0.5, 1.5));
            tests.push_back(Envelope(0.5, 1.5, 1.5, 2));

            for (size_t i = 0; i < tests.size(); ++i)
            {
                if (e1.contain(tests[i]) != (i == 0))
                {
                    failedCase += 1;
                    wrongMessage(e1, tests[i], (i != 0));
                }
                if (tests[i].contain(e1) == true)
                {
                    failedCase += 1;
                    wrongMessage(tests[i], e1, true);
                }
            }
            cout << "Envelope Contain: " << tests.size() * 2 - failedCase << " / "
                 << tests.size() * 2 << " tests are passed" << endl;

            failedCase = 0;
            for (size_t i = 0; i < tests.size(); ++i)
            {
                if (e1.intersect(tests[i]) != (i < 6))
                {
                    failedCase += 1;
                    wrongMessage(e1, tests[i], !(i < 6));
                }
                if (tests[i].intersect(e1) != (i < 6))
                {
                    failedCase += 1;
                    wrongMessage(tests[i], e1, !(i < 6));
                }
            }
            cout << "Envelope Intersect: " << tests.size() * 2 - failedCase << " / "
                 << tests.size() * 2 << " tests are passed" << endl;

            failedCase = 0;
            vector<Envelope> results;
            results.push_back(Envelope(-1, 1, -1, 1));
            results.push_back(Envelope(-1, 1, -1, 1.5));
            results.push_back(Envelope(-1, 1.5, -1, 1));
            results.push_back(Envelope(-1.5, 1, -1.5, 1));
            results.push_back(Envelope(-2, 1, -1, 1));
            results.push_back(Envelope(-1, 1.5, -1, 1.5));
            results.push_back(Envelope(-2, 1, -1, 1));
            results.push_back(Envelope(-1, 1, -1, 2));
            results.push_back(Envelope(-2, 1, -1, 1.5));
            results.push_back(Envelope(-1, 1.5, -1, 2));
            for (size_t i = 0; i < tests.size(); ++i)
            {
                if (e1.unionEnvelope(tests[i]) != results[i])
                {
                    failedCase += 1;
                    wrongMessage(e1, tests[i], e1.unionEnvelope(tests[i]),
                                 results[i]);
                }
                if (tests[i].unionEnvelope(e1) != results[i])
                {
                    failedCase += 1;
                    wrongMessage(tests[i], e1, e1.unionEnvelope(tests[i]),
                                 results[i]);
                }
            }
            cout << "Envelope Union: " << tests.size() * 2 - failedCase << " / "
                 << tests.size() * 2 << " tests are passed" << endl;
        }
        else if (t == TEST2)
        {
            cout << "����2: Distance between Point and LineString" << endl;

            vector<Point> points;
            points.push_back(Point(0, 0));
            points.push_back(Point(10, 10));
            LineString line(points);

            points.push_back(Point(-10, -10));
            points.push_back(Point(20, 20));
            points.push_back(Point(5, 5));
            points.push_back(Point(10, 0));
            points.push_back(Point(10, -10));
            points.push_back(Point(0, 10));
            points.push_back(Point(0, 20));
            points.push_back(Point(20, 0));

            double dists[] = {0, 0, 14.1421, 14.1421, 0,
                              7.07107, 14.1421, 7.07107, 14.1421, 14.1421};

            int failedCase = 0;
            for (size_t i = 0; i < points.size(); ++i)
            {
                double dist = points[i].distance(&line);
                if (fabs(dist - dists[i]) > 0.0001)
                {
                    failedCase += 1;
                    cout << "Your answer is " << dist << " for test between ";
                    line.print();
                    cout << " and ";
                    points[i].print();
                    cout << ", but the answer is " << dists[i] << endl;
                }
            }
            cout << "Distance between Point and LineString: "
                 << points.size() - failedCase << " / " << points.size()
                 << " tests are passed" << endl;
        }
        else if (t == TEST3)
        {
            cout << "����3: Distance between Point and Polygon" << endl;

            vector<Point> points;
            points.push_back(Point(5, 0));
            points.push_back(Point(3, 6));
            points.push_back(Point(2, 4));
            points.push_back(Point(-2, 4));
            points.push_back(Point(-3, 5));
            points.push_back(Point(-5, 0));
            points.push_back(Point(0, -3));
            points.push_back(Point(5, 0));
            LineString line(points);
            Polygon poly(line);

            points.clear();
            points.push_back(Point(5, 4));
            points.push_back(Point(3, 4));
            points.push_back(Point(0, 4));
            points.push_back(Point(-3, 4));
            points.push_back(Point(-5, 4));
            points.push_back(Point(5, 5));
            points.push_back(Point(3, 5));
            points.push_back(Point(0, 5));
            points.push_back(Point(-3, 5));
            points.push_back(Point(0, 0));

            double dists[] = {1.26491, 0, 0, 0, 1.48556, 1.58114, 0, 1, 0, 0};

            int failedCase = 0;
            for (size_t i = 0; i < points.size(); ++i)
            {
                double dist = points[i].distance(&poly);
                if (fabs(dist - dists[i]) > 0.00001)
                {
                    failedCase += 1;
                    cout << "Your answer is " << dist << " for test between ";
                    poly.print();
                    cout << " and ";
                    points[i].print();
                    cout << ", but the answer is " << dists[i] << endl;
                }
            }
            cout << "Distance between Point and Polygon: "
                 << points.size() - failedCase << " / " << points.size()
                 << " tests are passed" << endl;
        }
        else if (t == TEST4)
        {
            cout << "����4: QuadTree Construction" << endl;
            int ncase, cct;
            ncase = cct = 2;
            QuadTree qtree;
            vector<Geometry *> geom = readGeom(PROJ_SRC_DIR "/data/polygon");
            vector<Feature> features;

            for (size_t i = 0; i < geom.size(); ++i)
                features.push_back(Feature("", geom[i]));

            qtree.setCapacity(1);
            qtree.constructTree(features);

            int height, interiorNum, leafNum;
            qtree.countHeight(height);
            qtree.countNode(interiorNum, leafNum);

            if (!(height == 6 && interiorNum == 8 && leafNum == 25))
            {
                cout << "Case 1: "
                     << "Your answer is height: " << height
                     << ", interiorNum: " << interiorNum << ", leafNum: " << leafNum
                     << " for case1, but the answer is height: 6, interiorNum: 8, "
                        "leafNum: 25\n";
                --cct;
            }

            features.clear();
            for (size_t i = 0; i < geom.size(); ++i)
                delete geom[i];
            geom.clear();

            vector<Geometry *> geom2 = readGeom(PROJ_SRC_DIR "/data/highway");
            vector<Feature> features2;
            QuadTree qtree2;

            for (size_t i = 0; i < geom2.size(); ++i)
                features2.push_back(Feature("", geom2[i]));

            qtree2.setCapacity(20);
            qtree2.constructTree(features2);

            int height2, interiorNum2, leafNum2;
            qtree2.countHeight(height2);
            qtree2.countNode(interiorNum2, leafNum2);

            if (!(height2 == 11 && interiorNum2 == 1386 && leafNum2 == 4159))
            {
                cout << "Case 2: "
                     << "Your answer is height: " << height2
                     << ", interiorNum: " << interiorNum2
                     << ", leafNum: " << leafNum2
                     << " for case2, but the answer is height: 11, interiorNum: "
                        "1386, leafNum: 4159\n";
                --cct;
            }

            features2.clear();
            for (size_t i = 0; i < geom2.size(); ++i)
                delete geom2[i];
            geom2.clear();

            cout << "QuadTree Construction: " << cct << " / " << ncase
                 << " tests are passed" << endl;
        }
        else if (t == TEST5)
        {
            cout << "����5: Distance between Point and Polygon with interior ring" << endl;

            vector<Point> points;
            points.push_back(Point(5, 0));
            points.push_back(Point(3, 6));
            points.push_back(Point(2, 4));
            points.push_back(Point(-2, 4));
            points.push_back(Point(-3, 5));
            points.push_back(Point(-5, 0));
            points.push_back(Point(0, -3));
            points.push_back(Point(5, 0));
            LineString ering(points);
            points.clear();
            points.push_back(Point(0, 3));
            points.push_back(Point(2, -1));
            points.push_back(Point(-2, -1));
            points.push_back(Point(0, 3));
            LineString iring1(points);
            std::vector<LineString> iRings;
            iRings.push_back(iring1);

            Polygon poly(ering, iRings);
            points.clear();
            points.push_back(Point(5, 4));
            points.push_back(Point(0, 0));
            points.push_back(Point(0, 3));
            points.push_back(Point(0, -1));
            points.push_back(Point(1, 3));
            points.push_back(Point(3, 4));
            points.push_back(Point(0, 2));
            points.push_back(Point(5, 5));
            points.push_back(Point(0, -2));
            points.push_back(Point(0, 5));
            double dists[] = {1.26491, 1, 0, 0, 0, 0, 0.447214, 1.58114, 0, 1};
            int failedCase = 0;
            for (size_t i = 0; i < points.size(); ++i)
            {
                double dist = points[i].distance(&poly);
                if (fabs(dist - dists[i]) > 0.00001)
                {
                    failedCase += 1;
                    cout << "Your answer is " << dist << " for test between ";
                    poly.print();
                    cout << " and ";
                    points[i].print();
                    cout << ", but the answer is " << dists[i] << endl;
                }
            }
            cout << "Distance between Point and Polygon: "
                 << points.size() - failedCase << " / " << points.size()
                 << " tests are passed" << endl;
        }
        else if (t == TEST6)
        {
            cout << "����6: Polygon and Envelope Intersect" << endl;

            int failedCase = 0;

            vector<Point> points;
            points.push_back(Point(5, 0));
            points.push_back(Point(3, 6));
            points.push_back(Point(2, 4));
            points.push_back(Point(-2, 4));
            points.push_back(Point(-3, 5));
            points.push_back(Point(-5, 0));
            points.push_back(Point(0, -3));
            points.push_back(Point(5, 0));
            LineString ering(points);
            points.clear();
            points.push_back(Point(0, 3));
            points.push_back(Point(2, -1));
            points.push_back(Point(-2, -1));
            points.push_back(Point(0, 3));
            LineString iring1(points);
            std::vector<LineString> iRings;
            iRings.push_back(iring1);
            points.clear();
            points.push_back(Point(2, 0));
            points.push_back(Point(3, 0));
            points.push_back(Point(3, 1));
            points.push_back(Point(2, 1));
            points.push_back(Point(2, 0));
            LineString iring2(points);
            iRings.push_back(iring2);
            Polygon poly(ering, iRings);

            vector<Envelope> tests;
            tests.push_back(Envelope(-1, 1, -1, 1));
            tests.push_back(Envelope(-8, 8, -8, 8));
            tests.push_back(Envelope(-1, 8, -1, 8));
            tests.push_back(Envelope(0, 3, 0, 3));
            tests.push_back(Envelope(-2, 2, -4, -3));
            tests.push_back(Envelope(-2, 2, 5, 6));
            tests.push_back(Envelope(-1, 1, -0.5, 0.5));
            tests.push_back(Envelope(2.1, 2.9, 0.1, 0.9));
            tests.push_back(Envelope(-6, -6, -4, -4));
            tests.push_back(Envelope(4, 4, 6, 6));

            for (size_t i = 0; i < tests.size(); ++i)
            {
                if (poly.intersects(tests[i]) != (i < 5))
                {
                    failedCase += 1;
                    wrongMessage(poly, tests[i], !(i < 5));
                }
            }
            cout << "Polygon and Envelope Intersect: " << tests.size() - failedCase << " / "
                 << tests.size() << " tests are passed" << endl;
        }
        else if (t == TEST8)
        {
            cout << "����8: QuadTreeAnalysis" << endl;
            analyse();
        }

        cout << "**********************End**********************" << endl;
    }

} // namespace hw6