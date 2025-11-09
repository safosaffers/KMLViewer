#ifndef DEVIATIONRESULT_H
#define DEVIATIONRESULT_H
#include <QLineF>
#include <limits>
#define MIN_DEVIATION DeviationResult(std::numeric_limits<double>::max())
typedef struct DeviationResult {
private:
  double value;  /// deviation value (length)
  QPointF fromPoint;
  QLineF line;   /// line represented this deviation
  QLineF fromEdge;   /// perpendicular with lenth value represented this deviation
public:
  DeviationResult(double val) : value(val), line(QLineF()) {}
  DeviationResult() : value(0), line(QLineF()) {}
  DeviationResult(double value, QLineF line) : value(value), line(line) {}
  DeviationResult(DeviationResult&& other) noexcept
      : value(other.value), line(std::move(other.line)) {}
  DeviationResult& operator=(DeviationResult&& other) noexcept {
    if (this != &other) {
      value = other.value;
      line = std::move(other.line);
    }
    return *this;
  }
  DeviationResult(const DeviationResult& other) : value(other.value), line(other.line) {}
  DeviationResult& operator=(const DeviationResult& other) {
    if (this != &other) {
      value = other.value;
      line = other.line;
    }
    return *this;
  }

  void setValue(double value){
    this->value=value;
  }
  void setLine(QLineF line){
    this->line = line;
  }
  void setFromPoint(QPointF point){
    this->fromPoint = point;
  }
  void setFromEdge(QLineF edge){
    this->fromEdge = edge;
  }
  double getValue() const {
    return this->value;
  }
  QLineF getLine()const {
    return this->line;
  }
  QLineF getFromEdge()const {
    return this->fromEdge;
  }
  QPointF getFromPoint() const {
    return this->fromPoint;
  }
} DeviationResult;

#endif  // MAXDEVIATIONRESULT_H
