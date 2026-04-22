#include "curveeditorscreen.h"
#include <SDL3_image/SDL_image.h>
#include <stdexcept>
#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <utility>

// Set the size and location of the curve editor screen
CurveEditorScreen::CurveEditorScreen() : BaseScreen(50, 50, 800, 950, "../assets/curveeditorscreen/")
{
    // Load required textures
    this->loadTextures();

    // Set the initial curve selection to curve 1
    curve_selection = CURVE_SELECTED_1;

    // Set the initial editor mode to add tail
    editor_mode = MODE_TAIL;
}

void CurveEditorScreen::loadTextures()
{
    // Load the editor mode button textures and associated SDL Frect structs
    mode_textures[MODE_TAIL] = 
        IMG_LoadTexture(global_renderer, getAssetFilepath("mode_tail_button.png").c_str());
    mode_texture_frects[MODE_TAIL] = 
        {240, 825, (float) mode_textures[MODE_TAIL]->w, (float) mode_textures[MODE_TAIL]->h};

    mode_textures[MODE_HEAD] = 
        IMG_LoadTexture(global_renderer, getAssetFilepath("mode_head_button.png").c_str());
    mode_texture_frects[MODE_HEAD] = 
        {355, 825, (float) mode_textures[MODE_HEAD]->w, (float) mode_textures[MODE_HEAD]->h};

    mode_textures[MODE_DELETE] = 
        IMG_LoadTexture(global_renderer, getAssetFilepath("mode_delete_button.png").c_str());
    mode_texture_frects[MODE_DELETE] = 
        {470, 825, (float) mode_textures[MODE_DELETE]->w, (float) mode_textures[MODE_DELETE]->h};

    mode_textures[MODE_MOVE] = 
        IMG_LoadTexture(global_renderer, getAssetFilepath("mode_move_button.png").c_str());
    mode_texture_frects[MODE_MOVE] = 
        {585, 825, (float) mode_textures[MODE_MOVE]->w, (float) mode_textures[MODE_MOVE]->h};
    
    mode_textures[MODE_OFFSET] = 
        IMG_LoadTexture(global_renderer, getAssetFilepath("mode_offset_button.png").c_str());
    mode_texture_frects[MODE_OFFSET] = 
        {700, 825, (float) mode_textures[MODE_OFFSET]->w, (float) mode_textures[MODE_OFFSET]->h};


    // Load the curve selection button textures
    curve_selected_textures[CURVE_SELECTED_1] = 
        IMG_LoadTexture(global_renderer, getAssetFilepath("selected_curve_one.png").c_str());
    curve_selected_texture_frects[CURVE_SELECTED_1] = 
        {0, 825, (float) curve_selected_textures[CURVE_SELECTED_1]->w, (float) curve_selected_textures[CURVE_SELECTED_1]->h};

    curve_selected_textures[CURVE_SELECTED_2] = 
        IMG_LoadTexture(global_renderer, getAssetFilepath("selected_curve_two.png").c_str());
    curve_selected_texture_frects[CURVE_SELECTED_2] = 
        {115, 825, (float) curve_selected_textures[CURVE_SELECTED_2]->w, (float) curve_selected_textures[CURVE_SELECTED_2]->h};

        
    // Load the button selected and hovering texture
    button_selected_texture = 
        IMG_LoadTexture(global_renderer, getAssetFilepath("selected_button_texture.png").c_str());
    button_selected_texture_frect = 
        {0, 0, (float) button_selected_texture->w, (float) button_selected_texture->h};

    hovering_button_texture = 
        IMG_LoadTexture(global_renderer, getAssetFilepath("hovering_button_texture.png").c_str());
    hovering_button_texture_frect = 
        {0, 0, (float) hovering_button_texture->w, (float) hovering_button_texture->h};

    // Load the terminal input button
    terminal_input_button_texture = 
        IMG_LoadTexture(global_renderer, getAssetFilepath("terminal_button.png").c_str());
    terminal_input_button_texture_frect = 
        {240, 905, (float) terminal_input_button_texture->w, (float) terminal_input_button_texture->h};
    hovering_terminal_input_button_texture =
        IMG_LoadTexture(global_renderer, getAssetFilepath("hovering_terminal_button.png").c_str());
}

int CurveEditorScreen::isHoveringEditorModeButton()
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

