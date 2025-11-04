#ifndef MAXDEVIATIONRESULT_H
#define MAXDEVIATIONRESULT_H
#include <QLineF>

typedef struct MaxDeviationResult {
  double value;  /// max deviation value between simplified and original per
                 /// all lines
  QLineF line;   /// line from that max value achieved
  MaxDeviationResult() : value(0), line(QLineF()) {}
  MaxDeviationResult(double value, QLineF line) : value(value), line(line) {}
  MaxDeviationResult(MaxDeviationResult&& other) noexcept
      : value(other.value), line(std::move(other.line)) {}
  MaxDeviationResult& operator=(MaxDeviationResult&& other) noexcept {
    if (this != &other) {
      value = other.value;
      line = std::move(other.line);
    }
    return *this;
  }
  MaxDeviationResult(const MaxDeviationResult& other) : value(other.value), line(other.line) {}
  MaxDeviationResult& operator=(const MaxDeviationResult& other) {
    if (this != &other) {
      value = other.value;
      line = other.line;
    }
    return *this;
  }

} MaxDeviationResult;

#endif  // MAXDEVIATIONRESULT_H