/*
 * TFT_Grapher.h
 *
 * Real-time graphing utility for ST7735
 */

#ifndef TFT_GRAPHER_H_
#define TFT_GRAPHER_H_

#include <avr/io.h>
#include "LCD_GFX.h"
#include "ST7735.h"

// Graph settings
#define GRAPH_WIDTH      120  // Width of the graph area (match display width)
#define GRAPH_HEIGHT     100   // Height of the graph area (leave space for labels)
#define GRAPH_X_START    30   // Starting X position of the graph
#define GRAPH_Y_START    20   // Starting Y position of the graph
#define DATA_BUFFER_SIZE 128  // Size of circular buffer for data points

// Data structure for a graph
typedef struct {
    uint16_t data[DATA_BUFFER_SIZE];   // Circular buffer to store data points
    uint16_t color;                    // Graph line color
    uint16_t backgroundColor;          // Background color
    uint16_t axisColor;                // Color for the axes
    uint16_t gridColor;                // Color for grid lines
    uint8_t showGrid;                  // Toggle grid lines
    uint8_t autoScale;                 // Automatically scale the graph
    uint16_t minValue;                 // Minimum value for scaling
    uint16_t maxValue;                 // Maximum value for scaling
    uint8_t currentX;                  // Current X position for drawing
    uint16_t prevY;                    // Previous Y position (for line drawing)
    uint8_t firstPoint;                // Flag for first point after reset
    char title[32];                    // Graph title
    char yAxisLabel[8];                // Label for Y axis
} Graph;

// Function prototypes
void Graph_init(Graph* graph, uint16_t color, uint16_t bgColor, uint16_t axisColor);
void Graph_setTitle(Graph* graph, const char* title);
void Graph_setYLabel(Graph* graph, const char* label);
void Graph_setScale(Graph* graph, uint16_t min, uint16_t max);
void Graph_enableAutoScale(Graph* graph, uint8_t enable);
void Graph_enableGrid(Graph* graph, uint8_t enable, uint16_t gridColor);
void Graph_addDataPoint(Graph* graph, uint16_t value);
void Graph_reset(Graph* graph);
void Graph_drawAxes(Graph* graph);
void Graph_drawMinMax(Graph* graph);

#endif /* TFT_GRAPHER_H_ */