int CurveEditorScreen::isHoveringCurveSelectionButton()
{
    // Get the current global mouse position
    float x, y;
    SDL_GetMouseState(&x, &y);
    SDL_FPoint mouse = {x, y};

    // Subtract the offset of the screen positon
    mouse.x -= (float) this->x();
    mouse.y -= (float) this->y();

    // Iterate through all of the curve selection buttons
    for(int i = 0; i < CURVE_SELECTED_COUNT; i++)
    {
        // Check for intersection
        if(SDL_PointInRectFloat(&mouse, &curve_selected_texture_frects[i]))
        {
            return i;
        }
    }

    return -1;
}

bool CurveEditorScreen::drawHoveringEditorModeButton()
{
    // Check if the mouse is hovering over any of the editor mode buttons
    int mode;
    if((mode = isHoveringEditorModeButton()) != -1)
    {
        // Don't highlight if the button is already selected
        if(mode == editor_mode)
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

bool CurveEditorScreen::drawHoveringCurveSelectionButton()
{
    // Check if the mouse is hovering over any of the editor mode buttons
    int selection;
    if((selection = isHoveringCurveSelectionButton()) != -1)
    {
        // Don't highlight if the button is already selected
        if(selection == curve_selection)
        {
            return true;
        }

        // Draw the hovering button texture
        hovering_button_texture_frect.x = curve_selected_texture_frects[selection].x;
        hovering_button_texture_frect.y = curve_selected_texture_frects[selection].y;
        SDL_RenderTexture(global_renderer, hovering_button_texture, NULL, &hovering_button_texture_frect);
        
        return true;
    }

    return false;
}

bool CurveEditorScreen::drawHoveringEditorWindow()
{
    // Get mouse position
    SDL_FPoint mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);
    mouse.x -= x();
    mouse.y -= y();

    // Draw mouse coordinates at the bottom of the screen
    if(SDL_PointInRectFloat(&mouse, &editor_window_inner_frect))
    {
        SDL_SetRenderDrawColor(global_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_SetRenderScale(global_renderer, 2.0, 2.0);
        SDL_RenderDebugTextFormat(global_renderer, 179, 458, "Point: (%d, %d)", 
                                    (int) (mouse.x - editor_window_inner_frect.x),
                                    (int) (mouse.y - editor_window_inner_frect.y));
        SDL_SetRenderScale(global_renderer, 1.0, 1.0);
    }

    // Curve to modify
    Curve* c;
    Uint8 r, g, b;

    // Check if mouse is in inner editor window
    if(SDL_PointInRectFloat(&mouse, &editor_window_inner_frect))
    {
        // Determine curve to modify
        if(curve_selection == CURVE_SELECTED_1)
        {
            c = &curve_1;
            r = 237;
            g = 28;
            b = 36;
        }
        else if(curve_selection == CURVE_SELECTED_2)
        {
            c = &curve_2;
            r = 0;
            g = 162;
            b = 232;
        }
        else
        {
            throw std::runtime_error("Invalid curve");
        }

        switch(editor_mode)
        {
            case MODE_TAIL:
                c->drawNewTailHighlight(&editor_window_inner_rect, (int) mouse.x, (int) mouse.y, r, g, b);
                break;

            case MODE_HEAD:
                c->drawNewHeadHighlight(&editor_window_inner_rect, (int) mouse.x, (int) mouse.y, r, g, b);
                break;

            case MODE_DELETE:
                c->drawRemoveHighlight(&editor_window_inner_rect, (int) mouse.x, (int) mouse.y, r, g, b);
                break;

            case MODE_MOVE:
                c->drawMoveHighlight(&editor_window_inner_rect, (int) mouse.x, (int) mouse.y, r, g, b);
                break;

            case MODE_OFFSET:
                c->drawOffsetHighlight(&editor_window_inner_rect, (int) mouse.x, (int) mouse.y, r, g, b);
                break;
        }

        return true;
    }

    return false;
}

bool CurveEditorScreen::drawHoveringTerminalInputButton()
{
    // Get the current global mouse position
    float x, y;
    SDL_GetMouseState(&x, &y);
    SDL_FPoint mouse = {x, y};

    // Subtract the offset of the screen positon
    mouse.x -= (float) this->x();
    mouse.y -= (float) this->y();

    // Check if the mouse is hovering over the terminal input button
    if(SDL_PointInRectFloat(&mouse, &terminal_input_button_texture_frect))
    {
        SDL_RenderTexture(global_renderer, hovering_terminal_input_button_texture, 
                            NULL, &terminal_input_button_texture_frect);
        return true;
    }

    return false;
}

bool CurveEditorScreen::updateEditorModeClicked()
{
    // Check if the mouse is hovering over an editor mode button
    int mode;
    if((mode = isHoveringEditorModeButton()) != -1)
    {   
        // Invalidate curves selected points if mode has been changed
        if(editor_mode != (enum CurveEditorScreen::_editor_mode) mode)
        {
            curve_1.invalidateSelected();
            curve_2.invalidateSelected();
        }

        editor_mode = (enum CurveEditorScreen::_editor_mode) mode;
        return true;
    }

    return false;
}

bool CurveEditorScreen::updateCurveSelectionClicked()
{
    // Check if the mouse is hovering over an editor mode button
    int selection;
    if((selection = isHoveringCurveSelectionButton()) != -1)
    {   
        // Invalidate curves selected points if curve selection has been changed
        if(curve_selection != (enum CurveEditorScreen::_curve_selection) selection)
        {
            curve_1.invalidateSelected();
            curve_2.invalidateSelected();
        }

        curve_selection = (enum CurveEditorScreen::_curve_selection) selection;
        return true;
    }

    return false;
}

bool CurveEditorScreen::updateTerminalInputButtonClicked()
{
    // Get the current global mouse position
    float x, y;
    SDL_GetMouseState(&x, &y);
    SDL_FPoint mouse = {x, y};

    // Subtract the offset of the screen positon
    mouse.x -= (float) this->x();
    mouse.y -= (float) this->y();

    // Check if the mouse is hovering over the terminal input button
    if(SDL_PointInRectFloat(&mouse, &terminal_input_button_texture_frect))
    {
        // Call the terminal input handler
        handleTerminalInput();
        return true;
    }

    return false;
}

void CurveEditorScreen::handleTIPoint(int x, int y)
{
    // Determine the curve to target
    Curve* target_curve;
    if(curve_selection == CURVE_SELECTED_1)
    {
        target_curve = &curve_1;
    }
    else
    {
        target_curve = &curve_2;
    }

    // Offset for coordinate points
    const SDL_Rect offset = {0, 0, 0, 0};

    // Add point to tail or head
    if(editor_mode == MODE_TAIL)
    {
        target_curve->addTail(&offset, x, y);
    }
    else
    {
        target_curve->addHead(&offset, x, y);
    }
}

bool CurveEditorScreen::parseCoordinate(const std::string& s, size_t& pos, int& outVal) 
{
    if (pos >= s.length() || s[pos] < '0' || s[pos] > '9') return false;
    
    int val = 0;
    while (pos < s.length() && s[pos] >= '0' && s[pos] <= '9') {
        val = val * 10 + (s[pos] - '0');
        // Check upper bound; lower bound is guaranteed by digit-only parsing
        if (val > 770) return false;
        pos++;
    }
    outVal = val;
    return true;
}

int_fast32_t CurveEditorScreen::parseAndExecute(const std::string& input) 
{
    if (input.empty()) return 0;

    std::vector<std::pair<int, int>> validPoints;
    size_t pos = 0;
    bool isList = false;

    if (input[pos] == '{') {
        isList = true;
        pos++;
    }

    bool expectComma = false;
    while (pos < input.length()) {
        // Check for end of list
        if (isList && input[pos] == '}') {
            pos++;
            break;
        }

        // Handle point separation in lists
        if (expectComma) {
            if (input[pos] != ',') return 0;
            pos++;
            expectComma = 0;
            continue;
        }

        // Parse opening parenthesis
        if (input[pos] != '(') return 0;
        pos++;

        // Parse X
        int x = 0;
        if (!parseCoordinate(input, pos, x)) return 0;
        
        // Parse comma separator
        if (pos >= input.length() || input[pos] != ',') return 0;
        pos++;

        // Parse Y
        int y = 0;
        if (!parseCoordinate(input, pos, y)) return 0;

        // Parse closing parenthesis
        if (pos >= input.length() || input[pos] != ')') return 0;
        pos++;

        validPoints.push_back({x, y});
        expectComma = true;

        // If it's a single coordinate pair, exit the loop
        if (!isList) break; 
    }

    // Final Validation:
    // 1. Ensure the parser consumed the entire string.
    // 2. Ensure at least one valid point was found.
    if (pos != input.length() || validPoints.empty()) return 0;

    // Verification complete. Only now do we invoke the handler function.
    for (const auto& point : validPoints) {
        handleTIPoint(point.first, point.second);
    }

    return validPoints.size();
}

void CurveEditorScreen::handleTerminalInput()
{
    // String for user input
    std::string ui;

    // Minimize the simulator window
    SDL_MinimizeWindow(global_window);

    // Print the entrance message:
    std::cout << "\n----------------------------------------" << std::endl;
    std::cout << "Terminal Input:" << std::endl;

    // Invalid mode, must be in point add mode
    if(editor_mode != MODE_TAIL && editor_mode != MODE_HEAD)
    {
        // Print error message
        std::cout << "\tInvalid editor mode - valid editor modes are:" << std::endl;
        std::cout << "\t\t - (1) Add to curve tail" << std::endl;
        std::cout << "\t\t - (2) Add to curve head" << std::endl << std::endl;
        std::cout << "(Press enter to return to the simulator window)";

        // Wait for user to press enter
        std::getline(std::cin, ui);

        // Put the simulator window back on the screen
        SDL_RestoreWindow(global_window);
        return;
    }

    // Print instructions
    std::cout << "\t- Ex: \"(1, 2)\" to enter a single point." << std::endl;
    std::cout << "\t- Ex: \"{(1, 2), (4, 5), ...}\" to enter multiple points.\n\n";

    // Loop if just enter and whitespace entered
    for(;;)
    {
        // Output the terminal cursor
        std::cout << APP_NAME << "> ";

        // Get the user's terminal input
        std::getline(std::cin, ui);

        // Copy to preserve original input
        std::string str = ui;

        // Remove whitespace
        str.erase(std::remove(str.begin(), str.end(), ' '), str.end());

        // Nothing entered, loop
        if(str.size() < 1)
        {
            continue;
        }

        // Parse points and call handler function for each
        int result;
        if((result = parseAndExecute(str)) > 0)
        {
            if(result == 1)
            {
                std::cout << "Added 1 point.\n";
            }
            else
            {
                std::cout << "Added " << result << " points.\n"; 
            }
            break;
        }

        // Print error message
        std::cout << "Invalid input: \"" << ui <<  "\"\n";
        break;
    }

    // Prompt the user to press enter
    std::cout << "\n(Press enter to return to the simulator window)" << std::endl;

    // Wait for user to press enter
    std::getline(std::cin, ui);

    // Put the simulator window back on the screen
    SDL_RestoreWindow(global_window);
}

void CurveEditorScreen::renderEditorModeButtons()
{
    // Draw the editor mode buttons onto the screen
    SDL_RenderTexture(global_renderer, mode_textures[MODE_TAIL], NULL, &mode_texture_frects[MODE_TAIL]);
    SDL_RenderTexture(global_renderer, mode_textures[MODE_HEAD], NULL, &mode_texture_frects[MODE_HEAD]);
    SDL_RenderTexture(global_renderer, mode_textures[MODE_DELETE], NULL, &mode_texture_frects[MODE_DELETE]);
    SDL_RenderTexture(global_renderer, mode_textures[MODE_MOVE], NULL, &mode_texture_frects[MODE_MOVE]);
    SDL_RenderTexture(global_renderer, mode_textures[MODE_OFFSET], NULL, &mode_texture_frects[MODE_OFFSET]);

    // Indicate the selected editor mode button
    button_selected_texture_frect.x = mode_texture_frects[editor_mode].x;
    button_selected_texture_frect.y = mode_texture_frects[editor_mode].y;
    SDL_RenderTexture(global_renderer, button_selected_texture, NULL, &button_selected_texture_frect);
}

void CurveEditorScreen::renderCurveSelectionButtons()
{
    // Draw the curve selection buttons
    SDL_RenderTexture(global_renderer, curve_selected_textures[CURVE_SELECTED_1], NULL, &curve_selected_texture_frects[CURVE_SELECTED_1]);
    SDL_RenderTexture(global_renderer, curve_selected_textures[CURVE_SELECTED_2], NULL, &curve_selected_texture_frects[CURVE_SELECTED_2]);

    // Indicate the selected editor mode button
    button_selected_texture_frect.x = curve_selected_texture_frects[curve_selection].x;
    button_selected_texture_frect.y = curve_selected_texture_frects[curve_selection].y;
    SDL_RenderTexture(global_renderer, button_selected_texture, NULL, &button_selected_texture_frect);
}

bool CurveEditorScreen::updateEditorWindowClicked()
{
    // Get mouse position
    SDL_FPoint mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);
    mouse.x -= x();
    mouse.y -= y();

    // Curve to modify
    Curve* c;

    // Check if mouse is in inner editor window
    if(SDL_PointInRectFloat(&mouse, &editor_window_inner_frect))
    {
        // Determine curve to modify
        if(curve_selection == CURVE_SELECTED_1)
        {
            c = &curve_1;
        }
        else if(curve_selection == CURVE_SELECTED_2)
        {
            c = &curve_2;
        }

        switch(editor_mode)
        {
            case MODE_TAIL:
                c->addTail(&editor_window_inner_rect, (int) mouse.x, (int) mouse.y);
                break;

            case MODE_HEAD:
                c->addHead(&editor_window_inner_rect, (int) mouse.x, (int) mouse.y);
                break;

            case MODE_DELETE:
                c->remove(&editor_window_inner_rect, (int) mouse.x, (int) mouse.y);
                break;

            case MODE_MOVE:
                c->move(&editor_window_inner_rect, (int) mouse.x, (int) mouse.y);
                break;

            case MODE_OFFSET:
                c->offset(&editor_window_inner_rect, (int) mouse.x, (int) mouse.y);
                break;
        }

        return true;
    }

    return false;
}

void CurveEditorScreen::onHoverEvent(SDL_Event *event)
{
    // A mouse button has been pressed
    if(event->type == SDL_EVENT_MOUSE_BUTTON_DOWN)
    {
        // Left mouse button
        if(event->button.button == SDL_BUTTON_LEFT)
        {
            // Check if an editor mode button has been clicked
            if(updateEditorModeClicked())
            {

            }
            // Check is a curve selection button has been clicked
            else if(updateCurveSelectionClicked())
            {

            }
            // Check if the curve editor window has been clicked
            else if(updateEditorWindowClicked())
            {

            }
            // Check if the terminal input button has been clicked
            else if(updateTerminalInputButtonClicked())
            {

            }
        }
    }
}

void CurveEditorScreen::onUpdate()
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
    curve_1.draw(&editor_window_inner_rect, 237, 28, 36);
    curve_2.draw(&editor_window_inner_rect, 0, 162, 255);

    // Draw the separator line between the editor mode and curve selection buttons, in black
    SDL_SetRenderDrawColor(global_renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderLine(global_renderer, 227, 825, 227, 895);

    // Draw the editor mode buttons
    this->renderEditorModeButtons();

    // Draw the curve selection buttons
    this->renderCurveSelectionButtons();

    // Draw the terminal input button
    SDL_RenderTexture(global_renderer, terminal_input_button_texture, NULL,
                        &terminal_input_button_texture_frect);
}

void CurveEditorScreen::onHoverUpdate()
{
    // Update normally
    this->onUpdate();

    // Check for any editor mode button hovering and draw if so
    if(this->drawHoveringEditorModeButton())
    {
        return;
    }
    // Check for any curve selection button hovering and draw if so
    else if(this->drawHoveringCurveSelectionButton())
    {
        return;
    }
    // Check for any editor pane hovering
    else if(this->drawHoveringEditorWindow())
    {
        return;
    }
    // Check for terminal input button hovering
    else if(this->drawHoveringTerminalInputButton())
    {
        
    }
}

Curve* CurveEditorScreen::getCurve(int s)
{
    if((CurveEditorScreen::_curve_selection) s == CURVE_SELECTED_1)
    {
        return &curve_1;
    }
    else if((CurveEditorScreen::_curve_selection) s == CURVE_SELECTED_2)
    {
        return &curve_2;
    }

    return (Curve*) nullptr;
}

CurveEditorScreen::~CurveEditorScreen()
{
    // Free the editor mode button textures
    SDL_DestroyTexture(mode_textures[MODE_TAIL]);
    SDL_DestroyTexture(mode_textures[MODE_HEAD]);
    SDL_DestroyTexture(mode_textures[MODE_DELETE]);
    SDL_DestroyTexture(mode_textures[MODE_MOVE]);
    SDL_DestroyTexture(mode_textures[MODE_OFFSET]);

    // Free the selected button texture
    SDL_DestroyTexture(button_selected_texture);

    // Free the hovering button texture
    SDL_DestroyTexture(hovering_button_texture);

    // Free the selected curve button textures
    SDL_DestroyTexture(curve_selected_textures[CURVE_SELECTED_1]);
    SDL_DestroyTexture(curve_selected_textures[CURVE_SELECTED_2]);
}