#include "Geometry.h"
#include <cmath>
#include <gl/freeglut.h>

#define NOT_IMPLEMENT -1.0

namespace hw6
{

    /*
     * Envelope functions
     */
    bool Envelope::contain(double x, double y) const
    {
        return x >= minX && x <= maxX && y >= minY && y <= maxY;
    }

    bool Envelope::contain(const Envelope &envelope) const
    {
        // Task 测试Envelope是否包含关系
        // TODO
        return envelope.minX >= minX && envelope.maxX <= maxX && envelope.minY >= minY && envelope.maxY <= maxY;
    }

    bool Envelope::intersect(const Envelope &envelope) const
    {
        // Task 测试Envelope是否相交
        // TODO
        return maxX >= envelope.minX && minX <= envelope.maxX && maxY >= envelope.minY && minY <= envelope.maxY;
    }

    Envelope Envelope::unionEnvelope(const Envelope &envelope) const
    {
        // Task 合并两个Envelope生成一个新的Envelope
        // TODO
        return Envelope(std::min(minX, envelope.minX), std::max(maxX, envelope.maxX), std::min(minY, envelope.minY), std::max(maxY, envelope.maxY));
    }

    void Envelope::draw() const
    {
        glBegin(GL_LINE_STRIP);

        glVertex2d(minX, minY);
        glVertex2d(minX, maxY);
        glVertex2d(maxX, maxY);
        glVertex2d(maxX, minY);
        glVertex2d(minX, minY);

        glEnd();
    }

    /*
     * Points functions
     */
    double Point::distance(const Point *point) const
    {
        return sqrt((x - point->x) * (x - point->x) +
                    (y - point->y) * (y - point->y));
    }

    double Point::distance(const LineString *line) const
    {
        double mindist = line->getPointN(0).distance(this);
        for (size_t i = 0; i < line->numPoints() - 1; ++i)
        {
            double dist = 0;
            double x1 = line->getPointN(i).getX();
            double y1 = line->getPointN(i).getY();
            double x2 = line->getPointN(i + 1).getX();
            double y2 = line->getPointN(i + 1).getY();
            // Task calculate the distance between Point P(x, y) and Line [P1(x1, y1), P2(x2, y2)] (less than 10 lines)
            // TODO
            double x = this->getX(), y = this->getY(), x0, y0;
            if ((x - x1) / (y - y1) == (x - x2) / (y - y2) && ((x1 <= x && x <= x2) || (x2 <= x && x <= x1)))
            {
                mindist = 0;
                break;
            }
            else if ((x - x1) / (y - y1) == (x - x2) / (y - y2))
            {
                x0 = x;
                y0 = y;
            }
            else
            {
                x0 = x1 + ((x - x1) * (x2 - x1) + (y - y1) * (y2 - y1)) / (sqrt((1 + (y2 - y1) * (y2 - y1) / ((x2 - x1) * (x2 - x1))) * ((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1))));
                y0 = y1 + (y2 - y1) / (x2 - x1) * (x0 - x1);
                if (int((y - y0) * (y2 - y1) / ((x - x0) * (x2 - x1))) != -1)
                {
                    x0 = x1 - ((x - x1) * (x2 - x1) + (y - y1) * (y2 - y1)) / (sqrt((1 + (y2 - y1) * (y2 - y1) / ((x2 - x1) * (x2 - x1))) * ((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1))));
                    y0 = y1 + (y2 - y1) / (x2 - x1) * (x0 - x1);
                }
            }
            if ((x1 <= x0 && x0 <= x2) || (x2 <= x0 && x0 <= x1))
                dist = sqrt(pow((x - x0), 2) + pow((y - y0), 2));
            else
                dist = std::min(sqrt(pow((x - x1), 2) + pow((y - y1), 2)), sqrt(pow((x - x2), 2) + pow((y - y2), 2)));
            mindist = std::min(mindist, dist);
        }
        return mindist;
    }

