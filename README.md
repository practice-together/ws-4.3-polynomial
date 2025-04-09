## WARNING : Install Waveshare ESP32-S3 Touch 4.3 LCD Arduino Libraries First! 

https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-4.3#09_lvgl_Porting


# Polynomial Curve Fitting for ESP32 with Touch Screen

This project implements an interactive polynomial curve fitting tool for ESP32-S3 development boards with touchscreens. Users can place points on a coordinate system by touching the screen, select a polynomial degree, and generate a best-fit curve using the least squares method.

## Features

- Interactive touch interface for placing data points
- Adjustable polynomial degree (linear, quadratic, cubic, quartic, quintic)
- Real-time polynomial curve fitting using least squares method
- Clean, modern dark-themed UI
- All computation performed directly on the ESP32 (no external server required)
- Visualizes both data points and the fitted curve on a labeled coordinate system

## Hardware Requirements

- ESP32-S3 board with RGB display (e.g. Waveshare ESP32-S3 Touch 4.3 LCD)
- Touch screen interface

## Software Dependencies

- Arduino IDE
- ESP_Panel_Library
- ESP_IOExpander_Library
- LVGL (Light and Versatile Graphics Library) v8

## File Structure

```
├── ws-4.3-polynomial.ino         # Main Arduino sketch
├── curve_fitting.h               # Header for curve fitting UI
├── curve_fitting.cpp             # Implementation of UI and curve fitting
├── eigen.cpp                     # Simplified Eigen library
```

other files as per Waveshare sample code.


## Usage

1. Touch anywhere on the canvas to place data points
2. Select the desired polynomial degree from the dropdown menu
3. Press "Plot Curve" to calculate and display the best-fit polynomial
4. Press "Clear All" to start over with a new set of points

## How It Works

The application uses the least squares method to find the polynomial coefficients that minimize the squared error between the polynomial and the data points. The process involves:

1. Building a Vandermonde matrix from the x-coordinates
2. Setting up the normal equations
3. Solving the system using QR decomposition
4. Evaluating the resulting polynomial to draw the curve

## Mathematical Background

The polynomial fitting uses these key equations:

- Polynomial function: $f(x) = a_0 + a_1 x + a_2 x^2 + ... + a_n x^n$
- Matrix equation: $\mathbf{y} = \mathbf{X} \mathbf{a}$
- Normal equations: $\mathbf{X}^T \mathbf{X} \mathbf{a} = \mathbf{X}^T \mathbf{y}$
- Solution: $\mathbf{a} = (\mathbf{X}^T \mathbf{X})^{-1} \mathbf{X}^T \mathbf{y}$


## Acknowledgments

- The Eigen project for inspiration on matrix operations
- LVGL for the graphics library
- Waveshare sample code
- Note: LLMs were used to refine the code