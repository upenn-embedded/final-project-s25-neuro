/*
 * TFT_Grapher.c
 *
 * Real-time graphing utility for ST7735
 */

 #include "TFT_Grapher.h"
 #include "ST7735.h"
 #include <string.h>
 #include <stdio.h>
 #include <stdlib.h>
 
 
 /*
  * Initialize a graph structure with default values
  */
 void Graph_init(Graph* graph, uint16_t color, uint16_t bgColor, uint16_t axisColor) {
     // Initialize all data points to zero
     for (uint8_t i = 0; i < DATA_BUFFER_SIZE; i++) {
         graph->data[i] = 0;
     }
     
     // Set colors
     graph->color = color;
     graph->backgroundColor = bgColor;
     graph->axisColor = axisColor;
     graph->gridColor = rgb565(64, 64, 64); // Default grid color
     
     // Default settings
     graph->showGrid = 0;
     graph->autoScale = 1;
     graph->minValue = 0;
     graph->maxValue = 1023; // Default to 10-bit ADC range
     
     // Reset drawing position
     graph->currentX = GRAPH_X_START;
     graph->prevY = 0;
     graph->firstPoint = 1;
     
     // Clear title and label
     strcpy(graph->title, "");
     strcpy(graph->yAxisLabel, "");
     
     // Clear the screen
     LCD_setScreen(graph->backgroundColor);
 }
 
 /*
  * Set the title of the graph
  */
 void Graph_setTitle(Graph* graph, const char* title) {
     strncpy(graph->title, title, 31);
     graph->title[32] = '\0'; // Ensure null termination
     
     // Clear title area and redraw
     LCD_drawBlock(0, 0, LCD_WIDTH-1, GRAPH_Y_START-1, graph->backgroundColor);
     LCD_drawString(2, 5, graph->title, graph->axisColor, graph->backgroundColor);
 }
 
 /*
  * Set the Y axis label
  */
 void Graph_setYLabel(Graph* graph, const char* label) {
     strncpy(graph->yAxisLabel, label, 7);
     graph->yAxisLabel[7] = '\0'; // Ensure null termination
     
     // Redraw y axis label
     LCD_drawString(2, GRAPH_Y_START-15, graph->yAxisLabel, graph->axisColor, graph->backgroundColor);
 }
 
 /*
  * Set fixed scaling for the graph
  */
 void Graph_setScale(Graph* graph, uint16_t min, uint16_t max) {
     graph->minValue = min;
     graph->maxValue = max;
     graph->autoScale = 0; // Disable auto-scaling when manually set
     
     // Update scale labels
     Graph_drawMinMax(graph);
 }
 
 /*
  * Enable or disable auto-scaling
  */
 void Graph_enableAutoScale(Graph* graph, uint8_t enable) {
     graph->autoScale = enable;
 }
 
 /*
  * Enable or disable grid lines
  */
 void Graph_enableGrid(Graph* graph, uint8_t enable, uint16_t gridColor) {
     graph->showGrid = enable;
     graph->gridColor = gridColor;
     
     // Redraw grid if enabled
     if (enable) {
         // Horizontal grid lines (4 lines)
         for (uint8_t i = 1; i < 4; i++) {
             uint8_t y = GRAPH_Y_START + (GRAPH_HEIGHT * i) / 4;
             LCD_drawLine(GRAPH_X_START, y, 
                          GRAPH_X_START + GRAPH_WIDTH - 1, y, 
                          graph->gridColor);
         }
         
         // Vertical grid lines (every 16 pixels)
         for (uint8_t i = 1; i < (GRAPH_WIDTH / 16); i++) {
             uint8_t x = GRAPH_X_START + i * 16;
             LCD_drawLine(x, GRAPH_Y_START, 
                          x, GRAPH_Y_START + GRAPH_HEIGHT - 1, 
                          graph->gridColor);
         }
     }
 }
 
 /*
  * Draw the graph axes and labels
  */
 void Graph_drawAxes(Graph* graph) {
     // Draw the axes
     LCD_drawLine(GRAPH_X_START, GRAPH_Y_START, 
                  GRAPH_X_START, GRAPH_Y_START + GRAPH_HEIGHT - 1, 
                  graph->axisColor); // Y axis
     
     LCD_drawLine(GRAPH_X_START, GRAPH_Y_START + GRAPH_HEIGHT - 1, 
                  GRAPH_X_START + GRAPH_WIDTH - 1, GRAPH_Y_START + GRAPH_HEIGHT - 1, 
                  graph->axisColor); // X axis
     
     // Draw title
 //    if (strlen(graph->title) > 0) {
 //        LCD_drawBlock(0, 0, LCD_WIDTH-1, GRAPH_Y_START-1, graph->backgroundColor);
 //        LCD_drawString(2, 5, graph->title, graph->axisColor, graph->backgroundColor);
 //    }
     
     // Draw Y-axis label
 //    if (strlen(graph->yAxisLabel) > 0) {
 //        // Display horizontally at bottom
 //        LCD_drawString(2, GRAPH_Y_START, graph->yAxisLabel, 
 //                      graph->axisColor, graph->backgroundColor);
 //    }
     
     // Clear and redraw min/max value labels
     Graph_drawMinMax(graph);
 }
 
 void Graph_drawMinMax(Graph* graph) {
     // Clear and redraw min/max value labels
     LCD_drawBlock(2, GRAPH_Y_START, 
                   28, GRAPH_Y_START + 8, graph->backgroundColor);
     LCD_drawBlock(2, GRAPH_Y_START + GRAPH_HEIGHT - 10, 
                   28, GRAPH_Y_START + GRAPH_HEIGHT - 1, graph->backgroundColor);
     
     // Draw min/max values
     char buffer[8];
     sprintf(buffer, "%d", graph->maxValue);
     LCD_drawString(2, GRAPH_Y_START + 2, buffer, 
                   graph->axisColor, graph->backgroundColor);
     
     sprintf(buffer, "%d", graph->minValue);
     LCD_drawString(2, GRAPH_Y_START + GRAPH_HEIGHT - 10, buffer, 
                   graph->axisColor, graph->backgroundColor);
 }
 
 /*
  * Reset the graph (clear and start over)
  */
 void Graph_reset(Graph* graph) {
     // Clear the graph area
     LCD_drawBlock(GRAPH_X_START, GRAPH_Y_START, 
                  GRAPH_X_START + GRAPH_WIDTH - 1, 
                  GRAPH_Y_START + GRAPH_HEIGHT - 1, 
                  graph->backgroundColor);
     
     // Redraw grid if enabled
     if (graph->showGrid) {
         Graph_enableGrid(graph, 1, graph->gridColor);
     }
     
     // Redraw axes
     Graph_drawAxes(graph);
     
     // Reset current position
     graph->currentX = GRAPH_X_START;
     graph->firstPoint = 1;
 }
 
 /*
  * Add a new data point and draw it efficiently
  */
 void Graph_addDataPoint(Graph* graph, uint16_t value) {
     // Store the value in the buffer
     static uint8_t bufferIndex = 0;
     graph->data[bufferIndex] = value;
     bufferIndex = (bufferIndex + 1) % DATA_BUFFER_SIZE;
     
     // Update min/max if auto-scaling is enabled
     if (graph->autoScale) {
         // Find min and max values in the data
         uint16_t min = 0xFFFF;
         uint16_t max = 0;
         
         for (uint8_t i = 0; i < DATA_BUFFER_SIZE; i++) {
             if (graph->data[i] < min && graph->data[i] > 0) min = graph->data[i];
             if (graph->data[i] > max) max = graph->data[i];
         }
         
         // Add 10% margin
         uint16_t margin = (max - min) / 10;
         uint16_t newMin = (min > margin) ? min - margin : 0;
         uint16_t newMax = max + margin;
         
         // Prevent division by zero in scaling calculations
         if (newMax <= newMin) {
             newMax = newMin + 1;
         }
         
         // Only update and redraw if scale changed significantly
         if (abs(newMin - graph->minValue) > (graph->maxValue - graph->minValue)/10 ||
             abs(newMax - graph->maxValue) > (graph->maxValue - graph->minValue)/10) {
             graph->minValue = newMin;
             graph->maxValue = newMax;
             
             // Redraw the axes to update scale labels
             Graph_drawMinMax(graph);
             
             // Need to restart graph if scale changed significantly
             Graph_reset(graph);
             return;
         }
     }
     
     // Scale the data to fit the graph height
     uint16_t range = graph->maxValue - graph->minValue;
     if (range == 0) range = 1; // Prevent division by zero
     
     uint8_t y = GRAPH_Y_START + GRAPH_HEIGHT - 1 - 
                ((value - graph->minValue) * (GRAPH_HEIGHT - 1)) / range;
     
     // Ensure y value is within bounds
     if (y < GRAPH_Y_START) y = GRAPH_Y_START;
     if (y >= GRAPH_Y_START + GRAPH_HEIGHT) y = GRAPH_Y_START + GRAPH_HEIGHT - 1;
     
     // Draw the point or line
     if (graph->firstPoint) {
         // First point after reset, just draw a pixel
         LCD_drawPixel(graph->currentX, y, graph->color);
         graph->firstPoint = 0;
     } else {
         // Draw line from previous point to new point
         LCD_drawLine(graph->currentX - 2, graph->prevY, graph->currentX, y, graph->color);
     }
     
     // Store current position for next time
     graph->prevY = y;
     (graph->currentX)+=2;
     
     // Check if we've reached the right edge
     if (graph->currentX >= GRAPH_X_START + GRAPH_WIDTH) {
         Graph_reset(graph);
     }
 }