    double Point::distance(const Polygon *polygon) const
    {
        std::vector<LineString> rings, inRings = polygon->getInteriorRings();
        size_t exNum = polygon->getExteriorRing().numPoints();
        double mindist = 0;

        rings.push_back(polygon->getExteriorRing());
        for (auto iter = inRings.begin(); iter != inRings.end(); iter++)
            if ((*iter).numPoints())
                rings.push_back(*iter);
        // Task whether Point P(x, y) is within Polygon (less than 15 lines)
        // TODO
        double x = this->getX(), y = this->getY();
        int leftcount = 0, rightcount = 0;

        for (auto iter = rings.begin(); iter != rings.end(); iter++)
        {
            if (this->distance(&(*iter)) == 0)
                return 0;
            for (size_t i = 0; i < (*iter).numPoints() - 1; i++)
            {
                double x1 = (*iter).getPointN(i).getX();
                double y1 = (*iter).getPointN(i).getY();
                double x2 = (*iter).getPointN(i + 1).getX();
                double y2 = (*iter).getPointN(i + 1).getY();
                if (y1 > y2)
                {
                    std::swap(x1, x2);
                    std::swap(y1, y2);
                }
                if ((y1 <= y && y <= y2))
                {
                    if (y2 == y && y != y1)
                    {
                        if (x > x2)
                            leftcount++;
                        else
                            rightcount++;
                    }
                    else if (y != y1)
                    {
                        double projectX = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
                        if (projectX < x)
                            leftcount++;
                        else
                            rightcount++;
                    }
                }
            }
        }

        if (!((leftcount % 2) && (rightcount % 2)))
        {
            for (auto iter = rings.begin(); iter != rings.end(); iter++)
            {
                double dist = this->distance(&(*iter));
                if (iter == rings.begin())
                    mindist = dist;
                else if (dist < mindist)
                    mindist = dist;
            }
        }
        return mindist;
    }

    // double Point::distance(const Polygon *polygon) const
    // {
    //     LineString exRing = polygon->getExteriorRing(), inRing = polygon->getInteriorRing();
    //     size_t exNum = exRing.numPoints(), inNum = inRing.numPoints();
    //     bool inPolygon = false;
    //     // Task whether Point P(x, y) is within Polygon (less than 15 lines)
    //     // TODO
    //     double x = this->getX(), y = this->getY();
    //     int leftcount = 0, rightcount = 0;
    //     if (inNum)
    //     {
    //         if (this->distance(&exRing) == 0 || this->distance(&inRing) == 0)
    //         {
    //             inPolygon = true;
    //             return 0;
    //         }
    //     }
    //     else
    //     {
    //         if (this->distance(&exRing) == 0)
    //         {
    //             inPolygon = true;
    //             return 0;
    //         }
    //     }
    //     for (size_t i = 0; i < exNum - 1; i++)
    //     {
    //         double x1 = exRing.getPointN(i).getX();
    //         double y1 = exRing.getPointN(i).getY();
    //         double x2 = exRing.getPointN(i + 1).getX();
    //         double y2 = exRing.getPointN(i + 1).getY();
    //         if (y1 > y2)
    //         {
    //             std::swap(x1, x2);
    //             std::swap(y1, y2);
    //         }
    //         if ((y1 <= y && y <= y2))
    //         {
    //             if (y2 == y && y != y1)
    //             {
    //                 if (x > x2)
    //                     leftcount++;
    //                 else
    //                     rightcount++;
    //             }
    //             else if (y != y1)
    //             {
    //                 double projectX = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    //                 if (projectX < x)
    //                     leftcount++;
    //                 else
    //                     rightcount++;
    //             }
    //         }
    //     }

    //     if (inNum)
    //     {
    //         for (size_t i = 0; i < inNum - 1; i++)
    //         {
    //             double x1 = inRing.getPointN(i).getX();
    //             double y1 = inRing.getPointN(i).getY();
    //             double x2 = inRing.getPointN(i + 1).getX();
    //             double y2 = inRing.getPointN(i + 1).getY();
    //             if (y1 > y2)
    //             {
    //                 std::swap(x1, x2);
    //                 std::swap(y1, y2);
    //             }
    //             if ((y1 <= y && y <= y2))
    //             {
    //                 if (y2 == y && y != y1)
    //                 {
    //                     if (x > x2)
    //                         leftcount++;
    //                     else
    //                         rightcount++;
    //                 }
    //                 else if (y != y1)
    //                 {
    //                     double projectX = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    //                     if (projectX < x)
    //                         leftcount++;
    //                     else
    //                         rightcount++;
    //                 }
    //             }
    //         }
    //     }

