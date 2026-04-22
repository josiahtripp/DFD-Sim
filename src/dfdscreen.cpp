#include "dfdscreen.h"
#include <SDL3_image/SDL_image.h>
#include <stdexcept>
#include <algorithm>
#include <iostream>

DfdScreen::DfdScreen(Curve* c1, Curve* c2) : BaseScreen(900, 50, 800, 895, "../assets/dfdscreen/")
{
    // Set the references to the external curves
    curve_1 = c1;
    curve_2 = c2;

    // Load the required textures and set their location frects
    loadTextures();

    // Set the internal curves with a copy of the references
    refreshCurves();
}

void DfdScreen::refreshCurves()
{
    // Neither curve is null, copy
    if(curve_1 && curve_2)
    {
        current_c1 = *curve_1;
        current_c2 = *curve_2;
    }

    // Reset any step of the algorithm
    resetAlgorithm(true);

    // Set the current control mode to STOP
    _control_mode = MODE_STOP;
}

void DfdScreen::resetAlgorithm(bool curves_refreshed)
{
    // Possibly new curve lengths, reallocate tabulation matrix
    if(curves_refreshed)
    {
        if(current_c1.size() > 0 && current_c2.size() > 0)
        {
            tabulation_matrix = std::make_unique<std::unique_ptr<double[]>[]>(current_c1.size());

            for (int i = 0; i < current_c1.size(); i++)
            {
                tabulation_matrix[i] = std::make_unique<double[]>(current_c2.size());
            }
        }
    }

    // Reset the last update time
    this->_last_update = 0;

    // Reset to tabulation stage
    _alg_stage = STAGE_TABULATION;

    // Set calculated frechet distance to invalid
    _frechet_distance = -1.0;

    // Set drawing points to invalid
    _alg_p1 = nullptr;
    _alg_p2 = nullptr;
}

