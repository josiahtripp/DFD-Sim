#include "curve.h"
#include <limits>
#include <math.h>
#include "window.h"

/* Private Internal methods */

std::list<SDL_Point>::iterator Curve::selection(int x, int y)
{
    double distance = std::numeric_limits<double>::max();
    std::list<SDL_Point>::iterator closest_point;

    SDL_Point mouse_offset = {x, y};

    // Find the closest point in the curve
    for(auto it = _points.begin(); it != _points.end(); it++)
    {
        double mouse_dist = pointL2(&(*it), &mouse_offset);

        // Point is closer
        if(mouse_dist < distance)
        {
            distance = mouse_dist;
            closest_point = it;
        }
    }

    // Point is further than allowed selection distance threshold
    if(distance > this->_distance_threshold){
        return _points.end();
    }

    return closest_point;
}

void Curve::drawPoint(const SDL_Rect* dst, SDL_Point* p, Uint8 r, Uint8 g, Uint8 b)
{
    // Set the clipping area of the renderer
    SDL_SetRenderClipRect(global_renderer, dst);
    SDL_SetRenderDrawColor(global_renderer, r, g, b, SDL_ALPHA_OPAQUE);
    drawCircle(global_renderer, p->x + dst->x, p->y + dst->y, 4);
    SDL_SetRenderClipRect(global_renderer, NULL);
}

void Curve::drawLine(const SDL_Rect* dst, SDL_Point* p1, SDL_Point* p2, Uint8 r, Uint8 g, Uint8 b)
{
    // Set the clipping area of the renderer
    SDL_SetRenderClipRect(global_renderer, dst);
    SDL_SetRenderDrawColor(global_renderer, r, g, b, SDL_ALPHA_OPAQUE);
    SDL_RenderLine(global_renderer, p1->x + dst->x, p1->y + dst->y, p2->x + dst->x, p2->y + dst->y);
    SDL_SetRenderClipRect(global_renderer, NULL);
}

void Curve::drawHighlightedPoint(const SDL_Rect* dst, SDL_Point* p, Uint8 r, Uint8 g, Uint8 b)
{
    // Set the clipping area of the renderer
    SDL_SetRenderClipRect(global_renderer, dst);
    SDL_SetRenderDrawColor(global_renderer, r, g, b, 70);
    drawCircle(global_renderer, p->x + dst->x, p->y + dst->y, 8);
    SDL_SetRenderClipRect(global_renderer, NULL);
}

void Curve::drawHighlightedPointSmall(const SDL_Rect* dst, SDL_Point* p, Uint8 r, Uint8 g, Uint8 b)
{
    // Set the clipping area of the renderer
    SDL_SetRenderClipRect(global_renderer, dst);
    SDL_SetRenderDrawColor(global_renderer, r, g, b, 70);
    drawCircle(global_renderer, p->x + dst->x, p->y + dst->y, 4);
    SDL_SetRenderClipRect(global_renderer, NULL);
}

void Curve::drawHighlightedLine(const SDL_Rect* dst, SDL_Point* p1, SDL_Point* p2, Uint8 r, Uint8 g, Uint8 b)
{
    // Set the clipping area of the renderer
    SDL_SetRenderClipRect(global_renderer, dst);
    SDL_SetRenderDrawColor(global_renderer, r, g, b, 70);
    SDL_RenderLine(global_renderer, p1->x + dst->x, p1->y + dst->y, p2->x + dst->x, p2->y + dst->y);
    SDL_SetRenderClipRect(global_renderer, NULL);
}

void Curve::drawCross(const SDL_Rect* dst, SDL_Point* p)
{
    SDL_Point tl = {p->x - 8, p->y - 8};
    SDL_Point tr = {p->x + 8, p->y - 8};
    SDL_Point bl = {p->x - 8, p->y + 8};
    SDL_Point br = {p->x + 8, p->y + 8};

    drawLine(dst, &tl, &br, 0, 0, 0);
    drawLine(dst, &tr, &bl, 0, 0, 0);
}