    //     if ((leftcount % 2) && (rightcount % 2))
    //         inPolygon = true;

    //     double mindist = 0;
    //     if (!inPolygon)
    //     {
    //         if (inNum)
    //         {
    //             double dist1 = this->distance(&exRing);
    //             double dist2 = this->distance(&inRing);
    //             mindist = std::min(dist1, dist2);
    //         }
    //         else
    //             mindist = this->distance(&exRing);
    //     }
    //     return mindist;
    // }

    // 仅有外环的距离判断
    // double Point::distance(const Polygon *polygon) const
    // {
    //     LineString line = polygon->getExteriorRing();
    //     size_t n = line.numPoints();

    //     bool inPolygon = false;
    //     // Task whether Point P(x, y) is within Polygon (less than 15 lines)
    //     // TODO
    //     double x = this->getX(), y = this->getY();
    //     int leftcount = 0, rightcount = 0;
    //     if (this->distance(&line) == 0)
    //     {
    //         inPolygon = true;
    //         return 0;
    //     }
    //     for (size_t i = 0; i < n - 1; i++)
    //     {
    //         double x1 = line.getPointN(i).getX();
    //         double y1 = line.getPointN(i).getY();
    //         double x2 = line.getPointN(i + 1).getX();
    //         double y2 = line.getPointN(i + 1).getY();
    //         if (y1 > y2)
    //         {
    //             std::swap(x1, x2);
    //             std::swap(y1, y2);
    //         }
    //         if ((y1 <= y && y <= y2))
    //         {
    //             if (y2 == y && y != y1)
    //             {
    //                 if (x > x2)
    //                     leftcount++;
    //                 else
    //                     rightcount++;
    //             }
    //             else if (y != y1)
    //             {
    //                 double projectX = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
    //                 if (projectX < x)
    //                     leftcount++;
    //                 else
    //                     rightcount++;
    //             }
    //         }
    //     }
    //     if ((leftcount % 2) && (rightcount % 2))
    //         inPolygon = true;

    //     double mindist = 0;
    //     if (!inPolygon)
    //         mindist = this->distance(&line);
    //     return mindist;
    // }

    bool Point::intersects(const Envelope &rect) const
    {
        return (x >= rect.getMinX()) && (x <= rect.getMaxX()) &&
               (y >= rect.getMinY()) && (y <= rect.getMaxY());
    }

    void Point::draw() const
    {
        glBegin(GL_POINTS);
        glVertex2d(x, y);
        glEnd();
    }

    /*
     * LineString functions
     */
    void LineString::constructEnvelope()
    {
        double minX, minY, maxX, maxY;
        maxX = minX = points[0].getX();
        maxY = minY = points[0].getY();
        for (size_t i = 1; i < points.size(); ++i)
        {
            maxX = std::max(maxX, points[i].getX());
            maxY = std::max(maxY, points[i].getY());
            minX = std::min(minX, points[i].getX());
            minY = std::min(minY, points[i].getY());
        }
        envelope = Envelope(minX, maxX, minY, maxY);
    }

    double LineString::distance(const LineString *line) const
    {
        // TODO
        return NOT_IMPLEMENT;
    }

    double LineString::distance(const Polygon *polygon) const
    {
        // TODO
        return NOT_IMPLEMENT;
    }

    typedef int OutCode;

    const int INSIDE = 0; // 0000
    const int LEFT = 1;   // 0001
    const int RIGHT = 2;  // 0010
    const int BOTTOM = 4; // 0100
    const int TOP = 8;    // 1000

