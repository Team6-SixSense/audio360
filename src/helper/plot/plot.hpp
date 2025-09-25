#include <matplot/matplot.h>
#include <vector>

/**
 * @brief Plots a line plot with the given x and y points.
 *
 * @param x Vector of x points.
 * @param y Vector of y points. Corresponds with x.
 */
void linePlot(std::vector<double> x, std::vector<double> y) {
  matplot::plot(x, y);
  matplot::show();
  matplot::wait();
}