void Curve::drawHighlighted(const SDL_Rect* dst, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
    SDL_Point op1;
    SDL_Point op2;

    // No points in curve, nothing to draw
    if(this->_points.size() < 1)
    {
        return;
    }

    // Iterator to the first point
    auto p1 = this->_points.begin();

    // Only one point, draw vertex dot in color
    if(this->_points.size() == 1)
    {
        op1 = *p1;
        op1.x += x;
        op1.y += y;
        this->drawHighlightedPointSmall(dst, &op1, r, g, b);
        return;
    }

    // Iterator to the second point
    auto p2 = p1;
    p2++;

    // Iterate through all curve edges
    for(p1, p2; p2 != this->_points.end(); p1++, p2++)
    {
        op1 = *p1;
        op1.x += x;
        op1.y += y;

        op2 = *p2;
        op2.x += x;
        op2.y += y;
        this->drawHighlightedLine(dst, &op1, &op2, r, g, b);
    }

    // Draw the first vertex dot in black
    p1 = _points.begin();
    op1 = *p1;
    op1.x += x;
    op1.y += y;
    this->drawHighlightedPointSmall(dst, &op1, 0, 0, 0);
    p1++;

    for(p1; std::next(p1) != this->_points.end(); p1++)
    {
        // Draw internal points in the curve color
        op1 = *p1;
        op1.x += x;
        op1.y += y;
        this->drawHighlightedPointSmall(dst, &op1, r, g, b);
    }

    // Draw the last vertex dot in yellow
    op1 = *p1;
    op1.x += x;
    op1.y += y;
    this->drawHighlightedPointSmall(dst, &op1, 255, 242, 0);
}

/* Public methods */
Curve::Curve()
{

}

Curve &Curve::operator=(const Curve& c)
{   
    // Copy points list
    _points = c._points;

    _move_selected = c._move_selected;

    _valid_selected = c._valid_selected;

    return *this;
}

int Curve::size()
{
    return this->_points.size();
}

SDL_Point* Curve::getPoint(int idx)
{
    return &(*std::next(_points.begin(), idx));
}

double Curve::pointL2(SDL_Point* p1, SDL_Point* p2)
{
    return sqrt(((p1->x - p2->x) * (p1->x - p2->x)) + ((p1->y - p2->y) * (p1->y - p2->y)));
}

void Curve::draw(const SDL_Rect* dst, Uint8 r, Uint8 g, Uint8 b)
{
    // No points in curve, nothing to draw
    if(this->_points.size() < 1)
    {
        return;
    }

    // Iterator to the first point
    auto p1 = this->_points.begin();

    // Only one point, draw vertex dot in color
    if(this->_points.size() == 1)
    {
        this->drawPoint(dst, &(*p1), r, g, b);
        return;
    }

    // Iterator to the second point
    auto p2 = p1;
    p2++;

    // Iterate through all curve edges
    for(p1, p2; p2 != this->_points.end(); p1++, p2++)
    {
        // Draw internal lines in the curve color
        this->drawLine(dst, &(*p1), &(*p2), r, g, b);
    }

    // Draw the first vertex dot in black
    p1 = _points.begin();
    this->drawPoint(dst, &(*p1), 0, 0, 0);
    p1++;

    for(p1; std::next(p1) != this->_points.end(); p1++)
    {
        // Draw internal lines in the curve color
        this->drawPoint(dst, &(*p1), r, g, b);
    }

    // Draw the last vertex dot in yellow
    this->drawPoint(dst, &(*p1), 255, 242, 0);
}

void Curve::invalidateSelected()
{
    _valid_selected = false;
}

void Curve::addTail(const SDL_Rect* dst, int x, int y)
{
    _points.push_back({x - dst->x, y - dst->y});
}

void Curve::addHead(const SDL_Rect* dst, int x, int y)
{
    _points.push_front({x - dst->x, y - dst->y});
}

void Curve::remove(const SDL_Rect* dst, int x, int y)
{
    std::list<SDL_Point>::iterator s = this->selection(x - dst->x, y - dst->y);

    // No valid selection
    if(s == this->_points.end())
    {
        return;
    }

    // Remove the point referenced by the iterator
    this->_points.erase(s);
}

void Curve::move(const SDL_Rect* dst, int x, int y)
{
    // Get the selected point if need be
    if(!_valid_selected)
    {
        _move_selected = this->selection(x - dst->x, y - dst->y);

        // No valid selection
        if(_move_selected == this->_points.end())
        {
            return;
        }

        // Set valid and return, next click will be destination of point
        _valid_selected = true;
        return;
    }

    SDL_Point mouse = {x - dst->x, y - dst->y};

    // Mouse is within threshold distance of selected point
    if(pointL2(&(*_move_selected), &mouse) < this->_distance_threshold)
    {
        // Deselect point and return
        _valid_selected = false;
        return;
    }

    // Update the point location
    _move_selected->x = x - dst->x;
    _move_selected->y = y - dst->y; 
    _valid_selected = false;
}