    // Compute the bit code for a point (x, y) using the clip rectangle
    // bounded diagonally by (xmin, ymin), and (xmax, ymax)
    // ASSUME THAT xmax, xmin, ymax and ymin are global constants.
    OutCode ComputeOutCode(double x, double y, double xmin, double xmax,
                           double ymin, double ymax)
    {
        OutCode code;

        code = INSIDE; // initialised as being inside of [[clip window]]

        if (x < xmin) // to the left of clip window
            code |= LEFT;
        else if (x > xmax) // to the right of clip window
            code |= RIGHT;
        if (y < ymin) // below the clip window
            code |= BOTTOM;
        else if (y > ymax) // above the clip window
            code |= TOP;

        return code;
    }

    // Cohen–Sutherland clipping algorithm clips a line from
    // P0 = (x0, y0) to P1 = (x1, y1) against a rectangle with
    // diagonal from (xmin, ymin) to (xmax, ymax).
    bool intersectTest(double x0, double y0, double x1, double y1, double xmin,
                       double xmax, double ymin, double ymax)
    {
        // compute outcodes for P0, P1, and whatever point lies outside the clip
        // rectangle
        OutCode outcode0 = ComputeOutCode(x0, y0, xmin, xmax, ymin, ymax);
        OutCode outcode1 = ComputeOutCode(x1, y1, xmin, xmax, ymin, ymax);
        bool accept = false;

        while (true)
        {
            if (!(outcode0 | outcode1))
            {
                // bitwise OR is 0: both points inside window; trivially accept and
                // exit loop
                accept = true;
                break;
            }
            else if (outcode0 & outcode1)
            {
                // bitwise AND is not 0: both points share an outside zone (LEFT,
                // RIGHT, TOP, or BOTTOM), so both must be outside window; exit loop
                // (accept is false)
                break;
            }
            else
            {
                // failed both tests, so calculate the line segment to clip
                // from an outside point to an intersection with clip edge
                double x, y;

                // At least one endpoint is outside the clip rectangle; pick it.
                OutCode outcodeOut = outcode0 ? outcode0 : outcode1;

                // Now find the intersection point;
                // use formulas:
                //   slope = (y1 - y0) / (x1 - x0)
                //   x = x0 + (1 / slope) * (ym - y0), where ym is ymin or ymax
                //   y = y0 + slope * (xm - x0), where xm is xmin or xmax
                // No need to worry about divide-by-zero because, in each case, the
                // outcode bit being tested guarantees the denominator is non-zero
                if (outcodeOut & TOP)
                { // point is above the clip window
                    x = x0 + (x1 - x0) * (ymax - y0) / (y1 - y0);
                    y = ymax;
                }
                else if (outcodeOut & BOTTOM)
                { // point is below the clip window
                    x = x0 + (x1 - x0) * (ymin - y0) / (y1 - y0);
                    y = ymin;
                }
                else if (outcodeOut &
                         RIGHT)
                { // point is to the right of clip window
                    y = y0 + (y1 - y0) * (xmax - x0) / (x1 - x0);
                    x = xmax;
                }
                else if (outcodeOut &
                         LEFT)
                { // point is to the left of clip window
                    y = y0 + (y1 - y0) * (xmin - x0) / (x1 - x0);
                    x = xmin;
                }

                // Now we move outside point to intersection point to clip
                // and get ready for next pass.
                if (outcodeOut == outcode0)
                {
                    x0 = x;
                    y0 = y;
                    outcode0 = ComputeOutCode(x0, y0, xmin, xmax, ymin, ymax);
                }
                else
                {
                    x1 = x;
                    y1 = y;
                    outcode1 = ComputeOutCode(x1, y1, xmin, xmax, ymin, ymax);
                }
            }
        }
        return accept;
    }

    bool LineString::intersects(const Envelope &rect) const
    {
        double xmin = rect.getMinX();
        double xmax = rect.getMaxX();
        double ymin = rect.getMinY();
        double ymax = rect.getMaxY();

        for (size_t i = 1; i < points.size(); ++i)
            if (intersectTest(points[i - 1].getX(), points[i - 1].getY(),
                              points[i].getX(), points[i].getY(), xmin, xmax, ymin,
                              ymax))
                return true;
        return false;
    }

