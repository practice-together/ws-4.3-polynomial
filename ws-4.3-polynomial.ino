#include <Arduino.h>
#include <ESP_Panel_Library.h>
#include <ESP_IOExpander_Library.h>
#include <lvgl.h>
#include "lvgl_port_v8.h"
#include "curve_fitting.h"

// Extend IO Pin define
#define TP_RST 1
#define LCD_BL 2
#define LCD_RST 3
#define SD_CS 4
#define USB_SEL 5

// Global UI instance
CurveFittingUI* curveFittingUI = nullptr;

void setup() {
    Serial.begin(115200);
    delay(1000);  // Give serial time to initialize
    
    Serial.println("ESP32-S3 Polynomial Curve Fitting Starting...");

    pinMode(GPIO_INPUT_IO_4, OUTPUT);
    
    Serial.println("Initialize IO expander");
    ESP_IOExpander_CH422G *expander = new ESP_IOExpander_CH422G(
        (i2c_port_t)I2C_MASTER_NUM, 
        ESP_IO_EXPANDER_I2C_CH422G_ADDRESS, 
        I2C_MASTER_SCL_IO, 
        I2C_MASTER_SDA_IO
    );
    expander->init();
    expander->begin();

    Serial.println("Set the IO0-7 pin to output mode.");
    expander->enableAllIO_Output();
    expander->digitalWrite(TP_RST, HIGH);
    expander->digitalWrite(LCD_RST, HIGH);
    expander->digitalWrite(LCD_BL, HIGH);
    delay(100);
    
    // GT911 initialization sequence
    expander->digitalWrite(TP_RST, LOW);
    delay(100);
    digitalWrite(GPIO_INPUT_IO_4, LOW);
    delay(100);
    expander->digitalWrite(TP_RST, HIGH);
    delay(200);

    Serial.println("Initializing panel device");
    ESP_Panel *panel = new ESP_Panel();
    panel->init();
    
#if LVGL_PORT_AVOID_TEAR
    ESP_PanelBus_RGB *rgb_bus = static_cast<ESP_PanelBus_RGB *>(panel->getLcd()->getBus());
    rgb_bus->configRgbFrameBufferNumber(LVGL_PORT_DISP_BUFFER_NUM);
    rgb_bus->configRgbBounceBufferSize(LVGL_PORT_RGB_BOUNCE_BUFFER_SIZE);
#endif

    panel->begin();

    Serial.println("Initialize LVGL");
    lvgl_port_init(panel->getLcd(), panel->getTouch());

    Serial.println("Creating Curve Fitting UI");
    lvgl_port_lock(-1);
    curveFittingUI = new CurveFittingUI();
    curveFittingUI->init();
    lvgl_port_unlock();
    
    Serial.println("Polynomial Curve Fitting Ready");
}

void loop() {
    if (curveFittingUI) {
        curveFittingUI->update();
    }
    
    delay(10);
}