void Curve::offset(const SDL_Rect* dst, int x, int y)
{
    // Get the selected reference point if need be
    if(!_valid_selected)
    {
        _move_selected = this->selection(x - dst->x, y - dst->y);

        // No valid selection
        if(_move_selected == this->_points.end())
        {
            return;
        }

        // Set valid and return, next click will be destination of point
        _valid_selected = true;
        return;
    }

    SDL_Point mouse = {x - dst->x, y - dst->y};

    // Mouse is within threshold distance of selected point
    if(pointL2(&(*_move_selected), &mouse) < this->_distance_threshold)
    {
        // Deselect point and return
        _valid_selected = false;
        return;
    }

    // Calculate frame bound
    SDL_Rect fb = *dst;
    fb.x = 0;
    fb.y = 0;

    // Calculate delta from _move_selected
    SDL_Point delta = {mouse.x - _move_selected->x, mouse.y - _move_selected->y};

    // Ensure each point is within the bounds of the given frame
    auto it = _points.begin();
    for(it; it != _points.end(); it++)
    {
        SDL_Point p = {it->x + delta.x, it->y + delta.y};
        if(!SDL_PointInRect(&p, &fb))
        {
            return;
        }
    }

    it = _points.begin();
    for(it; it != _points.end(); it++)
    {
        it->x += delta.x;
        it->y += delta.y;
    }
    _valid_selected = false;
}

void Curve::drawNewTailHighlight(const SDL_Rect* dst, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
    SDL_Point p = {x - dst->x, y - dst->y};

    // No points yet, draw in curve color
    if(this->_points.size() < 1)
    {
        this->drawPoint(dst, &p, r, g, b);
        return;
    }

    // Draw the tail point in yellow
    this->drawHighlightedPoint(dst, &p, 255, 242, 0);

    SDL_Point* e = &(*std::prev(_points.end()));

    // Draw the line in curve color
    this->drawHighlightedLine(dst, &p, e, r, g, b);
}

void Curve::drawNewHeadHighlight(const SDL_Rect* dst, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
    SDL_Point p = {x - dst->x, y - dst->y};

    // No points yet, draw in curve color
    if(this->_points.size() < 1)
    {
        this->drawPoint(dst, &p, r, g, b);
        return;
    }

    // Draw the tail point in yellow
    this->drawHighlightedPoint(dst, &p, 0, 0, 0);

    SDL_Point* s = &(*_points.begin());

    // Draw the line in curve color
    this->drawHighlightedLine(dst, s, &p, r, g, b);
}

void Curve::drawRemoveHighlight(const SDL_Rect* dst, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
    std::list<SDL_Point>::iterator s = this->selection(x - dst->x, y - dst->y);

    // No valid selection
    if(s == this->_points.end())
    {
        return;
    }

    // Highlight the point to be deleted
    this->drawCross(dst, &(*s));

    // No new highlighted edge to be drawn
    if(this->_points.size() < 3)
    {
        return;
    }

    // Selected point is head
    if(s == this->_points.begin())
    {
        return;
    }

    // Selected point is tail
    if(s == std::prev(this->_points.end()))
    {
        return;
    }

    // Get the neighbors of the selected point
    auto n1 = s;
    n1++;
    auto n2 = s;
    n2--;

    // Draw a highlighted line between the neighbors
    this->drawHighlightedLine(dst, &(*n1), &(*n2), r, g, b);
}

