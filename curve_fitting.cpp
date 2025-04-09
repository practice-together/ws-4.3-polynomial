#include "curve_fitting.h"
#include <algorithm> // For std::min, std::max

CurveFittingUI* g_curveFittingUI = nullptr;

CurveFittingUI::CurveFittingUI() : 
    canvas(nullptr), 
    cbuf(nullptr),
    sidebar(nullptr),
    degree_dropdown(nullptr),
    plot_btn(nullptr),
    clear_btn(nullptr),
    status_label(nullptr),
    polynomial_degree(2),
    x_min(0),
    x_max(10),
    y_min(0),
    y_max(10),
    axis_initialized(false) {
    g_curveFittingUI = this;
}

void CurveFittingUI::init() {
    createUI();
}

void CurveFittingUI::createUI() {
    // Set screen background and disable scrolling
    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_hex(SCREEN_BG_COLOR), 0);
    lv_obj_clear_flag(screen, LV_OBJ_FLAG_SCROLLABLE);
    
    // Create title
    lv_obj_t *title_label = lv_label_create(screen);
    lv_label_set_text(title_label, "Polynomial Curve Fitting");
    lv_obj_set_style_text_color(title_label, lv_color_hex(TITLE_COLOR), 0);
    lv_obj_set_style_text_font(title_label, NULL, 0);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 10);
    
    // Create canvas for drawing
    cbuf = (lv_color_t*)heap_caps_malloc(
        LV_CANVAS_BUF_SIZE_TRUE_COLOR(CANVAS_WIDTH, CANVAS_HEIGHT),
        MALLOC_CAP_SPIRAM
    );
    canvas = lv_canvas_create(screen);
    lv_canvas_set_buffer(canvas, cbuf, CANVAS_WIDTH, CANVAS_HEIGHT, LV_IMG_CF_TRUE_COLOR);
    lv_obj_align(canvas, LV_ALIGN_LEFT_MID, 10, 0);
    
    // Set canvas background and style
    lv_canvas_fill_bg(canvas, lv_color_hex(CANVAS_BG_COLOR), LV_OPA_COVER);
    lv_obj_set_style_border_color(canvas, lv_color_hex(0x313244), 0);
    lv_obj_set_style_border_width(canvas, 2, 0);
    lv_obj_set_style_shadow_width(canvas, 20, 0);
    lv_obj_set_style_shadow_color(canvas, lv_color_hex(0x000000), 0);
    lv_obj_set_style_shadow_opa(canvas, LV_OPA_30, 0);
    
    // Drawing the initial axis
    drawAxis();
    
    // Add event for canvas touch
    lv_obj_add_flag(canvas, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(canvas, canvas_event_cb, LV_EVENT_PRESSED, NULL);
    
    // Create sidebar on the right
    sidebar = lv_obj_create(screen);
    lv_obj_set_size(sidebar, SIDEBAR_WIDTH - 20, CANVAS_HEIGHT);
    lv_obj_align(sidebar, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_set_style_bg_color(sidebar, lv_color_hex(0x1E1E2E), 0);
    lv_obj_set_style_border_color(sidebar, lv_color_hex(0x313244), 0);
    lv_obj_set_style_border_width(sidebar, 2, 0);
    lv_obj_set_style_shadow_width(sidebar, 20, 0);
    lv_obj_set_style_shadow_color(sidebar, lv_color_hex(0x000000), 0);
    lv_obj_set_style_shadow_opa(sidebar, LV_OPA_30, 0);
    lv_obj_clear_flag(sidebar, LV_OBJ_FLAG_SCROLLABLE);
    
    // Add degree selection dropdown
    lv_obj_t *degree_label = lv_label_create(sidebar);
    lv_label_set_text(degree_label, "Polynomial Degree:");
    lv_obj_set_style_text_color(degree_label, lv_color_hex(TEXT_COLOR), 0);
    lv_obj_align(degree_label, LV_ALIGN_TOP_MID, 0, 20);
    
    degree_dropdown = lv_dropdown_create(sidebar);
    lv_dropdown_set_options(degree_dropdown, 
                           "Linear (degree 1)\n"
                           "Quadratic (degree 2)\n"
                           "Cubic (degree 3)\n"
                           "Quartic (degree 4)\n"
                           "Quintic (degree 5)");
    lv_dropdown_set_selected(degree_dropdown, 1); // Default to quadratic
    lv_obj_set_size(degree_dropdown, SIDEBAR_WIDTH - 60, 40);
    lv_obj_align(degree_dropdown, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_add_event_cb(degree_dropdown, degree_dropdown_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Style the dropdown
    lv_obj_set_style_bg_color(degree_dropdown, lv_color_hex(DROPDOWN_BG_COLOR), 0);
    lv_obj_set_style_text_color(degree_dropdown, lv_color_hex(TEXT_COLOR), 0);
    lv_obj_set_style_border_color(degree_dropdown, lv_color_hex(0x45475A), 0);
    
    // Create "Plot Curve" button
    plot_btn = lv_btn_create(sidebar);
    lv_obj_set_size(plot_btn, SIDEBAR_WIDTH - 60, 50);
    lv_obj_align(plot_btn, LV_ALIGN_TOP_MID, 0, 120);
    lv_obj_set_style_bg_color(plot_btn, lv_color_hex(BUTTON_PLOT_COLOR), 0);
    lv_obj_set_style_bg_opa(plot_btn, LV_OPA_90, 0);
    lv_obj_set_style_shadow_width(plot_btn, 15, 0);
    lv_obj_set_style_shadow_color(plot_btn, lv_color_hex(BUTTON_PLOT_COLOR), 0);
    lv_obj_set_style_shadow_opa(plot_btn, LV_OPA_30, 0);
    lv_obj_set_style_border_width(plot_btn, 2, 0);
    lv_obj_set_style_border_color(plot_btn, lv_color_hex(0x313244), 0);
    lv_obj_set_style_radius(plot_btn, 10, 0);
    lv_obj_add_event_cb(plot_btn, plot_btn_event_cb, LV_EVENT_CLICKED, NULL);
    
    // Add label to the plot button
    lv_obj_t *plot_label = lv_label_create(plot_btn);
    lv_label_set_text(plot_label, "Plot Curve");
    lv_obj_set_style_text_color(plot_label, lv_color_hex(0x1E1E2E), 0);
    lv_obj_center(plot_label);
    
    // Create "Clear All" button
    clear_btn = lv_btn_create(sidebar);
    lv_obj_set_size(clear_btn, SIDEBAR_WIDTH - 60, 50);
    lv_obj_align(clear_btn, LV_ALIGN_TOP_MID, 0, 190);
    lv_obj_set_style_bg_color(clear_btn, lv_color_hex(BUTTON_CLEAR_COLOR), 0);
    lv_obj_set_style_bg_opa(clear_btn, LV_OPA_90, 0);
    lv_obj_set_style_shadow_width(clear_btn, 15, 0);
    lv_obj_set_style_shadow_color(clear_btn, lv_color_hex(BUTTON_CLEAR_COLOR), 0);
    lv_obj_set_style_shadow_opa(clear_btn, LV_OPA_30, 0);
    lv_obj_set_style_border_width(clear_btn, 2, 0);
    lv_obj_set_style_border_color(clear_btn, lv_color_hex(0x313244), 0);
    lv_obj_set_style_radius(clear_btn, 10, 0);
    lv_obj_add_event_cb(clear_btn, clear_btn_event_cb, LV_EVENT_CLICKED, NULL);
    
    // Add label to the clear button
    lv_obj_t *clear_label = lv_label_create(clear_btn);
    lv_label_set_text(clear_label, "Clear All");
    lv_obj_set_style_text_color(clear_label, lv_color_hex(0x1E1E2E), 0);
    lv_obj_center(clear_label);
    
    // Create status label
    status_label = lv_label_create(sidebar);
    lv_label_set_text(status_label, "Ready");
    lv_obj_set_style_text_color(status_label, lv_color_hex(TEXT_COLOR), 0);
    lv_obj_set_width(status_label, SIDEBAR_WIDTH - 60);
    lv_obj_align(status_label, LV_ALIGN_TOP_MID, 0, 260);
    lv_label_set_long_mode(status_label, LV_LABEL_LONG_WRAP);
}

void CurveFittingUI::updateStatusText(const char* text) {
    lvgl_port_lock(-1);
    lv_label_set_text(status_label, text);
    lvgl_port_unlock();
}

void CurveFittingUI::drawAxis() {
    // Initialize x and y ranges if not done already
    if (!axis_initialized) {
        x_min = 0;
        x_max = 10;
        y_min = 0;
        y_max = 10;
        axis_initialized = true;
    }
    
    // Clear canvas and set background
    lv_canvas_fill_bg(canvas, lv_color_hex(CANVAS_BG_COLOR), LV_OPA_COVER);
    
    // Define line style for axis
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.color = lv_color_hex(AXIS_COLOR);
    line_dsc.width = 2;
    
    // Calculate position of x and y axis
    int origin_x = 40;
    int origin_y = CANVAS_HEIGHT - 40;
    int axis_width = CANVAS_WIDTH - 60;
    int axis_height = CANVAS_HEIGHT - 60;
    
    // Draw x-axis
    lv_point_t x_points[] = {
        {origin_x, origin_y},
        {origin_x + axis_width, origin_y}
    };
    lv_canvas_draw_line(canvas, x_points, 2, &line_dsc);
    
    // Draw y-axis
    lv_point_t y_points[] = {
        {origin_x, origin_y},
        {origin_x, origin_y - axis_height}
    };
    lv_canvas_draw_line(canvas, y_points, 2, &line_dsc);
    
    // Draw x-axis ticks and labels
    lv_draw_rect_dsc_t tick_dsc;
    lv_draw_rect_dsc_init(&tick_dsc);
    tick_dsc.bg_color = lv_color_hex(AXIS_COLOR);
    
    // Initialize text style
    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.color = lv_color_hex(TEXT_COLOR);
    
    char label_text[10];
    
    // X-axis ticks
    for (int i = 0; i <= 10; i++) {
        int tick_x = origin_x + (i * axis_width) / 10;
        
        // Draw tick
        lv_canvas_draw_rect(canvas, tick_x, origin_y, 1, 5, &tick_dsc);
        
        // Draw label, but skip 0 which is at origin
        if (i > 0) {
            float value = x_min + (i * (x_max - x_min)) / 10;
            sprintf(label_text, "%d", (int)value);
            lv_canvas_draw_text(canvas, tick_x - 10, origin_y + 10, 20, 
                               &label_dsc, label_text);
        }
    }
    
    // Y-axis ticks
    for (int i = 0; i <= 10; i++) {
        int tick_y = origin_y - (i * axis_height) / 10;
        
        // Draw tick
        lv_canvas_draw_rect(canvas, origin_x - 5, tick_y, 5, 1, &tick_dsc);
        
        // Draw label, but skip 0 which is at origin
        if (i > 0) {
            float value = y_min + (i * (y_max - y_min)) / 10;
            sprintf(label_text, "%d", (int)value);
            lv_canvas_draw_text(canvas, origin_x - 25, tick_y - 5, 20, 
                               &label_dsc, label_text);
        }
    }
    
    // Draw axis labels
    lv_canvas_draw_text(canvas, origin_x + axis_width - 20, origin_y + 25, 20, 
                       &label_dsc, "X");
    lv_canvas_draw_text(canvas, origin_x - 25, origin_y - axis_height - 5, 20, 
                       &label_dsc, "Y");
    
    // Draw origin label
    lv_canvas_draw_text(canvas, origin_x - 25, origin_y + 10, 20, 
                       &label_dsc, "0");
    
    lv_obj_invalidate(canvas);
}

void CurveFittingUI::convertToCanvasCoords(float x, float y, int& canvas_x, int& canvas_y) {
    // Calculate position of x and y axis
    int origin_x = 40;
    int origin_y = CANVAS_HEIGHT - 40;
    int axis_width = CANVAS_WIDTH - 60;
    int axis_height = CANVAS_HEIGHT - 60;
    
    // Convert world coordinates to canvas coordinates
    float x_ratio = (x - x_min) / (x_max - x_min);
    float y_ratio = (y - y_min) / (y_max - y_min);
    
    canvas_x = origin_x + (int)(x_ratio * axis_width);
    canvas_y = origin_y - (int)(y_ratio * axis_height);
}

void CurveFittingUI::convertFromCanvasCoords(int canvas_x, int canvas_y, float& x, float& y) {
    // Calculate position of x and y axis
    int origin_x = 40;
    int origin_y = CANVAS_HEIGHT - 40;
    int axis_width = CANVAS_WIDTH - 60;
    int axis_height = CANVAS_HEIGHT - 60;
    
    // Convert canvas coordinates to world coordinates
    float x_ratio = (float)(canvas_x - origin_x) / axis_width;
    float y_ratio = (float)(origin_y - canvas_y) / axis_height;
    
    x = x_min + x_ratio * (x_max - x_min);
    y = y_min + y_ratio * (y_max - y_min);
}

void CurveFittingUI::addPoint(float x, float y) {
    if (points.size() < MAX_POINTS) {
        points.push_back(Point(x, y));
        drawPoints();
    } else {
        updateStatusText("Maximum points reached!");
    }
}

void CurveFittingUI::drawPoints() {
    // Redraw axis to clear previous points
    drawAxis();
    
    // Define circle style for points
    lv_draw_rect_dsc_t point_dsc;
    lv_draw_rect_dsc_init(&point_dsc);
    point_dsc.bg_color = lv_color_hex(POINT_COLOR);
    point_dsc.radius = POINT_RADIUS;
    
    // Draw each point
    for (size_t i = 0; i < points.size(); i++) {
        int canvas_x, canvas_y;
        convertToCanvasCoords(points[i].x, points[i].y, canvas_x, canvas_y);
        
        lv_canvas_draw_rect(canvas, canvas_x - POINT_RADIUS, canvas_y - POINT_RADIUS, 
                           POINT_RADIUS * 2, POINT_RADIUS * 2, &point_dsc);
    }
    
    // Draw the curve if we have one
    if (!curve_points.empty()) {
        drawCurve();
    }
    
    lv_obj_invalidate(canvas);
}

void CurveFittingUI::drawCurve() {
    if (curve_points.size() < 2) return;
    
    // Define line style for curve
    lv_draw_line_dsc_t line_dsc;
    lv_draw_line_dsc_init(&line_dsc);
    line_dsc.color = lv_color_hex(CURVE_COLOR);
    line_dsc.width = 2;
    
    // Draw line segments connecting curve points
    for (size_t i = 0; i < curve_points.size() - 1; i++) {
        int x1, y1, x2, y2;
        convertToCanvasCoords(curve_points[i].x, curve_points[i].y, x1, y1);
        convertToCanvasCoords(curve_points[i+1].x, curve_points[i+1].y, x2, y2);
        
        lv_point_t line_points[] = {
            {x1, y1},
            {x2, y2}
        };
        
        lv_canvas_draw_line(canvas, line_points, 2, &line_dsc);
    }
    
    lv_obj_invalidate(canvas);
}

void CurveFittingUI::clearCanvas() {
    points.clear();
    curve_points.clear();
    drawAxis();
    updateStatusText("Canvas cleared");
}

void CurveFittingUI::plotCurve() {
    if (points.size() < 2) {
        updateStatusText("Need at least 2 points!");
        return;
    }
    
    updateStatusText("Calculating curve...");
    
    // Perform polynomial fitting directly on ESP32
    calculatePolynomialFit();
    
    // Draw points and curve
    drawPoints();
    updateStatusText("Curve plotted successfully!");
}

void CurveFittingUI::clearPoints() {
    points.clear();
    curve_points.clear();
    drawAxis();
}

void CurveFittingUI::calculatePolynomialFit() {
    if (points.size() < 2) return;
    
    int n = points.size();
    int degree = polynomial_degree;
    
    // Create matrices for the linear system
    Eigen::MatrixXf A(n, degree + 1);
    Eigen::VectorXf b(n);
    
    // Fill matrices
    for (int i = 0; i < n; i++) {
        float x = points[i].x;
        float y = points[i].y;
        
        // Fill powers of x in the A matrix
        for (int j = 0; j <= degree; j++) {
            A(i, j) = pow(x, j);
        }
        
        // Fill y values in the b vector
        b(i) = y;
    }
    
    // Solve the system using QR decomposition
    Eigen::VectorXf coeffs = A.householderQr().solve(b);
    
    // Generate curve points
    curve_points.clear();
    
    // Get min and max x values
    float min_x = points[0].x;
    float max_x = points[0].x;
    
    for (size_t i = 1; i < points.size(); i++) {
        if (points[i].x < min_x) min_x = points[i].x;
        if (points[i].x > max_x) max_x = points[i].x;
    }
    
    // Add some margin to x range
    min_x = std::max(x_min, min_x - 0.5f);
    max_x = std::min(x_max, max_x + 0.5f);
    
    // Generate 100 points along the curve
    int num_curve_points = 100;
    float step = (max_x - min_x) / (num_curve_points - 1);
    
    for (int i = 0; i < num_curve_points; i++) {
        float x = min_x + i * step;
        float y = 0;
        
        // Calculate y using the polynomial coefficients
        for (int j = 0; j <= degree; j++) {
            y += coeffs(j) * pow(x, j);
        }
        
        curve_points.push_back(Point(x, y));
    }
}

void CurveFittingUI::update() {
    // No WebSocket updates needed, all computation is done locally
}

void CurveFittingUI::canvas_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    
    if (code == LV_EVENT_PRESSED) {
        lv_point_t point;
        lv_indev_get_point(lv_indev_get_act(), &point);
        
        // Convert to canvas-local coordinates
        point.x -= lv_obj_get_x(obj);
        point.y -= lv_obj_get_y(obj);
        
        float world_x, world_y;
        g_curveFittingUI->convertFromCanvasCoords(point.x, point.y, world_x, world_y);
        
        // Add the point if it's within the valid range
        if (world_x >= g_curveFittingUI->x_min && world_x <= g_curveFittingUI->x_max &&
            world_y >= g_curveFittingUI->y_min && world_y <= g_curveFittingUI->y_max) {
            g_curveFittingUI->addPoint(world_x, world_y);
            
            char status_text[50];
            sprintf(status_text, "Added point (%.2f, %.2f)", world_x, world_y);
            g_curveFittingUI->updateStatusText(status_text);
        }
    }
}

void CurveFittingUI::plot_btn_event_cb(lv_event_t * e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        g_curveFittingUI->plotCurve();
    }
}

void CurveFittingUI::clear_btn_event_cb(lv_event_t * e) {
    if (lv_event_get_code(e) == LV_EVENT_CLICKED) {
        g_curveFittingUI->clearCanvas();
    }
}

void CurveFittingUI::degree_dropdown_event_cb(lv_event_t * e) {
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t * dropdown = lv_event_get_target(e);
        int selected = lv_dropdown_get_selected(dropdown);
        
        // Convert dropdown index to polynomial degree (index + 1)
        g_curveFittingUI->polynomial_degree = selected + 1;
        
        char status_text[50];
        sprintf(status_text, "Set degree to %d", g_curveFittingUI->polynomial_degree);
        g_curveFittingUI->updateStatusText(status_text);
    }
}