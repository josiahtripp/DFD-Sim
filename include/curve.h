#ifndef CURVE_H
    #define CURVE_H

    #include <SDL3/SDL.h>
    #include <memory>
    #include <list>

    class Curve
    {

        private:
            // The list of points in the curve
            std::list<SDL_Point> _points;

            // Distance threshold
            const double _distance_threshold = 8;

            // Selected point is still valid
            bool _valid_selected = false;

            // Point selected to move
            std::list<SDL_Point>::iterator _move_selected;

            // Return an iterator to the closest point within the distance threshold
            // Return _points.end() if none is found
            std::list<SDL_Point>::iterator selection(int x,  int y);

            // Draw a point using the global renderer
            void drawPoint(const SDL_Rect* dst, SDL_Point* p, Uint8 r, Uint8 g, Uint8 b);

            // Draw a line using the global renderer
            void drawLine(const SDL_Rect* dst, SDL_Point* p1, SDL_Point* p2, Uint8 r, Uint8 g, Uint8 b);

            // Draw a highlighted point using the global renderer
            void drawHighlightedPoint(const SDL_Rect* dst, SDL_Point* p, Uint8 r, Uint8 g, Uint8 b);

            // Draw a highlighted point using the global renderer (but small)
            void drawHighlightedPointSmall(const SDL_Rect* dst, SDL_Point* p, Uint8 r, Uint8 g, Uint8 b);

            // Draw a highlighted line using the global renderer
            void drawHighlightedLine(const SDL_Rect* dst, SDL_Point* p1, SDL_Point* p2, Uint8 r, Uint8 g, Uint8 b);

            // Draw a highlighted x using the global renderer
            void drawCross(const SDL_Rect* dst, SDL_Point* p);

            // Draw the curve highlighted
            void drawHighlighted(const SDL_Rect* dst, int x, int y, Uint8 r, Uint8 g, Uint8 b);

        public:

            // Default and copy constructor
            Curve();
            Curve& operator=(const Curve& c);

            // Return the size of the curve in points
            int size();

            // Return a pointer to the specified point
            SDL_Point* getPoint(int idx);

            // Compute the L2 distance between two points
            static double pointL2(SDL_Point* p1, SDL_Point* p2);

            // Draw the curve using the global renderer, bounded by the given Frect
            void draw(const SDL_Rect* dst, Uint8 r, Uint8 g, Uint8 b);
            
            // Invalidate any selected move point
            void invalidateSelected();

            // Add points to the curve
            void addTail(const SDL_Rect* dst, int x, int y);
            void addHead(const SDL_Rect* dst, int x, int y);

            // Remove a point from the curve
            void remove(const SDL_Rect* dst, int x, int y);

            // Move a point from the curve
            void move(const SDL_Rect* dst, int x, int y);

            // Move the entire curve
            void offset(const SDL_Rect* dst, int x, int y);

            // Draw a higlighted line and point indicating a new line placement
            void drawNewTailHighlight(const SDL_Rect* dst, int x, int y, Uint8 r, Uint8 g, Uint8 b);
            void drawNewHeadHighlight(const SDL_Rect* dst, int x, int y, Uint8 r, Uint8 g, Uint8 b);
            void drawRemoveHighlight(const SDL_Rect* dst, int x, int y, Uint8 r, Uint8 g, Uint8 b);
            void drawMoveHighlight(const SDL_Rect* dst, int x, int y, Uint8 r, Uint8 g, Uint8 b);
            void drawOffsetHighlight(const SDL_Rect* dst, int x, int y, Uint8 r, Uint8 g, Uint8 b);
            
    };

#endif