void Curve::drawMoveHighlight(const SDL_Rect* dst, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
    SDL_Point mouse = {x - dst->x, y - dst->y};

    if(_valid_selected)
    {
        bool on_selected = false;
        
        if(pointL2(&(*_move_selected), &mouse) < this->_distance_threshold)
        {
            on_selected = true;
        }

        // Draw in color (sole point)
        if(_points.size() < 2)
        {
            this->drawHighlightedPointSmall(dst, &mouse, r, g, b);
            if(on_selected) this->drawHighlightedPoint(dst, &(*_move_selected), r, g, b);
            return;
        }

        // Draw in black (first of multiple)
        if(_move_selected == _points.begin())
        {
            this->drawHighlightedLine(dst, &mouse, &(*std::next(_points.begin())), r, g, b);
            this->drawHighlightedPointSmall(dst, &mouse, 0, 0, 0);
            if(on_selected) this->drawHighlightedPoint(dst, &(*_move_selected), 0, 0, 0);
            return;
        }
 
        this->drawHighlightedLine(dst, &mouse, &(*std::prev(_move_selected)), r, g, b);

        // Draw in yellow (last of multiple)
        if(std::next(_move_selected) == _points.end())
        {   
            this->drawHighlightedPointSmall(dst, &mouse, 255, 242, 0);
            if(on_selected) this->drawHighlightedPoint(dst, &(*_move_selected), 255, 242, 0);
            return;
        }

        this->drawHighlightedPointSmall(dst, &mouse, r, g, b);
        this->drawHighlightedLine(dst, &mouse, &(*std::next(_move_selected)), r, g, b);
        if(on_selected) this->drawHighlightedPoint(dst, &(*_move_selected), r, g, b);
    }
    else
    {   
        // Draw the (to be) selected point
        auto tmp = this->selection(x - dst->x, y - dst->y);

        if(tmp != _points.end())
        {
            // Draw in color (sole point)
            if(_points.size() < 2)
            {
                this->drawHighlightedPoint(dst, &(*tmp), r, g, b);
                return;
            }

            // Draw in black (first of multiple)
            if(tmp == _points.begin())
            {
                this->drawHighlightedPoint(dst, &(*tmp), 0, 0, 0);
                return;
            }

            // Draw in yellow (last of multiple)
            if(std::next(tmp) == _points.end())
            {
                this->drawHighlightedPoint(dst, &(*tmp), 255, 242, 0);
                return;
            }

            this->drawHighlightedPoint(dst, &(*tmp), r, g, b);
        }
    }
}

void Curve::drawOffsetHighlight(const SDL_Rect* dst, int x, int y, Uint8 r, Uint8 g, Uint8 b)
{
    // Mouse position, relative to dst (top left corner origin)
    SDL_Point mouse_normal = {x - dst->x, y - dst->y};

    // Calculate frame bound (valid space for points)
    SDL_Rect fb;
    fb.x = 0;
    fb.y = 0;
    fb.w = dst->w;
    fb.h = dst->h;

    // Reference point has been selected
    if(_valid_selected)
    {
        // Mouse is within threshold distance of selected point
        if(pointL2(&(*_move_selected), &mouse_normal) < this->_distance_threshold)
        {
            if(_points.size() < 2)
            {
                drawHighlightedPoint(dst, &(*_move_selected), r, g, b);
            }

            // Selected point is first of multiple, draw it in black
            else if(_move_selected == _points.begin())
            {
                drawHighlightedPoint(dst, &(*_move_selected), 0, 0, 0);
            }

            // Selected point is last of multiple, draw it in yellow
            else if(std::next(_move_selected) == _points.end())
            {
                drawHighlightedPoint(dst, &(*_move_selected), 255, 242, 0);
            }

            // Selected point is inner of multiple, draw it in curve color
            else
            {
                drawHighlightedPoint(dst, &(*_move_selected), r, g, b);
            }
        }

        // Calculate delta from _move_selected
        SDL_Point delta = {mouse_normal.x - _move_selected->x, mouse_normal.y - _move_selected->y};

        // Ensure each point is within the bounds of the given frame
        auto it = _points.begin();
        for(it; it != _points.end(); it++)
        {
            // Create the new point from the given offset
            SDL_Point p = {it->x + delta.x, it->y + delta.y};
            if(!SDL_PointInRect(&p, &fb))
            {
                return;
            }
        }

        drawHighlighted(dst, delta.x, delta.y, r, g, b);
        return;
    }

    // Get a temporary iterator to a possible selection point
    auto tmp = selection(mouse_normal.x, mouse_normal.y);

    // No valid selection point, don't draw anything and return
    if(tmp == _points.end())
    {
        return;
    }

    // Sole point, draw it in the curve color and return
    else if(_points.size() < 2)
    {
        drawHighlightedPoint(dst, &(*tmp), r, g, b);
        return;
    }

    // First point of multiple, draw it in black and return
    else if(tmp == _points.begin())
    {
        drawHighlightedPoint(dst, &(*tmp), 0, 0, 0);
        return;
    }

    // Last of multiple, draw it in yellow and return
    else if(std::next(tmp) == _points.end())
    {
        drawHighlightedPoint(dst, &(*tmp), 255, 242, 0);
        return;
    }

    // Inner of multiple, draw in curve color and return;
    drawHighlightedPoint(dst, &(*tmp), r, g, b);
    return;
}