    void LineString::draw() const
    {
        glBegin(GL_LINE_STRIP);
        for (size_t i = 0; i < points.size(); ++i)
            glVertex2d(points[i].getX(), points[i].getY());
        glEnd();
    }

    void LineString::print() const
    {
        std::cout << "LineString(";
        for (size_t i = 0; i < points.size(); ++i)
        {
            if (i != 0)
                std::cout << ", ";
            std::cout << points[i].getX() << " " << points[i].getY();
        }
        std::cout << ")";
    }

    /*
     * Polygon
     */
    double Polygon::distance(const Polygon *polygon) const
    {
        return std::min(exteriorRing.distance(polygon),
                        polygon->getExteriorRing().distance(this));
    }

    bool Polygon::intersects(const Envelope &rect) const
    {
        // TODO
        bool flag_ExteriorRing = false, flag_InteriorRings = false, flag_Contains = false;
        std::vector<LineString> rings;
        rings.push_back(this->getExteriorRing());

        if (this->getExteriorRing().intersects(rect))
            flag_ExteriorRing = true;
        for (auto iter = this->interiorRings.begin(); iter != this->interiorRings.end(); iter++)
        {
            if ((*iter).intersects(rect))
                flag_InteriorRings = true;
            if ((*iter).numPoints())
                rings.push_back(*iter);
        }

        std::vector<std::vector<double>> envelope_Vertices = {{rect.getMaxX(), rect.getMaxY()},
                                                              {rect.getMaxX(), rect.getMinY()},
                                                              {rect.getMinX(), rect.getMaxY()},
                                                              {rect.getMinX(), rect.getMinY()}};

        for (auto iter = envelope_Vertices.begin(); iter != envelope_Vertices.end(); iter++)
        {
            double x = (*iter)[0], y = (*iter)[1];
            int leftcount = 0, rightcount = 0;

            for (auto iter = rings.begin(); iter != rings.end(); iter++)
                for (size_t i = 0; i < (*iter).numPoints() - 1; i++)
                {
                    double x1 = (*iter).getPointN(i).getX();
                    double y1 = (*iter).getPointN(i).getY();
                    double x2 = (*iter).getPointN(i + 1).getX();
                    double y2 = (*iter).getPointN(i + 1).getY();
                    if (y1 > y2)
                    {
                        std::swap(x1, x2);
                        std::swap(y1, y2);
                    }
                    if ((y1 <= y && y <= y2))
                    {
                        if (y2 == y && y != y1)
                        {
                            if (x > x2)
                                leftcount++;
                            else
                                rightcount++;
                        }
                        else if (y != y1)
                        {
                            double projectX = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
                            if (projectX < x)
                                leftcount++;
                            else
                                rightcount++;
                        }
                    }
                }
            if ((leftcount % 2) && (rightcount % 2))
                flag_Contains = true;
        }

        return flag_ExteriorRing || flag_InteriorRings || flag_Contains;
    }

    void Polygon::draw() const { exteriorRing.draw(); }

    void Polygon::print() const
    {
        std::cout << "Polygon(";
        std::cout << "(";
        for (size_t i = 0; i < exteriorRing.numPoints(); ++i)
        {
            if (i != 0)
                std::cout << ", ";
            Point p = exteriorRing.getPointN(i);
            std::cout << p.getX() << " " << p.getY();
        }
        std::cout << ")";

        for (auto iter = interiorRings.begin(); iter != interiorRings.end(); iter++)
        {
            LineString ring = *iter;
            if (ring.numPoints())
            {
                std::cout << ", (";
                for (size_t i = 0; i < ring.numPoints(); ++i)
                {
                    if (i != 0)
                        std::cout << ", ";
                    Point p = ring.getPointN(i);
                    std::cout << p.getX() << " " << p.getY();
                }
                std::cout << ")";
            }
        }

        std::cout << ")";
    }

} // namespace hw6