void DfdScreen::updateAlgorithm()
{
    // Draw the calculated frechet distance (if valid)
    SDL_SetRenderDrawColor(global_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_SetRenderScale(global_renderer, 3.0, 3.0);
    if(_frechet_distance >= 0)
    {
        SDL_RenderDebugTextFormat(global_renderer, 160, 283, "F.D: %.2f", _frechet_distance);
    }
    else
    {
        SDL_RenderDebugTextFormat(global_renderer, 160, 283, "F.D: _______");
    }
    SDL_SetRenderScale(global_renderer, 1.0, 1.0);

    // No valid curves to calculate
    if(current_c1.size() <= 0 || current_c2.size() <= 0)
    {
        return;
    }

    // Algorithm control mode is set to play
    if(_control_mode == MODE_PLAY)
    {
        // Delay update interval has passed
        if(SDL_GetTicks() - _delay_interval > _last_update)
        {
            this->stepAlgorithm();

            // Update last handled update time
            this->_last_update = SDL_GetTicks();
        }
    }

    else if(_control_mode == MODE_PAUSE)
    {
        if(this->_last_update > 0)
        {
            this->_last_update = SDL_GetTicks();
        }
    }

    else if(_control_mode == MODE_STOP)
    {   
        // Do not draw anything
        return;
    }

    // Both drawn points are valid
    if(_alg_p1 != nullptr && _alg_p2 != nullptr)
    {
        // Determine offsets
        int x_off = editor_window_inner_frect.x;
        int y_off = editor_window_inner_frect.y;

        // Draw the algorithm points
        SDL_SetRenderDrawColor(global_renderer, _alg_r, _alg_g, _alg_b, SDL_ALPHA_OPAQUE);
        drawCircle(global_renderer, _alg_p1->x + x_off, _alg_p1->y + y_off, 5);
        drawCircle(global_renderer, _alg_p2->x + x_off, _alg_p2->y + y_off, 5);

        // Draw the algorithm line segment
        SDL_RenderLine(global_renderer, _alg_p1->x + x_off, _alg_p1->y + y_off, 
                            _alg_p2->x + x_off, _alg_p2->y + y_off);
    }
}

void DfdScreen::stepAlgorithm()
{
    // Tabulation step of algorithm
    if(_alg_stage == STAGE_TABULATION)
    {
        // First step of the algorithm
        if(this->_last_update == 0)
        {
            // Reset algorithm indexes
            _alg_c1_idx = current_c1.size() - 1;
            _alg_c2_idx = current_c2.size() - 1;

            // At least one point in each curve
            if(current_c1.size() > 0 && current_c2.size() > 0)
            {
                // Set end corner value (to distance between endpoints)
                tabulation_matrix[_alg_c1_idx][_alg_c2_idx]
                    = Curve::pointL2(current_c1.getPoint(_alg_c1_idx), current_c2.getPoint(_alg_c2_idx));
            }

            // Reset highlighted line color
            _alg_r = 255;
            _alg_g = 140;
            _alg_b = 0;
        }

        // Next step in tabulation (Not first)
        else
        {

            // Get the distance between the current points
            double current_dist 
                = Curve::pointL2(current_c1.getPoint(_alg_c1_idx), current_c2.getPoint(_alg_c2_idx));

            // Cell is of the bottom row
            if(_alg_c1_idx == (current_c1.size() - 1))
            {
                // Cell value is max of current distance and right neighbor
                tabulation_matrix[_alg_c1_idx][_alg_c2_idx] 
                    = std::max(current_dist, tabulation_matrix[_alg_c1_idx][_alg_c2_idx + 1]);
            }

            // Cell is of the rightmost column
            else if(_alg_c2_idx == (current_c2.size() - 1))
            {
                // Cell value is max of current distance and lower neighbor
                tabulation_matrix[_alg_c1_idx][_alg_c2_idx] 
                    = std::max(current_dist, tabulation_matrix[_alg_c1_idx + 1][_alg_c2_idx]);
            }

            // Cell has 3 neighbors
            else
            {
                // Calculate min of all 3 neighbors
                double neighbor_min
                = std::min(
                           std::min(tabulation_matrix[_alg_c1_idx][_alg_c2_idx + 1],
                                     tabulation_matrix[_alg_c1_idx + 1][_alg_c2_idx]),
                           tabulation_matrix[_alg_c1_idx + 1][_alg_c2_idx + 1]);

                // Cell value is max of current distance and (min of all 3 neighbors)
                tabulation_matrix[_alg_c1_idx][_alg_c2_idx] 
                    = std::max(current_dist, neighbor_min);
            }
        }

        // At least one point in each curve
        if(current_c1.size() > 0 && current_c2.size() > 0)
        {
            // Set SDL Point pointers for drawing
            _alg_p1 = current_c1.getPoint(_alg_c1_idx);
            _alg_p2 = current_c2.getPoint(_alg_c2_idx);
        }

        // Last step of tabulation stage
        if(_alg_c1_idx == 0 && _alg_c2_idx == 0)
        {
            // Move to next stage in algorithm
            _alg_stage = STAGE_DISPLAY;
        }

        // Move to next cell of tabulation stage (for next call)
        else
        {   
            // Move left on curve 2
            _alg_c2_idx--;

            // Moved out of bounds on curve 2
            if(_alg_c2_idx < 0)
            {   
                // Move up a row on curve 1
                _alg_c1_idx--;

                // Move curve 2 to rightmost column
                _alg_c2_idx = (current_c2.size() - 1);
            }
        }
    }

    // Display stage of algorithm
    else
    {
        // Start at 0, 0
        if(current_c1.size() <= 0 || current_c2.size() <= 0)
        {
            return;
        }

        // Set path display color (green)
        _alg_r = 29;
        _alg_g = 222;
        _alg_b = 55;

        // Display stage has looped, reset
        if(_alg_c1_idx == (current_c1.size() - 1) && (_alg_c2_idx  == current_c2.size() - 1))
        {
            _alg_c1_idx = 0;
            _alg_c2_idx = 0;
            _frechet_distance = -1; // Just to trigger reset
        }

        // First step of display stage, get frechet distance
        if(_frechet_distance < 0)
        {
            _frechet_distance = tabulation_matrix[0][0];
        }
        // Next step of display stage
        else
        {
            // In rightmost column, can only move down
            if(_alg_c2_idx == (current_c2.size() - 1))
            {
                _alg_c1_idx++;
            }

            // In lowest row, can only move right
            else if(_alg_c1_idx == (current_c1.size() - 1))
            {
                _alg_c2_idx++;
            }

            // Can move to all 3, try all 3
            else
            {
                // Get path distances
                double left = tabulation_matrix[_alg_c1_idx][_alg_c2_idx + 1];
                double down = tabulation_matrix[_alg_c1_idx + 1][_alg_c2_idx];
                double diag = tabulation_matrix[_alg_c1_idx + 1][_alg_c2_idx + 1];

                // Diagonal is optimal
                if(diag <= left && diag <= down)
                {
                    _alg_c1_idx++;
                    _alg_c2_idx++;
                }

                // Left is optimal
                else if(left <= diag && left <= down)
                {
                    _alg_c2_idx++;
                }

                // Down is optimal
                else
                {
                    _alg_c1_idx++;
                }
            }
        }

        // Set SDL Point pointers for drawing
        _alg_p1 = current_c1.getPoint(_alg_c1_idx);
        _alg_p2 = current_c2.getPoint(_alg_c2_idx);

        // Current location is max frechet distance, highlight in purple
        if(Curve::pointL2(_alg_p1, _alg_p2) == _frechet_distance)
        {
            _alg_r = 174;
            _alg_g = 0;
            _alg_b = 255;
        }
    }
}

void DfdScreen::loadTextures()
{
    // Load the editor mode button textures and associated SDL Frect structs
    mode_textures[MODE_PLAY] = 
        IMG_LoadTexture(global_renderer, getAssetFilepath("play.png").c_str());
    mode_texture_frects[MODE_PLAY] = 
        {125, 825, (float) mode_textures[MODE_PLAY]->w, (float) mode_textures[MODE_PLAY]->h};

    mode_textures[MODE_STOP] = 
        IMG_LoadTexture(global_renderer, getAssetFilepath("stop.png").c_str());
    mode_texture_frects[MODE_STOP] = 
        {355, 825, (float) mode_textures[MODE_STOP]->w, (float) mode_textures[MODE_STOP]->h};

    mode_textures[MODE_PAUSE] = 
        IMG_LoadTexture(global_renderer, getAssetFilepath("pause.png").c_str());
    mode_texture_frects[MODE_PAUSE] = 
        {240, 825, (float) mode_textures[MODE_PAUSE]->w, (float) mode_textures[MODE_PAUSE]->h};


    // Load the refresh button texture
    refresh_button_texture =  
        IMG_LoadTexture(global_renderer, getAssetFilepath("refresh.png").c_str());
    refresh_button_texture_frect = 
        {0, 825, (float) refresh_button_texture->w, (float) refresh_button_texture->h};

        
    // Load the button selected and hovering texture
    button_selected_texture = 
        IMG_LoadTexture(global_renderer, getAssetFilepath("selected_button_texture.png").c_str());
    button_selected_texture_frect = 
        {0, 0, (float) button_selected_texture->w, (float) button_selected_texture->h};

    hovering_button_texture = 
        IMG_LoadTexture(global_renderer, getAssetFilepath("hovering_button_texture.png").c_str());
    hovering_button_texture_frect = 
        {0, 0, (float) hovering_button_texture->w, (float) hovering_button_texture->h};
}

int DfdScreen::isHoveringControlModeButton()
{
    // Get the current global mouse position
    float x, y;
    SDL_GetMouseState(&x, &y);
    SDL_FPoint mouse = {x, y};

    // Subtract the offset of the screen positon
    mouse.x -= (float) this->x();
    mouse.y -= (float) this->y();

    // Iterate through all of the editor mode buttons
    for(int i = 0; i < MODE_COUNT; i++)
    {
        // Check for intersection
        if(SDL_PointInRectFloat(&mouse, &mode_texture_frects[i]))
        {
            return i;
        }
    }

    return -1;
}

bool DfdScreen::isHoveringRefreshButton()
{
    // Get the current global mouse position
    float x, y;
    SDL_GetMouseState(&x, &y);
    SDL_FPoint mouse = {x, y};

    // Subtract the offset of the screen positon
    mouse.x -= (float) this->x();
    mouse.y -= (float) this->y();

    // Check for intersection
    if(SDL_PointInRectFloat(&mouse, &refresh_button_texture_frect))
    {
        return true;
    }

    return false;
}

bool DfdScreen::drawHoveringControlModeButton()
{
    // Check if the mouse is hovering over any of the editor mode buttons
    int mode;
    if((mode = isHoveringControlModeButton()) != -1)
    {
        // Don't highlight if the button is already selected
        if(mode == _control_mode)
        {
            return true;
        }

        // Draw the hovering button texture
        hovering_button_texture_frect.x = mode_texture_frects[mode].x;
        hovering_button_texture_frect.y = mode_texture_frects[mode].y;
        SDL_RenderTexture(global_renderer, hovering_button_texture, NULL, &hovering_button_texture_frect);
        
        return true;
    }

    return false;
}

bool DfdScreen::drawHoveringRefreshButton()
{
    // Check if the mouse is hovering over the refresh button
    if(isHoveringRefreshButton())
    {
        // Draw the hovering button texture
        hovering_button_texture_frect.x = refresh_button_texture_frect.x;
        hovering_button_texture_frect.y = refresh_button_texture_frect.y;
        SDL_RenderTexture(global_renderer, hovering_button_texture, NULL, &hovering_button_texture_frect);
        
        return true;
    }

    return false;
}

bool DfdScreen::updateControlModeButtonsClicked()
{
    // Check if the mouse is hovering over a control mode button
    int mode;
    if((mode = isHoveringControlModeButton()) != -1)
    {
        _control_mode = (enum DfdScreen::control_mode) mode;

        // Stop button pressed, reset the algorithm
        if(_control_mode == MODE_STOP)
        {
            resetAlgorithm(false);
        }

        return true;
    }

    return false;
}

bool DfdScreen::updateRefreshButtonClicked()
{
    // Check if the mouse is hovering over the refresh button
    if(isHoveringRefreshButton())
    {   
        refreshCurves();
        return true;
    }

    return false;
}

void DfdScreen::renderControlModeButtons()
{
    // Draw the editor mode buttons onto the screen
    SDL_RenderTexture(global_renderer, mode_textures[MODE_PLAY], NULL, &mode_texture_frects[MODE_PLAY]);
    SDL_RenderTexture(global_renderer, mode_textures[MODE_STOP], NULL, &mode_texture_frects[MODE_STOP]);
    SDL_RenderTexture(global_renderer, mode_textures[MODE_PAUSE], NULL, &mode_texture_frects[MODE_PAUSE]);

    // Indicate the selected editor mode button
    button_selected_texture_frect.x = mode_texture_frects[_control_mode].x;
    button_selected_texture_frect.y = mode_texture_frects[_control_mode].y;
    SDL_RenderTexture(global_renderer, button_selected_texture, NULL, &button_selected_texture_frect);
}

void DfdScreen::renderRefreshButton()
{
    // Draw the refresh button
    SDL_RenderTexture(global_renderer, refresh_button_texture, NULL, &refresh_button_texture_frect);

    /*
    button_selected_texture_frect.x = refresh_button_texture_frect.x;
    button_selected_texture_frect.y = refresh_button_texture_frect.y;
    SDL_RenderTexture(global_renderer, button_selected_texture, NULL, &button_selected_texture_frect);
    */
}

void DfdScreen::onHoverEvent(SDL_Event *event)
{
    // A mouse button has been pressed
    if(event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        // Left mouse button
        if(event->button.button == SDL_BUTTON_LEFT)
        {
            // Check is a control mode button has been clicked
            if(updateControlModeButtonsClicked())
            {

            }
            // Check if the refresh button has been clicked
            else if(updateRefreshButtonClicked())
            {

            }
        }
    }
}

void DfdScreen::onUpdate()
{
    // Clear the background with white
    SDL_SetRenderDrawColor(global_renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(global_renderer);

    // Draw the editor window square outline in black
    SDL_SetRenderDrawColor(global_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(global_renderer, &editor_window_outer_frect);

    // Draw the editor window 
    SDL_SetRenderDrawColor(global_renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(global_renderer, &editor_window_inner_frect);

    // Draw both curves in the editor window
    current_c1.draw(&editor_window_inner_rect, 237, 28, 36);
    current_c2.draw(&editor_window_inner_rect, 0, 162, 232);

    // Draw the separator line between the editor mode and curve selection buttons, in black
    SDL_SetRenderDrawColor(global_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderLine(global_renderer, 112, 825, 112, 895);
    SDL_RenderLine(global_renderer, 468, 825, 468, 895);

    // Draw the editor mode buttons
    this->renderControlModeButtons();

    // Draw the curve selection buttons
    this->renderRefreshButton();

    // Update the algorithm
    this->updateAlgorithm();
}

void DfdScreen::onHoverUpdate()
{
    // Update normally
    this->onUpdate();

    // Check for any editor mode button hovering and draw if so
    if(this->drawHoveringControlModeButton())
    {
        return;
    }
    // Check for any curve selection button hovering and draw if so
    else if(this->drawHoveringRefreshButton())
    {
        return;
    }
}

DfdScreen::~DfdScreen()
{
    // Free the control mode button textures
    SDL_DestroyTexture(mode_textures[MODE_PLAY]);
    SDL_DestroyTexture(mode_textures[MODE_STOP]);
    SDL_DestroyTexture(mode_textures[MODE_PAUSE]);

    // Free the selected button texture
    SDL_DestroyTexture(button_selected_texture);

    // Free the hovering button texture
    SDL_DestroyTexture(hovering_button_texture);

    // Free the reset button texture
    SDL_DestroyTexture(refresh_button_texture);
}