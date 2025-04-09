#pragma once

#include <lvgl.h>
#include <vector>
#include "eigen.cpp"
#include "lvgl_port_v8.h"

// Colors
#define CANVAS_BG_COLOR       0x1E1E2E  // Dark modern background
#define AXIS_COLOR            0x45475A  // Medium gray for axis lines
#define POINT_COLOR           0xF5C2E7  // Pink for data points
#define CURVE_COLOR           0x89DCEB  // Cyan for fitted curve
#define SCREEN_BG_COLOR       0x11111B  // Darker background for screen
#define BUTTON_PLOT_COLOR     0x74C7EC  // Blue for plot button
#define BUTTON_CLEAR_COLOR    0xB4607E  // Red for clear button
#define DROPDOWN_BG_COLOR     0x313244  // Dark gray for dropdown
#define TITLE_COLOR           0x89DCEB  // Cyan for title
#define TEXT_COLOR            0xCDD6F4  // Light text color

// Canvas and UI dimensions
#define CANVAS_WIDTH          600
#define CANVAS_HEIGHT         400
#define SIDEBAR_WIDTH         200
#define TOTAL_WIDTH           800
#define TOTAL_HEIGHT          480

// Maximum number of points
#define MAX_POINTS            100

// Point constants
#define POINT_RADIUS          4

class CurveFittingUI {
public:
    CurveFittingUI();
    void init();
    void update();

private:
    struct Point {
        float x;
        float y;
        Point(float _x, float _y) : x(_x), y(_y) {}
    };

    // UI elements
    lv_obj_t *canvas;
    lv_color_t *cbuf;
    lv_obj_t *sidebar;
    lv_obj_t *degree_dropdown;
    lv_obj_t *plot_btn;
    lv_obj_t *clear_btn;
    lv_obj_t *status_label;
    
    // Data points and curve
    std::vector<Point> points;
    std::vector<Point> curve_points;
    
    // Selected polynomial degree
    int polynomial_degree;
    
    // Canvas coordinate transformation
    float x_min, x_max, y_min, y_max;
    bool axis_initialized;
    
    // UI methods
    void createUI();
    void drawAxis();
    void drawPoints();
    void drawCurve();
    void clearCanvas();
    void convertToCanvasCoords(float x, float y, int& canvas_x, int& canvas_y);
    void convertFromCanvasCoords(int canvas_x, int canvas_y, float& x, float& y);
    void updateStatusText(const char* text);
    
    // Curve fitting methods
    void addPoint(float x, float y);
    void plotCurve();
    void clearPoints();
    void calculatePolynomialFit();
    
    // Static event handlers
    static void canvas_event_cb(lv_event_t * e);
    static void plot_btn_event_cb(lv_event_t * e);
    static void clear_btn_event_cb(lv_event_t * e);
    static void degree_dropdown_event_cb(lv_event_t * e);
};

extern CurveFittingUI* g_curveFittingUI;