/*
 * error_reporter.h
 *
 *  Created on: 19 de jan. de 2022
 *      Author: gabriel
 */

#ifndef MAIN_DL_FRAMEWORK_ERROR_REPORTER_H_
#define MAIN_DL_FRAMEWORK_ERROR_REPORTER_H_

#include <cstdarg>

/*
namespace tflite {

/// A functor that reports error to supporting system. Invoked similar to
/// printf.
///
/// Usage:
///  ErrorReporter foo;
///  foo.Report("test %d", 5);
/// or
///  va_list args;
///  foo.Report("test %d", args); // where args is va_list
///
/// Subclass ErrorReporter to provide another reporting destination.
/// For example, if you have a GUI program, you might redirect to a buffer
/// that drives a GUI error log box.
class ErrorReporter {
 public:
  virtual ~ErrorReporter() {}
  virtual int Report(const char* format, va_list args) = 0;
  int Report(const char* format, ...);
  int ReportError(void*, const char* format, ...);
};

}  // namespace tflite

// You should not make bare calls to the error reporter, instead use the
// TF_LITE_REPORT_ERROR macro, since this allows message strings to be
// stripped when the binary size has to be optimized. If you are looking to
// reduce binary size, define TF_LITE_STRIP_ERROR_STRINGS when compiling and
// every call will be stubbed out, taking no memory.
#ifndef TF_LITE_STRIP_ERROR_STRINGS
#define TF_LITE_REPORT_ERROR(reporter, ...)                             \
  do {                                                                  \
    static_cast<tflite::ErrorReporter*>(reporter)->Report(__VA_ARGS__); \
  } while (false)
#else  // TF_LITE_STRIP_ERROR_STRINGS
#define TF_LITE_REPORT_ERROR(reporter, ...)
#endif  // TF_LITE_STRIP_ERROR_STRINGS

*/
#endif /* MAIN_DL_FRAMEWORK_ERROR_REPORTER_H_ */
