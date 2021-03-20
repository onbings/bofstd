/*
 * Copyright (c) 2013-2023, OnBings All rights reserved.
 *
 * THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 * PURPOSE.
 *
 * This module implements the unit testing of the stringformatter class
 *
 * Name:        ut_stringformatter.cpp
 * Author:      b.harmel@gmail.com
 * Revision:    1.0
 *
 * Rem:         Based on google test
 *
 * History:
 *
 * V 1.00  vendredi 30 mai 2014 16:51:15  b.harmel : Initial release
 */
#include <gtest/gtest.h>
#include <bofstd/bofstringformatter.h>
#include <fmt/core.h>
#include <fmt/printf.h>
#include <cctype>
#include <list>
#include <limits.h>
typedef fmt::v5::basic_string_view<char> string_view;		//Compile debian
//warning C4127 : conditional expression is constant
#pragma warning( push )
#pragma warning( disable : 4127)
USE_BOF_NAMESPACE()

const unsigned BIG_NUM = INT_MAX + 1u;
enum { BUFFER_SIZE = 4096 };
enum E { A = 42 };
enum TestEnum { Aa };

#ifdef _MSC_VER
# define FMT_VSNPRINTF    vsprintf_s
#else
# define FMT_VSNPRINTF    vsnprintf
#endif

template < std::size_t SIZE >
void safe_sprintf(char(&buffer)[SIZE], const char *format, ...) {
	std::va_list args;
	va_start(args, format);
	FMT_VSNPRINTF(buffer, SIZE, format, args);
	va_end(args);
}
// Format value using the standard library.
template <typename Char, typename T>
void std_format(const T &value, std::basic_string<Char> &result) {
	std::basic_ostringstream<Char> os;
	os << value;
	result = os.str();
}
// Checks if writing value to BasicWriter<Char> produces the same result
// as writing it to std::basic_ostringstream<Char>.
template <typename Char, typename T>
::testing::AssertionResult check_write(const T &value, const char *type) {
  fmt::basic_memory_buffer<Char> buffer;
  typedef fmt::back_insert_range<fmt::internal::basic_buffer<Char>> range;
  fmt::basic_writer<range> writer(buffer);
  writer.write(value);
  std::basic_string<Char> actual = to_string(buffer);
  std::basic_string<Char> expected;
  std_format(value, expected);
  if (expected == actual)
    return ::testing::AssertionSuccess();
  return ::testing::AssertionFailure()
    << "Value of: (Writer<" << type << ">() << value).str()\n"
    << "  Actual: " << actual << "\n"
    << "Expected: " << expected << "\n";
}
struct AnyWriteChecker {
  template <typename T>
  ::testing::AssertionResult operator()(const char *, const T &value) const {
    ::testing::AssertionResult result = check_write<char>(value, "char");
    return result ? check_write<wchar_t>(value, "wchar_t") : result;
  }
};

template <typename Char>
struct WriteChecker {
  template <typename T>
  ::testing::AssertionResult operator()(const char *, const T &value) const {
    return check_write<Char>(value, "char");
  }
};

// Checks if writing value to BasicWriter produces the same result
// as writing it to std::ostringstream both for char and wchar_t.
#define CHECK_WRITE(value) EXPECT_PRED_FORMAT1(AnyWriteChecker(), value)

#define CHECK_WRITE_CHAR(value) \
  EXPECT_PRED_FORMAT1(WriteChecker<char>(), value)
#define CHECK_WRITE_WCHAR(value) \
  EXPECT_PRED_FORMAT1(WriteChecker<wchar_t>(), value)




#define FMT_TEST_THROW_(statement, expected_exception, expected_message, fail) \
  GTEST_AMBIGUOUS_ELSE_BLOCKER_ \
  if (::testing::AssertionResult gtest_ar = ::testing::AssertionSuccess()) { \
    std::string gtest_expected_message = expected_message; \
    bool gtest_caught_expected = false; \
    try { \
      GTEST_SUPPRESS_UNREACHABLE_CODE_WARNING_BELOW_(statement); \
    } \
    catch (expected_exception const& e) { \
      if (gtest_expected_message != e.what()) { \
        gtest_ar \
          << #statement " throws an exception with a different message.\n" \
          << "Expected: " << gtest_expected_message << "\n" \
          << "  Actual: " << e.what(); \
        goto GTEST_CONCAT_TOKEN_(gtest_label_testthrow_, __LINE__); \
      } \
      gtest_caught_expected = true; \
    } \
    catch (...) { \
      gtest_ar << \
          "Expected: " #statement " throws an exception of type " \
          #expected_exception ".\n  Actual: it throws a different type."; \
      goto GTEST_CONCAT_TOKEN_(gtest_label_testthrow_, __LINE__); \
    } \
    if (!gtest_caught_expected) { \
      gtest_ar << \
          "Expected: " #statement " throws an exception of type " \
          #expected_exception ".\n  Actual: it throws nothing."; \
      goto GTEST_CONCAT_TOKEN_(gtest_label_testthrow_, __LINE__); \
    } \
  } else \
    GTEST_CONCAT_TOKEN_(gtest_label_testthrow_, __LINE__): \
      fail(gtest_ar.failure_message())

// Tests that the statement throws the expected exception and the exception's
// what() method returns expected message.
#define EXPECT_THROW_MSG(statement, expected_exception, expected_message) \
  FMT_TEST_THROW_(statement, expected_exception, \
      expected_message, GTEST_NONFATAL_FAILURE_)

void increment(char *s) {
  for (int i = static_cast<int>(std::strlen(s)) - 1; i >= 0; --i) {
    if (s[i] != '9') {
      ++s[i];
      break;
    }
    s[i] = '0';
  }
}
class Date {
  int year_, month_, day_;
public:
  Date(int year, int month, int day) : year_(year), month_(month), day_(day) {}

  int year() const { return year_; }
  int month() const { return month_; }
  int day() const { return day_; }
};
// ***************************************************************************************************************************************************
// *** Sprintf ***************************************************************************************************************************************
// ***************************************************************************************************************************************************

#define EXPECT_SPRINTF(expected_output, format, arg) \
	EXPECT_EQ(expected_output, Bof_Sprintf(format, arg) )

TEST(Sprintf_Test, NoArgs)
{
	EXPECT_EQ("test", Bof_Sprintf("test") );
}

TEST(Sprintf_Test, Escape)
{
	EXPECT_EQ("%", Bof_Sprintf("%%") );
	EXPECT_EQ("before %", Bof_Sprintf("before %%") );
	EXPECT_EQ("% after", Bof_Sprintf("%% after") );
	EXPECT_EQ("before % after", Bof_Sprintf("before %% after") );
	EXPECT_EQ("%s", Bof_Sprintf("%%s") );
}

TEST(Sprintf_Test, NoPositionalArgs)
{
	EXPECT_EQ("42", Bof_Sprintf("%d", 42) );
	EXPECT_EQ("before 42", Bof_Sprintf("before %d", 42) );
	EXPECT_EQ("42 after", Bof_Sprintf("%d after", 42) );
	EXPECT_EQ("before 42 after", Bof_Sprintf("before %d after", 42) );
	EXPECT_EQ("answer = 42", Bof_Sprintf("%s = %d", "answer", 42) );
}

TEST(Sprintf_Test, AutomaticArgIndexing)
{
	EXPECT_EQ("abc", Bof_Sprintf("%c%c%c", 'a', 'b', 'c') );
}

TEST(Sprintf_Test, DefaultAlignRight)
{
	EXPECT_SPRINTF("   42", "%5d", 42);
	EXPECT_SPRINTF("  abc", "%5s", "abc");
}

TEST(Sprintf_Test, ZeroFlag)
{
	EXPECT_SPRINTF("00042", "%05d", 42);
	EXPECT_SPRINTF("-0042", "%05d", -42);

	EXPECT_SPRINTF("00042", "%05d", 42);
	EXPECT_SPRINTF("-0042", "%05d", -42);
	EXPECT_SPRINTF("-004.2", "%06g", -4.2);
	EXPECT_SPRINTF("+00042", "%00+6d", 42);

	// '0' flag is ignored for non-numeric types.
// NO	
#if defined(_WIN32)
	EXPECT_SPRINTF("0000x", "%05c", 'x');
#else
	EXPECT_SPRINTF("    x", "%05c", 'x');	//c
#endif
}

TEST(Sprintf_Test, PlusFlag)
{
	EXPECT_SPRINTF("+42", "%+d", 42);
	EXPECT_SPRINTF("-42", "%+d", -42);
	EXPECT_SPRINTF("+0042", "%+05d", 42);
	EXPECT_SPRINTF("+0042", "%0++5d", 42);

	// '+' flag is ignored for non-numeric types.
	EXPECT_SPRINTF("x", "%+c", 'x');
}

TEST(Sprintf_Test, MinusFlag)
{
	EXPECT_SPRINTF("abc  ", "%-5s", "abc");
	EXPECT_SPRINTF("abc  ", "%0--5s", "abc");
}

TEST(Sprintf_Test, SpaceFlag)
{
	EXPECT_SPRINTF(" 42", "% d", 42);
	EXPECT_SPRINTF("-42", "% d", -42);
	EXPECT_SPRINTF(" 0042", "% 05d", 42);
	EXPECT_SPRINTF(" 0042", "%0  5d", 42);

	// ' ' flag is ignored for non-numeric types.
	EXPECT_SPRINTF("x", "% c", 'x');
}

TEST(Sprintf_Test, HashFlag)
{
	EXPECT_SPRINTF("042", "%#o", 042);
	EXPECT_SPRINTF("0", "%#o", 0);

	EXPECT_SPRINTF("0x42", "%#x", 0x42);
	EXPECT_SPRINTF("0X42", "%#X", 0x42);
	EXPECT_SPRINTF("0", "%#x", 0);

	EXPECT_SPRINTF("0x0042", "%#06x", 0x42);
	EXPECT_SPRINTF("0x0042", "%0##6x", 0x42);

// NO	EXPECT_SPRINTF("-42.000000", "%#F", -42.0);

	char buffer[BUFFER_SIZE];
	safe_sprintf(buffer, "%#e", -42.0);
	EXPECT_SPRINTF(buffer, "%#e", -42.0);
	safe_sprintf(buffer, "%#E", -42.0);
	EXPECT_SPRINTF(buffer, "%#E", -42.0);

	EXPECT_SPRINTF("-42.000000", "%#f", -42.0);
	EXPECT_SPRINTF("-42.0000", "%#g", -42.0);
	EXPECT_SPRINTF("-42.0000", "%#G", -42.0);
	safe_sprintf(buffer, "%#a", 16.0);
	EXPECT_SPRINTF(buffer, "%#a", 16.0);
	safe_sprintf(buffer, "%#A", 16.0);
	EXPECT_SPRINTF(buffer, "%#A", 16.0);

	// '#' flag is ignored for non-numeric types.
	EXPECT_SPRINTF("x", "%#c", 'x');
}

TEST(Sprintf_Test, Width)
{
	EXPECT_SPRINTF("  abc", "%5s", "abc");
}

TEST(Sprintf_Test, DynamicWidth)
{
	EXPECT_EQ("   42", Bof_Sprintf("%*d", 5, 42) );
	EXPECT_EQ("42   ", Bof_Sprintf("%*d", -5, 42) );
}

TEST(Sprintf_Test, IntPrecision)
{
	EXPECT_SPRINTF("00042", "%.5d", 42);
	EXPECT_SPRINTF("-00042", "%.5d", -42);
	EXPECT_SPRINTF("00042", "%.5x", 0x42);
	EXPECT_SPRINTF("0x00042", "%#.5x", 0x42);
	EXPECT_SPRINTF("00042", "%.5o", 042);
	EXPECT_SPRINTF("00042", "%#.5o", 042);

	EXPECT_SPRINTF("  00042", "%7.5d", 42);
	EXPECT_SPRINTF("  00042", "%7.5x", 0x42);
	EXPECT_SPRINTF("   0x00042", "%#10.5x", 0x42);
	EXPECT_SPRINTF("  00042", "%7.5o", 042);
	EXPECT_SPRINTF("     00042", "%#10.5o", 042);

	EXPECT_SPRINTF("00042  ", "%-7.5d", 42);
	EXPECT_SPRINTF("00042  ", "%-7.5x", 0x42);
	EXPECT_SPRINTF("0x00042   ", "%-#10.5x", 0x42);
	EXPECT_SPRINTF("00042  ", "%-7.5o", 042);
	EXPECT_SPRINTF("00042     ", "%-#10.5o", 042);
}

TEST(Sprintf_Test, FloatPrecision)
{
	char buffer[BUFFER_SIZE];
	safe_sprintf(buffer, "%.3e", 1234.5678);
	EXPECT_SPRINTF(buffer, "%.3e", 1234.5678);
	EXPECT_SPRINTF("1234.568", "%.3f", 1234.5678);
	safe_sprintf(buffer, "%.3g", 1234.5678);
	EXPECT_SPRINTF(buffer, "%.3g", 1234.5678);
	safe_sprintf(buffer, "%.3a", 1234.5678);
	EXPECT_SPRINTF(buffer, "%.3a", 1234.5678);
}

TEST(Sprintf_Test, IgnorePrecisionForNonNumericArg)
{
	EXPECT_SPRINTF("abc", "%.5s", "abc");
}

TEST(Sprintf_Test, DynamicPrecision)
{
	EXPECT_EQ("00042", Bof_Sprintf("%.*d", 5, 42) );
	EXPECT_EQ("42", Bof_Sprintf("%.*d", -5, 42) );
}


TEST(Sprintf_Test, Bool)
{
	EXPECT_SPRINTF("1", "%d", true);
// NO	EXPECT_SPRINTF("true", "%s", true);
}

TEST(Sprintf_Test, Int)
{
	EXPECT_SPRINTF("-42", "%d", -42);
	EXPECT_SPRINTF("-42", "%i", -42);
	unsigned u = 0 - 42u;
	EXPECT_SPRINTF(Bof_SafeFmtSprintf("{}", u), "%u", -42);
	EXPECT_SPRINTF(Bof_SafeFmtSprintf("{:o}", u), "%o", -42);
	EXPECT_SPRINTF(Bof_SafeFmtSprintf("{:x}", u), "%x", -42);
	EXPECT_SPRINTF(Bof_SafeFmtSprintf("{:X}", u), "%X", -42);
}


TEST(Sprintf_Test, Float)
{
	EXPECT_SPRINTF("392.650000", "%f", 392.65);
	EXPECT_SPRINTF("392.65", "%g", 392.65);
	EXPECT_SPRINTF("392.65", "%G", 392.65);
// NO	EXPECT_SPRINTF("392.650000", "%F", 392.65);
	char buffer[BUFFER_SIZE];
	safe_sprintf(buffer, "%e", 392.65);
	EXPECT_SPRINTF(buffer, "%e", 392.65);
	safe_sprintf(buffer, "%E", 392.65);
	EXPECT_SPRINTF(buffer, "%E", 392.65);
	safe_sprintf(buffer, "%a", -392.65);
	EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{:a}", -392.65) );
	safe_sprintf(buffer, "%A", -392.65);
	EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{:A}", -392.65) );
}

TEST(Sprintf_Test, Char)
{
	EXPECT_SPRINTF("x", "%c", 'x');
	int max = std::numeric_limits< int >::max();
	EXPECT_SPRINTF(Bof_SafeFmtSprintf("{}", static_cast< char > (max) ), "%c", max);
	// EXPECT_SPRINTF("x", "%lc", L'x');
	// TODO: test wchar_t
}

TEST(Sprintf_Test, String)
{
	EXPECT_SPRINTF("abc", "%s", "abc");
	const char *null_str = 0;
	EXPECT_SPRINTF("(null)", "%s", null_str);
	EXPECT_SPRINTF("    (null)", "%10s", null_str);
	// TODO: wide string
}

TEST(Sprintf_Test, Pointer)
{
	int        n;
	void       *p        = &n;
	char       buffer[BUFFER_SIZE];

	safe_sprintf(buffer, "%p", p);
	EXPECT_SPRINTF(buffer, "%p", p);
	p = 0;
	const char *null_str = 0;
#if defined(_WIN32)
#if defined(_WIN64)
	EXPECT_SPRINTF("0000000000000000", "%p", p);
	EXPECT_SPRINTF("  0000000000000000", "%18p", p);
	EXPECT_SPRINTF("0000000000000000", "%p", null_str);
#else
  EXPECT_SPRINTF("00000000", "%p", p);
  EXPECT_SPRINTF("  00000000", "%10p", p);
  EXPECT_SPRINTF("00000000", "%p", null_str);
#endif
#else
	EXPECT_SPRINTF("(nil)", "%p", p);
	EXPECT_SPRINTF("     (nil)", "%10p", p);
	EXPECT_SPRINTF("(nil)", "%p", null_str);
#endif
}

TEST(Sprintf_Test, Enum)
{
	EXPECT_SPRINTF("42", "%d", A);
}

// ***************************************************************************************************************************************************
// *** SafeSprintf_Test ************************************************* Add test from lib test file printf-test.cc and adapt them ******************
// ***************************************************************************************************************************************************


// Makes format string argument positional.
static std::string make_positional(fmt::string_view format) {
  std::string s(format.data(), format.size());
  s.replace(s.find('%'), 1, "%1$");
  return s;
}

static std::wstring make_positional(fmt::wstring_view format) {
  std::wstring s(format.data(), format.size());
  s.replace(s.find(L'%'), 1, L"%1$");
  return s;
}


#define EXPECT_PRINTF(expected_output, format, arg) \
  EXPECT_EQ(expected_output, Bof_SafeSprintf(format, arg)) \
    << "format: " << format; \
  EXPECT_EQ(expected_output, Bof_SafeSprintf(make_positional(format), arg))

TEST(PrintfTest, NoArgs) {
  EXPECT_EQ("test", Bof_SafeSprintf("test"));
  EXPECT_EQ(L"test", Bof_SafeSprintf(L"test"));
}

TEST(PrintfTest, Escape) {
  EXPECT_EQ("%", Bof_SafeSprintf("%%"));
  EXPECT_EQ("before %", Bof_SafeSprintf("before %%"));
  EXPECT_EQ("% after", Bof_SafeSprintf("%% after"));
  EXPECT_EQ("before % after", Bof_SafeSprintf("before %% after"));
  EXPECT_EQ("%s", Bof_SafeSprintf("%%s"));
  EXPECT_EQ(L"%", Bof_SafeSprintf(L"%%"));
  EXPECT_EQ(L"before %", Bof_SafeSprintf(L"before %%"));
  EXPECT_EQ(L"% after", Bof_SafeSprintf(L"%% after"));
  EXPECT_EQ(L"before % after", Bof_SafeSprintf(L"before %% after"));
  EXPECT_EQ(L"%s", Bof_SafeSprintf(L"%%s"));
}

TEST(PrintfTest, PositionalArgs) {
  EXPECT_EQ("42", Bof_SafeSprintf("%1$d", 42));
  EXPECT_EQ("before 42", Bof_SafeSprintf("before %1$d", 42));
  EXPECT_EQ("42 after", Bof_SafeSprintf("%1$d after", 42));
  EXPECT_EQ("before 42 after", Bof_SafeSprintf("before %1$d after", 42));
  EXPECT_EQ("answer = 42", Bof_SafeSprintf("%1$s = %2$d", "answer", 42));
  EXPECT_EQ("42 is the answer",
    Bof_SafeSprintf("%2$d is the %1$s", "answer", 42));
  EXPECT_EQ("abracadabra", Bof_SafeSprintf("%1$s%2$s%1$s", "abra", "cad"));
}

TEST(PrintfTest, AutomaticArgIndexing) {
  EXPECT_EQ("abc", Bof_SafeSprintf("%c%c%c", 'a', 'b', 'c'));
}

TEST(PrintfTest, NumberIsTooBigInArgIndex) {
  EXPECT_THROW_MSG(Bof_SafeSprintf(Bof_SafeFmtSprintf("%{}$", BIG_NUM)),
    fmt::format_error, "number is too big");
  EXPECT_THROW_MSG(Bof_SafeSprintf(Bof_SafeFmtSprintf("%{}$d", BIG_NUM)),
    fmt::format_error, "number is too big");
}

TEST(PrintfTest, SwitchArgIndexing) {
  EXPECT_THROW_MSG(Bof_SafeSprintf("%1$d%", 1, 2),
    fmt::format_error, "cannot switch from manual to automatic argument indexing");
  EXPECT_THROW_MSG(Bof_SafeSprintf(Bof_SafeFmtSprintf("%1$d%{}d", BIG_NUM), 1, 2),
    fmt::format_error, "number is too big");
  EXPECT_THROW_MSG(Bof_SafeSprintf("%1$d%d", 1, 2),
    fmt::format_error, "cannot switch from manual to automatic argument indexing");

  EXPECT_THROW_MSG(Bof_SafeSprintf("%d%1$", 1, 2),
    fmt::format_error, "cannot switch from automatic to manual argument indexing");
  EXPECT_THROW_MSG(Bof_SafeSprintf(Bof_SafeFmtSprintf("%d%{}$d", BIG_NUM), 1, 2),
    fmt::format_error, "number is too big");
  EXPECT_THROW_MSG(Bof_SafeSprintf("%d%1$d", 1, 2),
    fmt::format_error, "cannot switch from automatic to manual argument indexing");

  // Indexing errors override width errors.
  EXPECT_THROW_MSG(Bof_SafeSprintf(Bof_SafeFmtSprintf("%d%1${}d", BIG_NUM), 1, 2),
    fmt::format_error, "number is too big");
  EXPECT_THROW_MSG(Bof_SafeSprintf(Bof_SafeFmtSprintf("%1$d%{}d", BIG_NUM), 1, 2),
    fmt::format_error, "number is too big");
}

TEST(PrintfTest, InvalidArgIndex) {
  EXPECT_THROW_MSG(Bof_SafeSprintf("%0$d", 42), fmt::format_error,
    "argument index out of range");
  EXPECT_THROW_MSG(Bof_SafeSprintf("%2$d", 42), fmt::format_error,
    "argument index out of range");
  EXPECT_THROW_MSG(Bof_SafeSprintf(Bof_SafeFmtSprintf("%{}$d", INT_MAX), 42),
    fmt::format_error, "argument index out of range");

  EXPECT_THROW_MSG(Bof_SafeSprintf("%2$", 42),
    fmt::format_error, "argument index out of range");
  EXPECT_THROW_MSG(Bof_SafeSprintf(Bof_SafeFmtSprintf("%{}$d", BIG_NUM), 42),
    fmt::format_error, "number is too big");
}

TEST(PrintfTest, DefaultAlignRight) {
  EXPECT_PRINTF("   42", "%5d", 42);
  EXPECT_PRINTF("  abc", "%5s", "abc");
}

TEST(PrintfTest, ZeroFlag) {
  EXPECT_PRINTF("00042", "%05d", 42);
  EXPECT_PRINTF("-0042", "%05d", -42);

  EXPECT_PRINTF("00042", "%05d", 42);
  EXPECT_PRINTF("-0042", "%05d", -42);
  EXPECT_PRINTF("-004.2", "%06g", -4.2);

  EXPECT_PRINTF("+00042", "%00+6d", 42);

  // '0' flag is ignored for non-numeric types.
  EXPECT_PRINTF("0000x", "%05c", 'x');
}

TEST(PrintfTest, PlusFlag) {
  EXPECT_PRINTF("+42", "%+d", 42);
  EXPECT_PRINTF("-42", "%+d", -42);
  EXPECT_PRINTF("+0042", "%+05d", 42);
  EXPECT_PRINTF("+0042", "%0++5d", 42);

  // '+' flag is ignored for non-numeric types.
  EXPECT_PRINTF("x", "%+c", 'x');
}

TEST(PrintfTest, MinusFlag) {
  EXPECT_PRINTF("abc  ", "%-5s", "abc");
  EXPECT_PRINTF("abc  ", "%0--5s", "abc");
}

TEST(PrintfTest, SpaceFlag) {
  EXPECT_PRINTF(" 42", "% d", 42);
  EXPECT_PRINTF("-42", "% d", -42);
  EXPECT_PRINTF(" 0042", "% 05d", 42);
  EXPECT_PRINTF(" 0042", "%0  5d", 42);

  // ' ' flag is ignored for non-numeric types.
  EXPECT_PRINTF("x", "% c", 'x');
}

TEST(PrintfTest, HashFlag) {
  EXPECT_PRINTF("042", "%#o", 042);
  EXPECT_PRINTF(Bof_SafeFmtSprintf("0{:o}", static_cast<unsigned>(-042)), "%#o", -042);
  EXPECT_PRINTF("0", "%#o", 0);

  EXPECT_PRINTF("0x42", "%#x", 0x42);
  EXPECT_PRINTF("0X42", "%#X", 0x42);
  EXPECT_PRINTF(
    Bof_SafeFmtSprintf("0x{:x}", static_cast<unsigned>(-0x42)), "%#x", -0x42);
  EXPECT_PRINTF("0", "%#x", 0);

  EXPECT_PRINTF("0x0042", "%#06x", 0x42);
  EXPECT_PRINTF("0x0042", "%0##6x", 0x42);

  EXPECT_PRINTF("-42.000000", "%#f", -42.0);
  EXPECT_PRINTF("-42.000000", "%#F", -42.0);

  char buffer[BUFFER_SIZE];
  safe_sprintf(buffer, "%#e", -42.0);
  EXPECT_PRINTF(buffer, "%#e", -42.0);
  safe_sprintf(buffer, "%#E", -42.0);
  EXPECT_PRINTF(buffer, "%#E", -42.0);

  EXPECT_PRINTF("-42.0000", "%#g", -42.0);
  EXPECT_PRINTF("-42.0000", "%#G", -42.0);

  safe_sprintf(buffer, "%#a", 16.0);
  EXPECT_PRINTF(buffer, "%#a", 16.0);
  safe_sprintf(buffer, "%#A", 16.0);
  EXPECT_PRINTF(buffer, "%#A", 16.0);

  // '#' flag is ignored for non-numeric types.
  EXPECT_PRINTF("x", "%#c", 'x');
}

TEST(PrintfTest, Width) {
  EXPECT_PRINTF("  abc", "%5s", "abc");

  // Width cannot be specified twice.
  EXPECT_THROW_MSG(Bof_SafeSprintf("%5-5d", 42), fmt::format_error,
    "invalid type specifier");

  EXPECT_THROW_MSG(Bof_SafeSprintf(Bof_SafeFmtSprintf("%{}d", BIG_NUM), 42),
    fmt::format_error, "number is too big");
  EXPECT_THROW_MSG(Bof_SafeSprintf(Bof_SafeFmtSprintf("%1${}d", BIG_NUM), 42),
    fmt::format_error, "number is too big");
}

TEST(PrintfTest, DynamicWidth) {
  EXPECT_EQ("   42", Bof_SafeSprintf("%*d", 5, 42));
  EXPECT_EQ("42   ", Bof_SafeSprintf("%*d", -5, 42));
  EXPECT_THROW_MSG(Bof_SafeSprintf("%*d", 5.0, 42), fmt::format_error,
    "width is not integer");
  EXPECT_THROW_MSG(Bof_SafeSprintf("%*d"), fmt::format_error,
    "argument index out of range");
  EXPECT_THROW_MSG(Bof_SafeSprintf("%*d", BIG_NUM, 42), fmt::format_error,
    "number is too big");
}

TEST(PrintfTest, IntPrecision) {
  EXPECT_PRINTF("00042", "%.5d", 42);
  EXPECT_PRINTF("-00042", "%.5d", -42);
  EXPECT_PRINTF("00042", "%.5x", 0x42);
  EXPECT_PRINTF("0x00042", "%#.5x", 0x42);
  EXPECT_PRINTF("00042", "%.5o", 042);
  EXPECT_PRINTF("00042", "%#.5o", 042);

  EXPECT_PRINTF("  00042", "%7.5d", 42);
  EXPECT_PRINTF("  00042", "%7.5x", 0x42);
  EXPECT_PRINTF("   0x00042", "%#10.5x", 0x42);
  EXPECT_PRINTF("  00042", "%7.5o", 042);
  EXPECT_PRINTF("     00042", "%#10.5o", 042);

  EXPECT_PRINTF("00042  ", "%-7.5d", 42);
  EXPECT_PRINTF("00042  ", "%-7.5x", 0x42);
  EXPECT_PRINTF("0x00042   ", "%-#10.5x", 0x42);
  EXPECT_PRINTF("00042  ", "%-7.5o", 042);
  EXPECT_PRINTF("00042     ", "%-#10.5o", 042);
}

TEST(PrintfTest, FloatPrecision) {
  char buffer[BUFFER_SIZE];
  safe_sprintf(buffer, "%.3e", 1234.5678);
  EXPECT_PRINTF(buffer, "%.3e", 1234.5678);
  EXPECT_PRINTF("1234.568", "%.3f", 1234.5678);
  safe_sprintf(buffer, "%.3g", 1234.5678);
  EXPECT_PRINTF(buffer, "%.3g", 1234.5678);
  safe_sprintf(buffer, "%.3a", 1234.5678);
  EXPECT_PRINTF(buffer, "%.3a", 1234.5678);
}

TEST(PrintfTest, IgnorePrecisionForNonNumericArg) {
  EXPECT_PRINTF("abc", "%.5s", "abc");
}

TEST(PrintfTest, DynamicPrecision) {
  EXPECT_EQ("00042", Bof_SafeSprintf("%.*d", 5, 42));
  EXPECT_EQ("42", Bof_SafeSprintf("%.*d", -5, 42));
  EXPECT_THROW_MSG(Bof_SafeSprintf("%.*d", 5.0, 42), fmt::format_error,
    "precision is not integer");
  EXPECT_THROW_MSG(Bof_SafeSprintf("%.*d"), fmt::format_error,
    "argument index out of range");
  EXPECT_THROW_MSG(Bof_SafeSprintf("%.*d", BIG_NUM, 42), fmt::format_error,
    "number is too big");

  if (sizeof(long long) != sizeof(int)) 
  {
    long long prec = static_cast<long long>(INT_MIN) - 1;
    EXPECT_THROW_MSG(Bof_SafeSprintf("%.*d", prec, 42), fmt::format_error,
      "number is too big");
  }
}

template <typename T>
struct make_signed { typedef T type; };

#define SPECIALIZE_MAKE_SIGNED(T, S) \
  template <> \
  struct make_signed<T> { typedef S type; }

SPECIALIZE_MAKE_SIGNED(char, signed char);
SPECIALIZE_MAKE_SIGNED(unsigned char, signed char);
SPECIALIZE_MAKE_SIGNED(unsigned short, short);
SPECIALIZE_MAKE_SIGNED(unsigned, int);
SPECIALIZE_MAKE_SIGNED(unsigned long, long);
SPECIALIZE_MAKE_SIGNED(unsigned long long, long long);

// Test length format specifier ``length_spec``.
template <typename T, typename U>
void TestLength(const char *length_spec, U value) {
  long long signed_value = 0;
  unsigned long long unsigned_value = 0;
  // Apply integer promotion to the argument.
  using std::numeric_limits;
  unsigned long long max = numeric_limits<U>::max();
  using fmt::internal::const_check;
  if (const_check(max <= static_cast<unsigned>(numeric_limits<int>::max()))) {
    signed_value = static_cast<int>(value);
    unsigned_value = static_cast<unsigned>(value);
  }
  else if (const_check(max <= numeric_limits<unsigned>::max())) {
    signed_value = static_cast<unsigned>(value);
    unsigned_value = static_cast<unsigned>(value);
  }
  if (sizeof(U) <= sizeof(int) && sizeof(int) < sizeof(T)) {
    signed_value = static_cast<long long>(value);
    unsigned_value =
      static_cast<typename std::make_unsigned<unsigned>::type>(value);
  }
  else {
    signed_value = static_cast<typename make_signed<T>::type>(value);
    unsigned_value = static_cast<typename std::make_unsigned<T>::type>(value);
  }
  std::ostringstream os;
  os << signed_value;
  EXPECT_PRINTF(os.str(), Bof_SafeFmtSprintf("%{}d", length_spec), value);
  EXPECT_PRINTF(os.str(), Bof_SafeFmtSprintf("%{}i", length_spec), value);
  os.str("");
  os << unsigned_value;
  EXPECT_PRINTF(os.str(), Bof_SafeFmtSprintf("%{}u", length_spec), value);
  os.str("");
  os << std::oct << unsigned_value;
  EXPECT_PRINTF(os.str(), Bof_SafeFmtSprintf("%{}o", length_spec), value);
  os.str("");
  os << std::hex << unsigned_value;
  EXPECT_PRINTF(os.str(), Bof_SafeFmtSprintf("%{}x", length_spec), value);
  os.str("");
  os << std::hex << std::uppercase << unsigned_value;
  EXPECT_PRINTF(os.str(), Bof_SafeFmtSprintf("%{}X", length_spec), value);
}

template <typename T>
void TestLength(const char *length_spec) {
  T min = std::numeric_limits<T>::min(), max = std::numeric_limits<T>::max();
  TestLength<T>(length_spec, 42);
  TestLength<T>(length_spec, -42);
  TestLength<T>(length_spec, min);
  TestLength<T>(length_spec, max);
  TestLength<T>(length_spec, static_cast<long long>(min) - 1);
  unsigned long long long_long_max = std::numeric_limits<long long>::max();
  if (static_cast<unsigned long long>(max) < long_long_max)
    TestLength<T>(length_spec, static_cast<long long>(max) + 1);
  TestLength<T>(length_spec, std::numeric_limits<short>::min());
  TestLength<T>(length_spec, std::numeric_limits<unsigned short>::max());
  TestLength<T>(length_spec, std::numeric_limits<int>::min());
  TestLength<T>(length_spec, std::numeric_limits<int>::max());
  TestLength<T>(length_spec, std::numeric_limits<unsigned>::min());
  TestLength<T>(length_spec, std::numeric_limits<unsigned>::max());
  TestLength<T>(length_spec, std::numeric_limits<long long>::min());
  TestLength<T>(length_spec, std::numeric_limits<long long>::max());
  TestLength<T>(length_spec, std::numeric_limits<unsigned long long>::min());
  TestLength<T>(length_spec, std::numeric_limits<unsigned long long>::max());
}

TEST(PrintfTest, Length) {
  TestLength<char>("hh");
  TestLength<signed char>("hh");
  TestLength<unsigned char>("hh");
  TestLength<short>("h");
  TestLength<unsigned short>("h");
  TestLength<long>("l");
  TestLength<unsigned long>("l");
  TestLength<long long>("ll");
  TestLength<unsigned long long>("ll");
  TestLength<intmax_t>("j");
  TestLength<std::size_t>("z");
  TestLength<std::ptrdiff_t>("t");
  long double max = std::numeric_limits<long double>::max();
  EXPECT_PRINTF(Bof_SafeFmtSprintf("{}", max), "%g", max);
  EXPECT_PRINTF(Bof_SafeFmtSprintf("{}", max), "%Lg", max);
}

TEST(PrintfTest, Bool) {
  EXPECT_PRINTF("1", "%d", true);
  EXPECT_PRINTF("true", "%s", true);
}

TEST(PrintfTest, Int) {
  EXPECT_PRINTF("-42", "%d", -42);
  EXPECT_PRINTF("-42", "%i", -42);
  unsigned u = 0 - 42u;
  EXPECT_PRINTF(Bof_SafeFmtSprintf("{}", u), "%u", -42);
  EXPECT_PRINTF(Bof_SafeFmtSprintf("{:o}", u), "%o", -42);
  EXPECT_PRINTF(Bof_SafeFmtSprintf("{:x}", u), "%x", -42);
  EXPECT_PRINTF(Bof_SafeFmtSprintf("{:X}", u), "%X", -42);
}

TEST(PrintfTest, long_long) {
  // fmt::printf allows passing long long arguments to %d without length
  // specifiers.
  long long max = std::numeric_limits<long long>::max();
  EXPECT_PRINTF(Bof_SafeFmtSprintf("{}", max), "%d", max);
}

TEST(PrintfTest, Float) {
  EXPECT_PRINTF("392.650000", "%f", 392.65);
  EXPECT_PRINTF("392.65", "%.2f", 392.65);
  EXPECT_PRINTF("392.6", "%.1f", 392.65);
  EXPECT_PRINTF("393", "%.f", 392.65);
  EXPECT_PRINTF("392.650000", "%F", 392.65);
  char buffer[BUFFER_SIZE];
  safe_sprintf(buffer, "%e", 392.65);
  EXPECT_PRINTF(buffer, "%e", 392.65);
  safe_sprintf(buffer, "%E", 392.65);
  EXPECT_PRINTF(buffer, "%E", 392.65);
  EXPECT_PRINTF("392.65", "%g", 392.65);
  EXPECT_PRINTF("392.65", "%G", 392.65);
  safe_sprintf(buffer, "%a", -392.65);
  EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{:a}", -392.65));
  safe_sprintf(buffer, "%A", -392.65);
  EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{:A}", -392.65));
}

TEST(PrintfTest, Inf) {
  double inf = std::numeric_limits<double>::infinity();
  for (const char* type = "fega"; *type; ++type) {
    EXPECT_PRINTF("inf", Bof_SafeFmtSprintf("%{}", *type), inf);
    char upper = static_cast<char>(std::toupper(*type));
    EXPECT_PRINTF("INF", Bof_SafeFmtSprintf("%{}", upper), inf);
  }
}


TEST(PrintfTest, Char) {
  EXPECT_PRINTF("x", "%c", 'x');
  int max = std::numeric_limits<int>::max();
  EXPECT_PRINTF(Bof_SafeFmtSprintf("{}", static_cast<char>(max)), "%c", max);
  //EXPECT_PRINTF("x", "%lc", L'x');
  EXPECT_PRINTF(L"x", L"%c", L'x');
  EXPECT_PRINTF(Bof_SafeFmtSprintf(L"{}", static_cast<wchar_t>(max)), L"%c", max);
}

TEST(PrintfTest, String) {
  EXPECT_PRINTF("abc", "%s", "abc");
  const char *null_str = FMT_NULL;
  EXPECT_PRINTF("(null)", "%s", null_str);
  EXPECT_PRINTF("    (null)", "%10s", null_str);
  EXPECT_PRINTF(L"abc", L"%s", L"abc");
  const wchar_t *null_wstr = FMT_NULL;
  EXPECT_PRINTF(L"(null)", L"%s", null_wstr);
  EXPECT_PRINTF(L"    (null)", L"%10s", null_wstr);
}

TEST(PrintfTest, Pointer) {
  int n;
  void *p = &n;
  EXPECT_PRINTF(Bof_SafeFmtSprintf("{}", p), "%p", p);
  p = FMT_NULL;
  EXPECT_PRINTF("(nil)", "%p", p);
  EXPECT_PRINTF("     (nil)", "%10p", p);
  const char *s = "test";
  EXPECT_PRINTF(Bof_SafeFmtSprintf("{:p}", s), "%p", s);
  const char *null_str = FMT_NULL;
  EXPECT_PRINTF("(nil)", "%p", null_str);

  p = &n;
  EXPECT_PRINTF(Bof_SafeFmtSprintf(L"{}", p), L"%p", p);
  p = FMT_NULL;
  EXPECT_PRINTF(L"(nil)", L"%p", p);
  EXPECT_PRINTF(L"     (nil)", L"%10p", p);
  const wchar_t *w = L"test";
  EXPECT_PRINTF(Bof_SafeFmtSprintf(L"{:p}", w), L"%p", w);
  const wchar_t *null_wstr = FMT_NULL;
  EXPECT_PRINTF(L"(nil)", L"%p", null_wstr);
}

TEST(PrintfTest, Location) {
  // TODO: test %n
}


TEST(PrintfTest, Enum) {
  EXPECT_PRINTF("42", "%d", A);
}

#if FMT_USE_FILE_DESCRIPTORS
TEST(PrintfTest, Examples) {
  const char *weekday = "Thursday";
  const char *month = "August";
  int day = 21;
  EXPECT_WRITE(stdout, fmt::printf("%1$s, %3$d %2$s", weekday, month, day),
    "Thursday, 21 August");
}

TEST(PrintfTest, PrintfError) {
  fmt::file read_end, write_end;
  fmt::file::pipe(read_end, write_end);
  int result = fmt::fprintf(read_end.fdopen("r").get(), "test");
  EXPECT_LT(result, 0);
}
#endif

TEST(PrintfTest, WideString) {
  EXPECT_EQ(L"abc", Bof_SafeSprintf(L"%s", L"abc"));
}

TEST(PrintfTest, PrintfCustom) {
//  EXPECT_EQ("abc", Bof_SafeSprintf("%s", TestString("abc")));
}

TEST(PrintfTest, OStream) {
  std::ostringstream os;
  int ret = fmt::fprintf(os, "Don't %s!", "panic");
  EXPECT_EQ("Don't panic!", os.str());
  EXPECT_EQ(12, ret);
}

TEST(PrintfTest, VPrintf) {
  fmt::format_arg_store<fmt::printf_context, int> as{ 42 };
  fmt::basic_format_args<fmt::printf_context> args(as);
  EXPECT_EQ(fmt::vsprintf("%d", args), "42");
//  EXPECT_WRITE(stdout, fmt::vprintf("%d", args), "42");
//  EXPECT_WRITE(stdout, fmt::vfprintf(stdout, "%d", args), "42");
//  EXPECT_WRITE(stdout, fmt::vfprintf(std::cout, "%d", args), "42");
}

//template<typename... Args>
//void check_format_string_regression(fmt::string_view s, const Args&... args) {
//  Bof_SafeSprintf(s, args...);
//}

//TEST(PrintfTest, CheckFormatStringRegression) {
//  check_format_string_regression("%c%s", 'x', "");
//}

TEST(PrintfTest, VSPrintfMakeArgsExample) {
  fmt::format_arg_store<fmt::printf_context, int, const char *> as{
    42, "something" };
  fmt::basic_format_args<fmt::printf_context> args(as);
  EXPECT_EQ(
    "[42] something happened", fmt::vsprintf("[%d] %s happened", args));
  auto as2 = fmt::make_printf_args(42, "something");
  fmt::basic_format_args<fmt::printf_context> args2(as2);
  EXPECT_EQ(
    "[42] something happened", fmt::vsprintf("[%d] %s happened", args2));
  //the older gcc versions can't cast the return value
#if !defined(__GNUC__) || (__GNUC__ > 4) 
  EXPECT_EQ(
    "[42] something happened",
    fmt::vsprintf(
      "[%d] %s happened", fmt::make_printf_args(42, "something")));
#endif
}

TEST(PrintfTest, VSPrintfMakeWArgsExample) {
  fmt::format_arg_store<fmt::wprintf_context, int, const wchar_t *> as{
    42, L"something" };
  fmt::basic_format_args<fmt::wprintf_context> args(as);
  EXPECT_EQ(
    L"[42] something happened",
    fmt::vsprintf(L"[%d] %s happened", args));
  auto  as2 = fmt::make_wprintf_args(42, L"something");
  fmt::basic_format_args<fmt::wprintf_context> args2(as2);
  EXPECT_EQ(
    L"[42] something happened", fmt::vsprintf(L"[%d] %s happened", args2));
  // the older gcc versions can't cast the return value
#if !defined(__GNUC__) || (__GNUC__ > 4)
  EXPECT_EQ(
    L"[42] something happened",
    fmt::vsprintf(
      L"[%d] %s happened", fmt::make_wprintf_args(42, L"something")));
#endif
}


// ***************************************************************************************************************************************************
// *** Bof_SafeFmtSprintf ************************************************* Add test from lib test file Bof_SafeFmtSprintf-test.cc and adapt them ***********
// ***************************************************************************************************************************************************


TEST(StringViewTest, Ctor) {
  EXPECT_STREQ("abc", fmt::string_view("abc").data());
  EXPECT_EQ(3u, fmt::string_view("abc").size());

  EXPECT_STREQ("defg", fmt::string_view(std::string("defg")).data());
  EXPECT_EQ(4u, fmt::string_view(std::string("defg")).size());
}

TEST(WriterTest, Data) {
  fmt::memory_buffer buf;
  fmt::writer w(buf);
  w.write(42);
  EXPECT_EQ("42", to_string(buf));
}

TEST(WriterTest, WriteInt) {
  CHECK_WRITE(42);
  CHECK_WRITE(-42);
  CHECK_WRITE(static_cast<short>(12));
  CHECK_WRITE(34u);
  CHECK_WRITE(std::numeric_limits<int>::min());
  CHECK_WRITE(std::numeric_limits<int>::max());
  CHECK_WRITE(std::numeric_limits<unsigned>::max());
}

TEST(WriterTest, WriteLong) {
  CHECK_WRITE(56l);
  CHECK_WRITE(78ul);
  CHECK_WRITE(std::numeric_limits<long>::min());
  CHECK_WRITE(std::numeric_limits<long>::max());
  CHECK_WRITE(std::numeric_limits<unsigned long>::max());
}

TEST(WriterTest, WriteLongLong) {
  CHECK_WRITE(56ll);
  CHECK_WRITE(78ull);
  CHECK_WRITE(std::numeric_limits<long long>::min());
  CHECK_WRITE(std::numeric_limits<long long>::max());
  CHECK_WRITE(std::numeric_limits<unsigned long long>::max());
}

TEST(WriterTest, WriteDouble) {
  CHECK_WRITE(4.2);
  CHECK_WRITE(-4.2);
  CHECK_WRITE(std::numeric_limits<double>::min());
  CHECK_WRITE(std::numeric_limits<double>::max());
}

TEST(WriterTest, WriteLongDouble) {
  CHECK_WRITE(4.2l);
  CHECK_WRITE_CHAR(-4.2l);
  std::wstring str;
  std_format(4.2l, str);
  if (str[0] != '-')
    CHECK_WRITE_WCHAR(-4.2l);
  else
    fmt::print("warning: long double formatting with std::swprintf is broken");
  CHECK_WRITE(std::numeric_limits<long double>::min());
  CHECK_WRITE(std::numeric_limits<long double>::max());
}

TEST(WriterTest, WriteDoubleAtBufferBoundary) {
  fmt::memory_buffer buf;
  fmt::writer writer(buf);
  for (int i = 0; i < 100; ++i)
    writer.write(1.23456789);
}

TEST(WriterTest, WriteDoubleWithFilledBuffer) {
  fmt::memory_buffer buf;
  fmt::writer writer(buf);
  // Fill the buffer.
  for (int i = 0; i < fmt::inline_buffer_size; ++i)
    writer.write(' ');
  writer.write(1.2);
  fmt::string_view sv(buf.data(), buf.size());
  sv.remove_prefix(fmt::inline_buffer_size);
  EXPECT_EQ("1.2", sv);
}

TEST(WriterTest, WriteChar) {
  CHECK_WRITE('a');
}

TEST(WriterTest, WriteWideChar) {
  CHECK_WRITE_WCHAR(L'a');
}

TEST(WriterTest, WriteString) {
  CHECK_WRITE_CHAR("abc");
  CHECK_WRITE_WCHAR("abc");
  // The following line shouldn't compile:
  //std::declval<fmt::basic_writer<fmt::buffer>>().write(L"abc");
}

TEST(WriterTest, WriteWideString) {
  CHECK_WRITE_WCHAR(L"abc");
  // The following line shouldn't compile:
  //std::declval<fmt::basic_writer<fmt::wbuffer>>().write("abc");
}

TEST(FormatToTest, FormatWithoutArgs) {
  std::string s;
  fmt::format_to(std::back_inserter(s), "test");
  EXPECT_EQ("test", s);
}

TEST(FormatToTest, Format) {
  std::string s;
  fmt::format_to(std::back_inserter(s), "part{0}", 1);
  EXPECT_EQ("part1", s);
  fmt::format_to(std::back_inserter(s), "part{0}", 2);
  EXPECT_EQ("part1part2", s);
}

TEST(FormatToTest, WideString) {
  std::vector<wchar_t> buf;
  fmt::format_to(std::back_inserter(buf), L"{}{}", 42, L'\0');
  EXPECT_STREQ(buf.data(), L"42");
}

TEST(FormatToTest, FormatToNonbackInsertIteratorWithSignAndNumericAlignment) {
  char buffer[16] = {};
  fmt::format_to(fmt::internal::make_checked(buffer, 16), "{: =+}", 42.0);
  EXPECT_STREQ("+42", buffer);
}

TEST(FormatToTest, FormatToMemoryBuffer) {
  fmt::basic_memory_buffer<char, 100> buffer;
  fmt::format_to(buffer, "{}", "foo");
  EXPECT_EQ("foo", to_string(buffer));
  fmt::wmemory_buffer wbuffer;
  fmt::format_to(wbuffer, L"{}", L"foo");
  EXPECT_EQ(L"foo", to_string(wbuffer));
}

TEST(FormatterTest, Escape) {
  EXPECT_EQ("{", Bof_SafeFmtSprintf("{{"));
  EXPECT_EQ("before {", Bof_SafeFmtSprintf("before {{"));
  EXPECT_EQ("{ after", Bof_SafeFmtSprintf("{{ after"));
  EXPECT_EQ("before { after", Bof_SafeFmtSprintf("before {{ after"));

  EXPECT_EQ("}", Bof_SafeFmtSprintf("}}"));
  EXPECT_EQ("before }", Bof_SafeFmtSprintf("before }}"));
  EXPECT_EQ("} after", Bof_SafeFmtSprintf("}} after"));
  EXPECT_EQ("before } after", Bof_SafeFmtSprintf("before }} after"));

  EXPECT_EQ("{}", Bof_SafeFmtSprintf("{{}}"));
  EXPECT_EQ("{42}", Bof_SafeFmtSprintf("{{{0}}}", 42));
}

TEST(FormatterTest, UnmatchedBraces) {
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{"), fmt::format_error, "invalid format string");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("}"), fmt::format_error, "unmatched '}' in format string");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0{}"), fmt::format_error, "invalid format string");
}

TEST(FormatterTest, NoArgs) {
  EXPECT_EQ("test", Bof_SafeFmtSprintf("test"));
}

TEST(FormatterTest, ArgsInDifferentPositions) {
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{0}", 42));
  EXPECT_EQ("before 42", Bof_SafeFmtSprintf("before {0}", 42));
  EXPECT_EQ("42 after", Bof_SafeFmtSprintf("{0} after", 42));
  EXPECT_EQ("before 42 after", Bof_SafeFmtSprintf("before {0} after", 42));
  EXPECT_EQ("answer = 42", Bof_SafeFmtSprintf("{0} = {1}", "answer", 42));
  EXPECT_EQ("42 is the answer", Bof_SafeFmtSprintf("{1} is the {0}", "answer", 42));
  EXPECT_EQ("abracadabra", Bof_SafeFmtSprintf("{0}{1}{0}", "abra", "cad"));
}

TEST(FormatterTest, ArgErrors) {
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{"), fmt::format_error, "invalid format string");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{?}"), fmt::format_error, "invalid format string");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0"), fmt::format_error, "invalid format string");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0}"), fmt::format_error, "argument index out of range");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{00}", 42), fmt::format_error, "invalid format string");

  char format_str[BUFFER_SIZE];
  safe_sprintf(format_str, "{%u", INT_MAX);
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf(format_str), fmt::format_error, "invalid format string");
  safe_sprintf(format_str, "{%u}", INT_MAX);
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf(format_str), fmt::format_error,
    "argument index out of range");

  safe_sprintf(format_str, "{%u", INT_MAX + 1u);
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf(format_str), fmt::format_error, "number is too big");
  safe_sprintf(format_str, "{%u}", INT_MAX + 1u);
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf(format_str), fmt::format_error, "number is too big");
}

template <int N>
struct TestFormat {
  template <typename... Args>
  static std::string format(fmt::string_view format_str, const Args &... args) {
    return TestFormat<N - 1>::format(format_str, N - 1, args...);
  }
};

template <>
struct TestFormat<0> {
  template <typename... Args>
  static std::string format(fmt::string_view format_str, const Args &... args) {
    return  fmt::format(format_str, args...);
  }
};

TEST(FormatterTest, ManyArgs) {
  EXPECT_EQ("19", TestFormat<20>::format("{19}"));
  EXPECT_THROW_MSG(TestFormat<20>::format("{20}"),
    fmt::format_error, "argument index out of range");
  EXPECT_THROW_MSG(TestFormat<21>::format("{21}"),
    fmt::format_error, "argument index out of range");
  enum { max_packed_args = fmt::internal::max_packed_args };
  std::string format_str = Bof_SafeFmtSprintf("{{{}}}", max_packed_args + 1);
  EXPECT_THROW_MSG(TestFormat<max_packed_args>::format(format_str),
    fmt::format_error, "argument index out of range");
}

TEST(FormatterTest, NamedArg) {
  EXPECT_EQ("1/a/A", Bof_SafeFmtSprintf("{_1}/{a_}/{A_}", fmt::arg("a_", 'a'),
    fmt::arg("A_", "A"), fmt::arg("_1", 1)));
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{a}"), fmt::format_error, "argument not found");
  EXPECT_EQ(" -42", Bof_SafeFmtSprintf("{0:{width}}", -42, fmt::arg("width", 4)));
  EXPECT_EQ("st", Bof_SafeFmtSprintf("{0:.{precision}}", "str", fmt::arg("precision", 2)));
  EXPECT_EQ("1 2", Bof_SafeFmtSprintf("{} {two}", 1, fmt::arg("two", 2)));
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{c}",
    fmt::arg("a", 0), fmt::arg("b", 0), fmt::arg("c", 42), fmt::arg("d", 0),
    fmt::arg("e", 0), fmt::arg("f", 0), fmt::arg("g", 0), fmt::arg("h", 0),
    fmt::arg("i", 0), fmt::arg("j", 0), fmt::arg("k", 0), fmt::arg("l", 0),
    fmt::arg("m", 0), fmt::arg("n", 0), fmt::arg("o", 0), fmt::arg("p", 0)));
}

TEST(FormatterTest, AutoArgIndex) {
  EXPECT_EQ("abc", Bof_SafeFmtSprintf("{}{}{}", 'a', 'b', 'c'));
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0}{}", 'a', 'b'),
    fmt::format_error, "cannot switch from manual to automatic argument indexing");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{}{0}", 'a', 'b'),
    fmt::format_error, "cannot switch from automatic to manual argument indexing");
  EXPECT_EQ("1.2", Bof_SafeFmtSprintf("{:.{}}", 1.2345, 2));
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0}:.{}", 1.2345, 2),
    fmt::format_error, "cannot switch from manual to automatic argument indexing");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{:.{0}}", 1.2345, 2),
    fmt::format_error, "cannot switch from automatic to manual argument indexing");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{}"), fmt::format_error, "argument index out of range");
}

TEST(FormatterTest, EmptySpecs) {
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{0:}", 42));
}

TEST(FormatterTest, LeftAlign) {
  EXPECT_EQ("42  ", Bof_SafeFmtSprintf("{0:<4}", 42));
  EXPECT_EQ("42  ", Bof_SafeFmtSprintf("{0:<4o}", 042));
  EXPECT_EQ("42  ", Bof_SafeFmtSprintf("{0:<4x}", 0x42));
  EXPECT_EQ("-42  ", Bof_SafeFmtSprintf("{0:<5}", -42));
  EXPECT_EQ("42   ", Bof_SafeFmtSprintf("{0:<5}", 42u));
  EXPECT_EQ("-42  ", Bof_SafeFmtSprintf("{0:<5}", -42l));
  EXPECT_EQ("42   ", Bof_SafeFmtSprintf("{0:<5}", 42ul));
  EXPECT_EQ("-42  ", Bof_SafeFmtSprintf("{0:<5}", -42ll));
  EXPECT_EQ("42   ", Bof_SafeFmtSprintf("{0:<5}", 42ull));
  EXPECT_EQ("-42  ", Bof_SafeFmtSprintf("{0:<5}", -42.0));
  EXPECT_EQ("-42  ", Bof_SafeFmtSprintf("{0:<5}", -42.0l));
  EXPECT_EQ("c    ", Bof_SafeFmtSprintf("{0:<5}", 'c'));
  EXPECT_EQ("abc  ", Bof_SafeFmtSprintf("{0:<5}", "abc"));
  EXPECT_EQ("0xface  ", Bof_SafeFmtSprintf("{0:<8}", reinterpret_cast<void*>(0xface)));
}

TEST(FormatterTest, RightAlign) {
  EXPECT_EQ("  42", Bof_SafeFmtSprintf("{0:>4}", 42));
  EXPECT_EQ("  42", Bof_SafeFmtSprintf("{0:>4o}", 042));
  EXPECT_EQ("  42", Bof_SafeFmtSprintf("{0:>4x}", 0x42));
  EXPECT_EQ("  -42", Bof_SafeFmtSprintf("{0:>5}", -42));
  EXPECT_EQ("   42", Bof_SafeFmtSprintf("{0:>5}", 42u));
  EXPECT_EQ("  -42", Bof_SafeFmtSprintf("{0:>5}", -42l));
  EXPECT_EQ("   42", Bof_SafeFmtSprintf("{0:>5}", 42ul));
  EXPECT_EQ("  -42", Bof_SafeFmtSprintf("{0:>5}", -42ll));
  EXPECT_EQ("   42", Bof_SafeFmtSprintf("{0:>5}", 42ull));
  EXPECT_EQ("  -42", Bof_SafeFmtSprintf("{0:>5}", -42.0));
  EXPECT_EQ("  -42", Bof_SafeFmtSprintf("{0:>5}", -42.0l));
  EXPECT_EQ("    c", Bof_SafeFmtSprintf("{0:>5}", 'c'));
  EXPECT_EQ("  abc", Bof_SafeFmtSprintf("{0:>5}", "abc"));
  EXPECT_EQ("  0xface", Bof_SafeFmtSprintf("{0:>8}", reinterpret_cast<void*>(0xface)));
}

TEST(FormatterTest, NumericAlign) {
  EXPECT_EQ("  42", Bof_SafeFmtSprintf("{0:=4}", 42));
  EXPECT_EQ("+ 42", Bof_SafeFmtSprintf("{0:=+4}", 42));
  EXPECT_EQ("  42", Bof_SafeFmtSprintf("{0:=4o}", 042));
  EXPECT_EQ("+ 42", Bof_SafeFmtSprintf("{0:=+4o}", 042));
  EXPECT_EQ("  42", Bof_SafeFmtSprintf("{0:=4x}", 0x42));
  EXPECT_EQ("+ 42", Bof_SafeFmtSprintf("{0:=+4x}", 0x42));
  EXPECT_EQ("-  42", Bof_SafeFmtSprintf("{0:=5}", -42));
  EXPECT_EQ("   42", Bof_SafeFmtSprintf("{0:=5}", 42u));
  EXPECT_EQ("-  42", Bof_SafeFmtSprintf("{0:=5}", -42l));
  EXPECT_EQ("   42", Bof_SafeFmtSprintf("{0:=5}", 42ul));
  EXPECT_EQ("-  42", Bof_SafeFmtSprintf("{0:=5}", -42ll));
  EXPECT_EQ("   42", Bof_SafeFmtSprintf("{0:=5}", 42ull));
  EXPECT_EQ("-  42", Bof_SafeFmtSprintf("{0:=5}", -42.0));
  EXPECT_EQ("-  42", Bof_SafeFmtSprintf("{0:=5}", -42.0l));
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:=5", 'c'),
    fmt::format_error, "missing '}' in format string");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:=5}", 'c'),
    fmt::format_error, "invalid format specifier for char");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:=5}", "abc"),
    fmt::format_error, "format specifier requires numeric argument");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:=8}", reinterpret_cast<void*>(0xface)),
    fmt::format_error, "format specifier requires numeric argument");
  EXPECT_EQ(" 1", Bof_SafeFmtSprintf("{:= }", 1.0));
}

TEST(FormatterTest, CenterAlign) {
  EXPECT_EQ(" 42  ", Bof_SafeFmtSprintf("{0:^5}", 42));
  EXPECT_EQ(" 42  ", Bof_SafeFmtSprintf("{0:^5o}", 042));
  EXPECT_EQ(" 42  ", Bof_SafeFmtSprintf("{0:^5x}", 0x42));
  EXPECT_EQ(" -42 ", Bof_SafeFmtSprintf("{0:^5}", -42));
  EXPECT_EQ(" 42  ", Bof_SafeFmtSprintf("{0:^5}", 42u));
  EXPECT_EQ(" -42 ", Bof_SafeFmtSprintf("{0:^5}", -42l));
  EXPECT_EQ(" 42  ", Bof_SafeFmtSprintf("{0:^5}", 42ul));
  EXPECT_EQ(" -42 ", Bof_SafeFmtSprintf("{0:^5}", -42ll));
  EXPECT_EQ(" 42  ", Bof_SafeFmtSprintf("{0:^5}", 42ull));
  EXPECT_EQ(" -42  ", Bof_SafeFmtSprintf("{0:^6}", -42.0));
  EXPECT_EQ(" -42 ", Bof_SafeFmtSprintf("{0:^5}", -42.0l));
  EXPECT_EQ("  c  ", Bof_SafeFmtSprintf("{0:^5}", 'c'));
  EXPECT_EQ(" abc  ", Bof_SafeFmtSprintf("{0:^6}", "abc"));
  EXPECT_EQ(" 0xface ", Bof_SafeFmtSprintf("{0:^8}", reinterpret_cast<void*>(0xface)));
}

TEST(FormatterTest, Fill) {
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:{<5}", 'c'),
    fmt::format_error, "invalid fill character '{'");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:{<5}}", 'c'),
    fmt::format_error, "invalid fill character '{'");
  EXPECT_EQ("**42", Bof_SafeFmtSprintf("{0:*>4}", 42));
  EXPECT_EQ("**-42", Bof_SafeFmtSprintf("{0:*>5}", -42));
  EXPECT_EQ("***42", Bof_SafeFmtSprintf("{0:*>5}", 42u));
  EXPECT_EQ("**-42", Bof_SafeFmtSprintf("{0:*>5}", -42l));
  EXPECT_EQ("***42", Bof_SafeFmtSprintf("{0:*>5}", 42ul));
  EXPECT_EQ("**-42", Bof_SafeFmtSprintf("{0:*>5}", -42ll));
  EXPECT_EQ("***42", Bof_SafeFmtSprintf("{0:*>5}", 42ull));
  EXPECT_EQ("**-42", Bof_SafeFmtSprintf("{0:*>5}", -42.0));
  EXPECT_EQ("**-42", Bof_SafeFmtSprintf("{0:*>5}", -42.0l));
  EXPECT_EQ("c****", Bof_SafeFmtSprintf("{0:*<5}", 'c'));
  EXPECT_EQ("abc**", Bof_SafeFmtSprintf("{0:*<5}", "abc"));
  EXPECT_EQ("**0xface", Bof_SafeFmtSprintf("{0:*>8}", reinterpret_cast<void*>(0xface)));
  EXPECT_EQ("foo=", Bof_SafeFmtSprintf("{:}=", "foo"));
  EXPECT_EQ(std::string("\0\0\0*", 4), fmt::format(fmt::string_view("{:\0>4}", 6), '*'));
}

TEST(FormatterTest, PlusSign) {
  EXPECT_EQ("+42", Bof_SafeFmtSprintf("{0:+}", 42));
  EXPECT_EQ("-42", Bof_SafeFmtSprintf("{0:+}", -42));
  EXPECT_EQ("+42", Bof_SafeFmtSprintf("{0:+}", 42));
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:+}", 42u),
    fmt::format_error, "format specifier requires signed argument");
  EXPECT_EQ("+42", Bof_SafeFmtSprintf("{0:+}", 42l));
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:+}", 42ul),
    fmt::format_error, "format specifier requires signed argument");
  EXPECT_EQ("+42", Bof_SafeFmtSprintf("{0:+}", 42ll));
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:+}", 42ull),
    fmt::format_error, "format specifier requires signed argument");
  EXPECT_EQ("+42", Bof_SafeFmtSprintf("{0:+}", 42.0));
  EXPECT_EQ("+42", Bof_SafeFmtSprintf("{0:+}", 42.0l));
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:+", 'c'),
    fmt::format_error, "missing '}' in format string");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:+}", 'c'),
    fmt::format_error, "invalid format specifier for char");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:+}", "abc"),
    fmt::format_error, "format specifier requires numeric argument");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:+}", reinterpret_cast<void*>(0x42)),
    fmt::format_error, "format specifier requires numeric argument");
}

TEST(FormatterTest, MinusSign) {
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{0:-}", 42));
  EXPECT_EQ("-42", Bof_SafeFmtSprintf("{0:-}", -42));
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{0:-}", 42));
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:-}", 42u),
    fmt::format_error, "format specifier requires signed argument");
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{0:-}", 42l));
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:-}", 42ul),
    fmt::format_error, "format specifier requires signed argument");
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{0:-}", 42ll));
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:-}", 42ull),
    fmt::format_error, "format specifier requires signed argument");
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{0:-}", 42.0));
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{0:-}", 42.0l));
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:-", 'c'),
    fmt::format_error, "missing '}' in format string");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:-}", 'c'),
    fmt::format_error, "invalid format specifier for char");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:-}", "abc"),
    fmt::format_error, "format specifier requires numeric argument");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:-}", reinterpret_cast<void*>(0x42)),
    fmt::format_error, "format specifier requires numeric argument");
}

TEST(FormatterTest, SpaceSign) {
  EXPECT_EQ(" 42", Bof_SafeFmtSprintf("{0: }", 42));
  EXPECT_EQ("-42", Bof_SafeFmtSprintf("{0: }", -42));
  EXPECT_EQ(" 42", Bof_SafeFmtSprintf("{0: }", 42));
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0: }", 42u),
    fmt::format_error, "format specifier requires signed argument");
  EXPECT_EQ(" 42", Bof_SafeFmtSprintf("{0: }", 42l));
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0: }", 42ul),
    fmt::format_error, "format specifier requires signed argument");
  EXPECT_EQ(" 42", Bof_SafeFmtSprintf("{0: }", 42ll));
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0: }", 42ull),
    fmt::format_error, "format specifier requires signed argument");
  EXPECT_EQ(" 42", Bof_SafeFmtSprintf("{0: }", 42.0));
  EXPECT_EQ(" 42", Bof_SafeFmtSprintf("{0: }", 42.0l));
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0: ", 'c'),
    fmt::format_error, "missing '}' in format string");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0: }", 'c'),
    fmt::format_error, "invalid format specifier for char");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0: }", "abc"),
    fmt::format_error, "format specifier requires numeric argument");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0: }", reinterpret_cast<void*>(0x42)),
    fmt::format_error, "format specifier requires numeric argument");
}

TEST(FormatterTest, HashFlag) {
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{0:#}", 42));
  EXPECT_EQ("-42", Bof_SafeFmtSprintf("{0:#}", -42));
  EXPECT_EQ("0b101010", Bof_SafeFmtSprintf("{0:#b}", 42));
  EXPECT_EQ("0B101010", Bof_SafeFmtSprintf("{0:#B}", 42));
  EXPECT_EQ("-0b101010", Bof_SafeFmtSprintf("{0:#b}", -42));
  EXPECT_EQ("0x42", Bof_SafeFmtSprintf("{0:#x}", 0x42));
  EXPECT_EQ("0X42", Bof_SafeFmtSprintf("{0:#X}", 0x42));
  EXPECT_EQ("-0x42", Bof_SafeFmtSprintf("{0:#x}", -0x42));
  EXPECT_EQ("042", Bof_SafeFmtSprintf("{0:#o}", 042));
  EXPECT_EQ("-042", Bof_SafeFmtSprintf("{0:#o}", -042));
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{0:#}", 42u));
  EXPECT_EQ("0x42", Bof_SafeFmtSprintf("{0:#x}", 0x42u));
  EXPECT_EQ("042", Bof_SafeFmtSprintf("{0:#o}", 042u));

  EXPECT_EQ("-42", Bof_SafeFmtSprintf("{0:#}", -42l));
  EXPECT_EQ("0x42", Bof_SafeFmtSprintf("{0:#x}", 0x42l));
  EXPECT_EQ("-0x42", Bof_SafeFmtSprintf("{0:#x}", -0x42l));
  EXPECT_EQ("042", Bof_SafeFmtSprintf("{0:#o}", 042l));
  EXPECT_EQ("-042", Bof_SafeFmtSprintf("{0:#o}", -042l));
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{0:#}", 42ul));
  EXPECT_EQ("0x42", Bof_SafeFmtSprintf("{0:#x}", 0x42ul));
  EXPECT_EQ("042", Bof_SafeFmtSprintf("{0:#o}", 042ul));

  EXPECT_EQ("-42", Bof_SafeFmtSprintf("{0:#}", -42ll));
  EXPECT_EQ("0x42", Bof_SafeFmtSprintf("{0:#x}", 0x42ll));
  EXPECT_EQ("-0x42", Bof_SafeFmtSprintf("{0:#x}", -0x42ll));
  EXPECT_EQ("042", Bof_SafeFmtSprintf("{0:#o}", 042ll));
  EXPECT_EQ("-042", Bof_SafeFmtSprintf("{0:#o}", -042ll));
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{0:#}", 42ull));
  EXPECT_EQ("0x42", Bof_SafeFmtSprintf("{0:#x}", 0x42ull));
  EXPECT_EQ("042", Bof_SafeFmtSprintf("{0:#o}", 042ull));

  if (FMT_USE_GRISU)
    EXPECT_EQ("-42.0", Bof_SafeFmtSprintf("{0:#}", -42.0));
  else
    EXPECT_EQ("-42.0000", Bof_SafeFmtSprintf("{0:#}", -42.0));
  EXPECT_EQ("-42.0000", Bof_SafeFmtSprintf("{0:#}", -42.0l));
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:#", 'c'),
    fmt::format_error, "missing '}' in format string");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:#}", 'c'),
    fmt::format_error, "invalid format specifier for char");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:#}", "abc"),
    fmt::format_error, "format specifier requires numeric argument");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:#}", reinterpret_cast<void*>(0x42)),
    fmt::format_error, "format specifier requires numeric argument");
}

TEST(FormatterTest, ZeroFlag) {
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{0:0}", 42));
  EXPECT_EQ("-0042", Bof_SafeFmtSprintf("{0:05}", -42));
  EXPECT_EQ("00042", Bof_SafeFmtSprintf("{0:05}", 42u));
  EXPECT_EQ("-0042", Bof_SafeFmtSprintf("{0:05}", -42l));
  EXPECT_EQ("00042", Bof_SafeFmtSprintf("{0:05}", 42ul));
  EXPECT_EQ("-0042", Bof_SafeFmtSprintf("{0:05}", -42ll));
  EXPECT_EQ("00042", Bof_SafeFmtSprintf("{0:05}", 42ull));
  EXPECT_EQ("-0042", Bof_SafeFmtSprintf("{0:05}", -42.0));
  EXPECT_EQ("-0042", Bof_SafeFmtSprintf("{0:05}", -42.0l));
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:0", 'c'),
    fmt::format_error, "missing '}' in format string");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:05}", 'c'),
    fmt::format_error, "invalid format specifier for char");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:05}", "abc"),
    fmt::format_error, "format specifier requires numeric argument");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:05}", reinterpret_cast<void*>(0x42)),
    fmt::format_error, "format specifier requires numeric argument");
}

TEST(FormatterTest, Width) {
  char format_str[BUFFER_SIZE];
  safe_sprintf(format_str, "{0:%u", UINT_MAX);
  increment(format_str + 3);
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf(format_str, 0), fmt::format_error, "number is too big");
  std::size_t size = std::strlen(format_str);
  format_str[size] = '}';
  format_str[size + 1] = 0;
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf(format_str, 0), fmt::format_error, "number is too big");

  safe_sprintf(format_str, "{0:%u", INT_MAX + 1u);
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf(format_str, 0), fmt::format_error, "number is too big");
  safe_sprintf(format_str, "{0:%u}", INT_MAX + 1u);
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf(format_str, 0), fmt::format_error, "number is too big");
  EXPECT_EQ(" -42", Bof_SafeFmtSprintf("{0:4}", -42));
  EXPECT_EQ("   42", Bof_SafeFmtSprintf("{0:5}", 42u));
  EXPECT_EQ("   -42", Bof_SafeFmtSprintf("{0:6}", -42l));
  EXPECT_EQ("     42", Bof_SafeFmtSprintf("{0:7}", 42ul));
  EXPECT_EQ("   -42", Bof_SafeFmtSprintf("{0:6}", -42ll));
  EXPECT_EQ("     42", Bof_SafeFmtSprintf("{0:7}", 42ull));
  EXPECT_EQ("   -1.23", Bof_SafeFmtSprintf("{0:8}", -1.23));
  EXPECT_EQ("    -1.23", Bof_SafeFmtSprintf("{0:9}", -1.23l));
  EXPECT_EQ("    0xcafe", Bof_SafeFmtSprintf("{0:10}", reinterpret_cast<void*>(0xcafe)));
  EXPECT_EQ("x          ", Bof_SafeFmtSprintf("{0:11}", 'x'));
  EXPECT_EQ("str         ", Bof_SafeFmtSprintf("{0:12}", "str"));
}

TEST(FormatterTest, RuntimeWidth) {
  char format_str[BUFFER_SIZE];
  safe_sprintf(format_str, "{0:{%u", UINT_MAX);
  increment(format_str + 4);
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf(format_str, 0), fmt::format_error, "number is too big");
  std::size_t size = std::strlen(format_str);
  format_str[size] = '}';
  format_str[size + 1] = 0;
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf(format_str, 0), fmt::format_error, "number is too big");
  format_str[size + 1] = '}';
  format_str[size + 2] = 0;
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf(format_str, 0), fmt::format_error, "number is too big");

  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:{", 0),
    fmt::format_error, "invalid format string");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:{}", 0),
    fmt::format_error, "cannot switch from manual to automatic argument indexing");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:{?}}", 0),
    fmt::format_error, "invalid format string");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:{1}}", 0),
    fmt::format_error, "argument index out of range");

  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:{0:}}", 0),
    fmt::format_error, "invalid format string");

  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:{1}}", 0, -1),
    fmt::format_error, "negative width");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:{1}}", 0, (INT_MAX + 1u)),
    fmt::format_error, "number is too big");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:{1}}", 0, -1l),
    fmt::format_error, "negative width");
  if (fmt::internal::const_check(sizeof(long) > sizeof(int))) {
    long value = INT_MAX;
    EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:{1}}", 0, (value + 1)),
      fmt::format_error, "number is too big");
  }
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:{1}}", 0, (INT_MAX + 1ul)),
    fmt::format_error, "number is too big");

  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:{1}}", 0, '0'),
    fmt::format_error, "width is not integer");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:{1}}", 0, 0.0),
    fmt::format_error, "width is not integer");

  EXPECT_EQ(" -42", Bof_SafeFmtSprintf("{0:{1}}", -42, 4));
  EXPECT_EQ("   42", Bof_SafeFmtSprintf("{0:{1}}", 42u, 5));
  EXPECT_EQ("   -42", Bof_SafeFmtSprintf("{0:{1}}", -42l, 6));
  EXPECT_EQ("     42", Bof_SafeFmtSprintf("{0:{1}}", 42ul, 7));
  EXPECT_EQ("   -42", Bof_SafeFmtSprintf("{0:{1}}", -42ll, 6));
  EXPECT_EQ("     42", Bof_SafeFmtSprintf("{0:{1}}", 42ull, 7));
  EXPECT_EQ("   -1.23", Bof_SafeFmtSprintf("{0:{1}}", -1.23, 8));
  EXPECT_EQ("    -1.23", Bof_SafeFmtSprintf("{0:{1}}", -1.23l, 9));
  EXPECT_EQ("    0xcafe",
    Bof_SafeFmtSprintf("{0:{1}}", reinterpret_cast<void*>(0xcafe), 10));
  EXPECT_EQ("x          ", Bof_SafeFmtSprintf("{0:{1}}", 'x', 11));
  EXPECT_EQ("str         ", Bof_SafeFmtSprintf("{0:{1}}", "str", 12));
}

TEST(FormatterTest, Precision) {
  char format_str[BUFFER_SIZE];
  safe_sprintf(format_str, "{0:.%u", UINT_MAX);
  increment(format_str + 4);
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf(format_str, 0), fmt::format_error, "number is too big");
  std::size_t size = std::strlen(format_str);
  format_str[size] = '}';
  format_str[size + 1] = 0;
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf(format_str, 0), fmt::format_error, "number is too big");

  safe_sprintf(format_str, "{0:.%u", INT_MAX + 1u);
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf(format_str, 0), fmt::format_error, "number is too big");
  safe_sprintf(format_str, "{0:.%u}", INT_MAX + 1u);
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf(format_str, 0), fmt::format_error, "number is too big");

  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.", 0),
    fmt::format_error, "missing precision specifier");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.}", 0),
    fmt::format_error, "missing precision specifier");

  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.2", 0),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.2}", 42),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.2f}", 42),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.2}", 42u),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.2f}", 42u),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.2}", 42l),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.2f}", 42l),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.2}", 42ul),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.2f}", 42ul),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.2}", 42ll),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.2f}", 42ll),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.2}", 42ull),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.2f}", 42ull),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:3.0}", 'x'),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_EQ("1.2", Bof_SafeFmtSprintf("{0:.2}", 1.2345));
  EXPECT_EQ("1.2", Bof_SafeFmtSprintf("{0:.2}", 1.2345l));

  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.2}", reinterpret_cast<void*>(0xcafe)),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.2f}", reinterpret_cast<void*>(0xcafe)),
    fmt::format_error, "precision not allowed for this argument type");

  EXPECT_EQ("st", Bof_SafeFmtSprintf("{0:.2}", "str"));
}

TEST(FormatterTest, RuntimePrecision) {
  char format_str[BUFFER_SIZE];
  safe_sprintf(format_str, "{0:.{%u", UINT_MAX);
  increment(format_str + 5);
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf(format_str, 0), fmt::format_error, "number is too big");
  std::size_t size = std::strlen(format_str);
  format_str[size] = '}';
  format_str[size + 1] = 0;
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf(format_str, 0), fmt::format_error, "number is too big");
  format_str[size + 1] = '}';
  format_str[size + 2] = 0;
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf(format_str, 0), fmt::format_error, "number is too big");

  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{", 0),
    fmt::format_error, "invalid format string");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{}", 0),
    fmt::format_error, "cannot switch from manual to automatic argument indexing");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{?}}", 0),
    fmt::format_error, "invalid format string");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{1}", 0, 0),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{1}}", 0),
    fmt::format_error, "argument index out of range");

  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{0:}}", 0),
    fmt::format_error, "invalid format string");

  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{1}}", 0, -1),
    fmt::format_error, "negative precision");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{1}}", 0, (INT_MAX + 1u)),
    fmt::format_error, "number is too big");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{1}}", 0, -1l),
    fmt::format_error, "negative precision");
  if (fmt::internal::const_check(sizeof(long) > sizeof(int))) {
    long value = INT_MAX;
    EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{1}}", 0, (value + 1)),
      fmt::format_error, "number is too big");
  }
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{1}}", 0, (INT_MAX + 1ul)),
    fmt::format_error, "number is too big");

  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{1}}", 0, '0'),
    fmt::format_error, "precision is not integer");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{1}}", 0, 0.0),
    fmt::format_error, "precision is not integer");

  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{1}}", 42, 2),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{1}f}", 42, 2),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{1}}", 42u, 2),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{1}f}", 42u, 2),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{1}}", 42l, 2),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{1}f}", 42l, 2),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{1}}", 42ul, 2),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{1}f}", 42ul, 2),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{1}}", 42ll, 2),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{1}f}", 42ll, 2),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{1}}", 42ull, 2),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{1}f}", 42ull, 2),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:3.{1}}", 'x', 0),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_EQ("1.2", Bof_SafeFmtSprintf("{0:.{1}}", 1.2345, 2));
  EXPECT_EQ("1.2", Bof_SafeFmtSprintf("{1:.{0}}", 2, 1.2345l));

  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{1}}", reinterpret_cast<void*>(0xcafe), 2),
    fmt::format_error, "precision not allowed for this argument type");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:.{1}f}", reinterpret_cast<void*>(0xcafe), 2),
    fmt::format_error, "precision not allowed for this argument type");

  EXPECT_EQ("st", Bof_SafeFmtSprintf("{0:.{1}}", "str", 2));
}

template <typename T>
void check_unknown_types(const T &value, const char *types, const char *) {
  char format_str[BUFFER_SIZE];
  const char *special = ".0123456789}";
  for (int i = CHAR_MIN; i <= CHAR_MAX; ++i) {
    char c = static_cast<char>(i);
    if (std::strchr(types, c) || std::strchr(special, c) || !c) continue;
    safe_sprintf(format_str, "{0:10%c}", c);
    const char *message = "invalid type specifier";
    EXPECT_THROW_MSG(Bof_SafeFmtSprintf(format_str, value), fmt::format_error, message)
      << format_str << " " << message;
  }
}

TEST(BoolTest, FormatBool) {
  EXPECT_EQ("true", Bof_SafeFmtSprintf("{}", true));
  EXPECT_EQ("false", Bof_SafeFmtSprintf("{}", false));
  EXPECT_EQ("1", Bof_SafeFmtSprintf("{:d}", true));
  EXPECT_EQ("true ", Bof_SafeFmtSprintf("{:5}", true));
  EXPECT_EQ(L"true", Bof_SafeFmtSprintf(L"{}", true));
}

TEST(FormatterTest, FormatShort) {
  short s = 42;
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{0:d}", s));
  unsigned short us = 42;
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{0:d}", us));
}

TEST(FormatterTest, FormatInt) {
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:v", 42),
    fmt::format_error, "missing '}' in format string");
  check_unknown_types(42, "bBdoxXn", "integer");
}

TEST(FormatterTest, FormatBin) {
  EXPECT_EQ("0", Bof_SafeFmtSprintf("{0:b}", 0));
  EXPECT_EQ("101010", Bof_SafeFmtSprintf("{0:b}", 42));
  EXPECT_EQ("101010", Bof_SafeFmtSprintf("{0:b}", 42u));
  EXPECT_EQ("-101010", Bof_SafeFmtSprintf("{0:b}", -42));
  EXPECT_EQ("11000000111001", Bof_SafeFmtSprintf("{0:b}", 12345));
  EXPECT_EQ("10010001101000101011001111000", Bof_SafeFmtSprintf("{0:b}", 0x12345678));
  EXPECT_EQ("10010000101010111100110111101111", Bof_SafeFmtSprintf("{0:b}", 0x90ABCDEF));
  EXPECT_EQ("11111111111111111111111111111111",
    Bof_SafeFmtSprintf("{0:b}", std::numeric_limits<uint32_t>::max()));
}

TEST(FormatterTest, FormatDec) {
  EXPECT_EQ("0", Bof_SafeFmtSprintf("{0}", 0));
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{0}", 42));
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{0:d}", 42));
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{0}", 42u));
  EXPECT_EQ("-42", Bof_SafeFmtSprintf("{0}", -42));
  EXPECT_EQ("12345", Bof_SafeFmtSprintf("{0}", 12345));
  EXPECT_EQ("67890", Bof_SafeFmtSprintf("{0}", 67890));
  char buffer[BUFFER_SIZE];
  safe_sprintf(buffer, "%d", INT_MIN);
  EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{0}", INT_MIN));
  safe_sprintf(buffer, "%d", INT_MAX);
  EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{0}", INT_MAX));
  safe_sprintf(buffer, "%u", UINT_MAX);
  EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{0}", UINT_MAX));
  safe_sprintf(buffer, "%ld", 0 - static_cast<unsigned long>(LONG_MIN));
  EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{0}", LONG_MIN));
  safe_sprintf(buffer, "%ld", LONG_MAX);
  EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{0}", LONG_MAX));
  safe_sprintf(buffer, "%lu", ULONG_MAX);
  EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{0}", ULONG_MAX));
}

TEST(FormatterTest, FormatHex) {
  EXPECT_EQ("0", Bof_SafeFmtSprintf("{0:x}", 0));
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{0:x}", 0x42));
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{0:x}", 0x42u));
  EXPECT_EQ("-42", Bof_SafeFmtSprintf("{0:x}", -0x42));
  EXPECT_EQ("12345678", Bof_SafeFmtSprintf("{0:x}", 0x12345678));
  EXPECT_EQ("90abcdef", Bof_SafeFmtSprintf("{0:x}", 0x90abcdef));
  EXPECT_EQ("12345678", Bof_SafeFmtSprintf("{0:X}", 0x12345678));
  EXPECT_EQ("90ABCDEF", Bof_SafeFmtSprintf("{0:X}", 0x90ABCDEF));

  char buffer[BUFFER_SIZE];
  safe_sprintf(buffer, "-%x", 0 - static_cast<unsigned>(INT_MIN));
  EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{0:x}", INT_MIN));
  safe_sprintf(buffer, "%x", INT_MAX);
  EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{0:x}", INT_MAX));
  safe_sprintf(buffer, "%x", UINT_MAX);
  EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{0:x}", UINT_MAX));
  safe_sprintf(buffer, "-%lx", 0 - static_cast<unsigned long>(LONG_MIN));
  EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{0:x}", LONG_MIN));
  safe_sprintf(buffer, "%lx", LONG_MAX);
  EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{0:x}", LONG_MAX));
  safe_sprintf(buffer, "%lx", ULONG_MAX);
  EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{0:x}", ULONG_MAX));
}

TEST(FormatterTest, FormatOct) {
  EXPECT_EQ("0", Bof_SafeFmtSprintf("{0:o}", 0));
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{0:o}", 042));
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{0:o}", 042u));
  EXPECT_EQ("-42", Bof_SafeFmtSprintf("{0:o}", -042));
  EXPECT_EQ("12345670", Bof_SafeFmtSprintf("{0:o}", 012345670));
  char buffer[BUFFER_SIZE];
  safe_sprintf(buffer, "-%o", 0 - static_cast<unsigned>(INT_MIN));
  EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{0:o}", INT_MIN));
  safe_sprintf(buffer, "%o", INT_MAX);
  EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{0:o}", INT_MAX));
  safe_sprintf(buffer, "%o", UINT_MAX);
  EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{0:o}", UINT_MAX));
  safe_sprintf(buffer, "-%lo", 0 - static_cast<unsigned long>(LONG_MIN));
  EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{0:o}", LONG_MIN));
  safe_sprintf(buffer, "%lo", LONG_MAX);
  EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{0:o}", LONG_MAX));
  safe_sprintf(buffer, "%lo", ULONG_MAX);
  EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{0:o}", ULONG_MAX));
}

TEST(FormatterTest, FormatIntLocale) {
  EXPECT_EQ("123", Bof_SafeFmtSprintf("{:n}", 123));
  EXPECT_EQ("1,234", Bof_SafeFmtSprintf("{:n}", 1234));
  EXPECT_EQ("1,234,567", Bof_SafeFmtSprintf("{:n}", 1234567));
  EXPECT_EQ("4,294,967,295",
    Bof_SafeFmtSprintf("{:n}", std::numeric_limits<uint32_t>::max()));
}

struct ConvertibleToLongLong {
  operator long long() const { return 1LL << 32; }
};

TEST(FormatterTest, FormatConvertibleToLongLong) {
  EXPECT_EQ("100000000", Bof_SafeFmtSprintf("{:x}", ConvertibleToLongLong()));
}

TEST(FormatterTest, FormatFloat) {
  EXPECT_EQ("392.500000", Bof_SafeFmtSprintf("{0:f}", 392.5f));
}

TEST(FormatterTest, FormatDouble) {
  check_unknown_types(1.2, "eEfFgGaA", "double");
  EXPECT_EQ("0", Bof_SafeFmtSprintf("{:}", 0.0));
  EXPECT_EQ("0.000000", Bof_SafeFmtSprintf("{:f}", 0.0));
  EXPECT_EQ("0", Bof_SafeFmtSprintf("{:g}", 0.0));
  EXPECT_EQ("392.65", Bof_SafeFmtSprintf("{:}", 392.65));
  EXPECT_EQ("392.65", Bof_SafeFmtSprintf("{:g}", 392.65));
  EXPECT_EQ("392.65", Bof_SafeFmtSprintf("{:G}", 392.65));
  EXPECT_EQ("392.650000", Bof_SafeFmtSprintf("{:f}", 392.65));
  EXPECT_EQ("392.650000", Bof_SafeFmtSprintf("{:F}", 392.65));
  char buffer[BUFFER_SIZE];
  safe_sprintf(buffer, "%e", 392.65);
  EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{0:e}", 392.65));
  safe_sprintf(buffer, "%E", 392.65);
  EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{0:E}", 392.65));
  EXPECT_EQ("+0000392.6", Bof_SafeFmtSprintf("{0:+010.4g}", 392.65));
  safe_sprintf(buffer, "%a", -42.0);
  EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{:a}", -42.0));
  safe_sprintf(buffer, "%A", -42.0);
  EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{:A}", -42.0));
}

TEST(FormatterTest, FormatDoubleBigPrecision) {
  // sprintf with big precision is broken in MSVC2013, so only test on Grisu.
  if (FMT_USE_GRISU)
    EXPECT_EQ(Bof_SafeFmtSprintf("0.{:0<1000}", ""), Bof_SafeFmtSprintf("{:.1000f}", 0.0));
}

TEST(FormatterTest, FormatNaN) {
  double nan = std::numeric_limits<double>::quiet_NaN();
  EXPECT_EQ("nan", Bof_SafeFmtSprintf("{}", nan));
  EXPECT_EQ("+nan", Bof_SafeFmtSprintf("{:+}", nan));
  EXPECT_EQ(" nan", Bof_SafeFmtSprintf("{: }", nan));
  EXPECT_EQ("NAN", Bof_SafeFmtSprintf("{:F}", nan));
  EXPECT_EQ("nan    ", Bof_SafeFmtSprintf("{:<7}", nan));
  EXPECT_EQ("  nan  ", Bof_SafeFmtSprintf("{:^7}", nan));
  EXPECT_EQ("    nan", Bof_SafeFmtSprintf("{:>7}", nan));
}

TEST(FormatterTest, FormatInfinity) {
  double inf = std::numeric_limits<double>::infinity();
  EXPECT_EQ("inf", Bof_SafeFmtSprintf("{}", inf));
  EXPECT_EQ("+inf", Bof_SafeFmtSprintf("{:+}", inf));
  EXPECT_EQ("-inf", Bof_SafeFmtSprintf("{}", -inf));
  EXPECT_EQ(" inf", Bof_SafeFmtSprintf("{: }", inf));
  EXPECT_EQ("INF", Bof_SafeFmtSprintf("{:F}", inf));
  EXPECT_EQ("inf    ", Bof_SafeFmtSprintf("{:<7}", inf));
  EXPECT_EQ("  inf  ", Bof_SafeFmtSprintf("{:^7}", inf));
  EXPECT_EQ("    inf", Bof_SafeFmtSprintf("{:>7}", inf));
}

TEST(FormatterTest, FormatLongDouble) {
  EXPECT_EQ("0", Bof_SafeFmtSprintf("{0:}", 0.0l));
  EXPECT_EQ("0.000000", Bof_SafeFmtSprintf("{0:f}", 0.0l));
  EXPECT_EQ("392.65", Bof_SafeFmtSprintf("{0:}", 392.65l));
  EXPECT_EQ("392.65", Bof_SafeFmtSprintf("{0:g}", 392.65l));
  EXPECT_EQ("392.65", Bof_SafeFmtSprintf("{0:G}", 392.65l));
  EXPECT_EQ("392.650000", Bof_SafeFmtSprintf("{0:f}", 392.65l));
  EXPECT_EQ("392.650000", Bof_SafeFmtSprintf("{0:F}", 392.65l));
  char buffer[BUFFER_SIZE];
  safe_sprintf(buffer, "%Le", 392.65l);
  EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{0:e}", 392.65l));
  EXPECT_EQ("+0000392.6", Bof_SafeFmtSprintf("{0:+010.4g}", 392.64l));
}

TEST(FormatterTest, FormatChar) {
  const char types[] = "cbBdoxXn";
  check_unknown_types('a', types, "char");
  EXPECT_EQ("a", Bof_SafeFmtSprintf("{0}", 'a'));
  EXPECT_EQ("z", Bof_SafeFmtSprintf("{0:c}", 'z'));
  EXPECT_EQ(L"a", Bof_SafeFmtSprintf(L"{0}", 'a'));
  int n = 'x';
  for (const char *type = types + 1; *type; ++type) {
    std::string format_str = Bof_SafeFmtSprintf("{{:{}}}", *type);
    EXPECT_EQ(Bof_SafeFmtSprintf(format_str, n), Bof_SafeFmtSprintf(format_str, 'x'));
  }
  EXPECT_EQ(Bof_SafeFmtSprintf("{:02X}", n), Bof_SafeFmtSprintf("{:02X}", 'x'));
}

TEST(FormatterTest, FormatUnsignedChar) {
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{}", static_cast<unsigned char>(42)));
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{}", static_cast<uint8_t>(42)));
}

TEST(FormatterTest, FormatWChar) {
  EXPECT_EQ(L"a", Bof_SafeFmtSprintf(L"{0}", L'a'));
  // This shouldn't compile:
  //Bof_SafeFmtSprintf("{}", L'a');
}

TEST(FormatterTest, FormatCString) {
  check_unknown_types("test", "sp", "string");
  EXPECT_EQ("test", Bof_SafeFmtSprintf("{0}", "test"));
  EXPECT_EQ("test", Bof_SafeFmtSprintf("{0:s}", "test"));
  char nonconst[] = "nonconst";
  EXPECT_EQ("nonconst", Bof_SafeFmtSprintf("{0}", nonconst));
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0}", static_cast<const char*>(FMT_NULL)),
    fmt::format_error, "string pointer is null");
}

TEST(FormatterTest, FormatSCharString) {
  signed char str[] = "test";
  EXPECT_EQ("test", Bof_SafeFmtSprintf("{0:s}", str));
  const signed char *const_str = str;
  EXPECT_EQ("test", Bof_SafeFmtSprintf("{0:s}", const_str));
}

TEST(FormatterTest, FormatUCharString) {
  unsigned char str[] = "test";
  EXPECT_EQ("test", Bof_SafeFmtSprintf("{0:s}", str));
  const unsigned char *const_str = str;
  EXPECT_EQ("test", Bof_SafeFmtSprintf("{0:s}", const_str));
  unsigned char *ptr = str;
  EXPECT_EQ("test", Bof_SafeFmtSprintf("{0:s}", ptr));
}

TEST(FormatterTest, FormatPointer) {
  check_unknown_types(reinterpret_cast<void*>(0x1234), "p", "pointer");
  EXPECT_EQ("0x0", Bof_SafeFmtSprintf("{0}", static_cast<void*>(FMT_NULL)));
  EXPECT_EQ("0x1234", Bof_SafeFmtSprintf("{0}", reinterpret_cast<void*>(0x1234)));
  EXPECT_EQ("0x1234", Bof_SafeFmtSprintf("{0:p}", reinterpret_cast<void*>(0x1234)));
  EXPECT_EQ("0x" + std::string(sizeof(void*) * CHAR_BIT / 4, 'f'),
    Bof_SafeFmtSprintf("{0}", reinterpret_cast<void*>(~uintptr_t())));
  EXPECT_EQ("0x1234", Bof_SafeFmtSprintf("{}", fmt::ptr(reinterpret_cast<int*>(0x1234))));
#if FMT_USE_NULLPTR
  EXPECT_EQ("0x0", Bof_SafeFmtSprintf("{}", FMT_NULL));
#endif
}

TEST(FormatterTest, FormatString) {
  EXPECT_EQ("test", Bof_SafeFmtSprintf("{0}", std::string("test")));
}

TEST(FormatterTest, FormatStringView) {
  EXPECT_EQ("test", Bof_SafeFmtSprintf("{}", fmt::string_view("test")));
  EXPECT_EQ("", Bof_SafeFmtSprintf("{}", fmt::string_view()));
}

#ifdef FMT_USE_STD_STRING_VIEW
TEST(FormatterTest, FormatStdStringView) {
  EXPECT_EQ("test", Bof_SafeFmtSprintf("{0}", std::string_view("test")));
}
#endif

FMT_BEGIN_NAMESPACE
template <>
struct formatter<Date> {
  template <typename ParseContext>
  FMT_CONSTEXPR auto parse(ParseContext &ctx) -> decltype(ctx.begin()) {
    auto it = ctx.begin();
    if (*it == 'd')
      ++it;
    return it;
  }

  auto format(const Date &d, format_context &ctx) -> decltype(ctx.out()) {
    format_to(ctx.out(), "{}-{}-{}", d.year(), d.month(), d.day());
    return ctx.out();
  }
};
FMT_END_NAMESPACE

TEST(FormatterTest, FormatCustom) {
  Date date(2012, 12, 9);
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{:s}", date), fmt::format_error,
    "unknown format specifier");
}

class Answer {};

FMT_BEGIN_NAMESPACE
template <>
struct formatter<Answer> : formatter<int> {
  template <typename FormatContext>
  auto format(Answer, FormatContext &ctx) -> decltype(ctx.out()) {
    return formatter<int>::format(42, ctx);
  }
};
FMT_END_NAMESPACE

TEST(FormatterTest, CustomFormat) {
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{0}", Answer()));
  EXPECT_EQ("0042", Bof_SafeFmtSprintf("{:04}", Answer()));
}

TEST(FormatterTest, CustomFormatTo) {
  char buf[10] = {};
  auto end = &*fmt::format_to(
    fmt::internal::make_checked(buf, 10), "{}", Answer());
  EXPECT_EQ(end, buf + 2);
  EXPECT_STREQ(buf, "42");
}

TEST(FormatterTest, WideFormatString) {
  EXPECT_EQ(L"42", Bof_SafeFmtSprintf(L"{}", 42));
  EXPECT_EQ(L"4.2", Bof_SafeFmtSprintf(L"{}", 4.2));
  EXPECT_EQ(L"abc", Bof_SafeFmtSprintf(L"{}", L"abc"));
  EXPECT_EQ(L"z", Bof_SafeFmtSprintf(L"{}", L'z'));
}

TEST(FormatterTest, FormatStringFromSpeedTest) {
  EXPECT_EQ("1.2340000000:0042:+3.13:str:0x3e8:X:%",
    Bof_SafeFmtSprintf("{0:0.10f}:{1:04}:{2:+g}:{3}:{4}:{5}:%",
      1.234, 42, 3.13, "str", reinterpret_cast<void*>(1000), 'X'));
}

TEST(FormatterTest, FormatExamples) {
  std::string message = Bof_SafeFmtSprintf("The answer is {}", 42);
  EXPECT_EQ("The answer is 42", message);

  EXPECT_EQ("42", Bof_SafeFmtSprintf("{}", 42));
  EXPECT_EQ("42", Bof_SafeFmtSprintf(std::string("{}"), 42));

  fmt::memory_buffer out;
  format_to(out, "The answer is {}.", 42);
  EXPECT_EQ("The answer is 42.", to_string(out));
  /*
  const char *filename = "nonexistent";
  FILE *ftest = safe_fopen(filename, "r");
  if (ftest) fclose(ftest);
  int error_code = errno;
  EXPECT_TRUE(ftest == FMT_NULL);
  EXPECT_SYSTEM_ERROR({
    FILE *f = safe_fopen(filename, "r");
  if (!f)
    throw fmt::system_error(errno, "Cannot open file '{}'", filename);
  fclose(f);
  }, error_code, "Cannot open file 'nonexistent'");
  */
}

TEST(FormatterTest, Examples) {
  EXPECT_EQ("First, thou shalt count to three",
    Bof_SafeFmtSprintf("First, thou shalt count to {0}", "three"));
  EXPECT_EQ("Bring me a shrubbery",
    Bof_SafeFmtSprintf("Bring me a {}", "shrubbery"));
  EXPECT_EQ("From 1 to 3", Bof_SafeFmtSprintf("From {} to {}", 1, 3));

  char buffer[BUFFER_SIZE];
  safe_sprintf(buffer, "%03.2f", -1.2);
  EXPECT_EQ(buffer, Bof_SafeFmtSprintf("{:03.2f}", -1.2));

  EXPECT_EQ("a, b, c", Bof_SafeFmtSprintf("{0}, {1}, {2}", 'a', 'b', 'c'));
  EXPECT_EQ("a, b, c", Bof_SafeFmtSprintf("{}, {}, {}", 'a', 'b', 'c'));
  EXPECT_EQ("c, b, a", Bof_SafeFmtSprintf("{2}, {1}, {0}", 'a', 'b', 'c'));
  EXPECT_EQ("abracadabra", Bof_SafeFmtSprintf("{0}{1}{0}", "abra", "cad"));

  EXPECT_EQ("left aligned                  ",
    Bof_SafeFmtSprintf("{:<30}", "left aligned"));
  EXPECT_EQ("                 right aligned",
    Bof_SafeFmtSprintf("{:>30}", "right aligned"));
  EXPECT_EQ("           centered           ",
    Bof_SafeFmtSprintf("{:^30}", "centered"));
  EXPECT_EQ("***********centered***********",
    Bof_SafeFmtSprintf("{:*^30}", "centered"));

  EXPECT_EQ("+3.140000; -3.140000",
    Bof_SafeFmtSprintf("{:+f}; {:+f}", 3.14, -3.14));
  EXPECT_EQ(" 3.140000; -3.140000",
    Bof_SafeFmtSprintf("{: f}; {: f}", 3.14, -3.14));
  EXPECT_EQ("3.140000; -3.140000",
    Bof_SafeFmtSprintf("{:-f}; {:-f}", 3.14, -3.14));

  EXPECT_EQ("int: 42;  hex: 2a;  oct: 52",
    Bof_SafeFmtSprintf("int: {0:d};  hex: {0:x};  oct: {0:o}", 42));
  EXPECT_EQ("int: 42;  hex: 0x2a;  oct: 052",
    Bof_SafeFmtSprintf("int: {0:d};  hex: {0:#x};  oct: {0:#o}", 42));

  EXPECT_EQ("The answer is 42", Bof_SafeFmtSprintf("The answer is {}", 42));
  EXPECT_THROW_MSG(
    Bof_SafeFmtSprintf("The answer is {:d}", "forty-two"), fmt::format_error,
    "invalid type specifier");

  EXPECT_EQ(L"Cyrillic letter \x42e",
    Bof_SafeFmtSprintf(L"Cyrillic letter {}", L'\x42e'));

//  EXPECT_WRITE(stdout, fmt::print("{}", std::numeric_limits<double>::infinity()), "inf");
}

TEST(FormatIntTest, Data) {
  fmt::format_int format_int(42);
  EXPECT_EQ("42", std::string(format_int.data(), format_int.size()));
}

TEST(FormatIntTest, FormatInt) {
  EXPECT_EQ("42", fmt::format_int(42).str());
  EXPECT_EQ(2u, fmt::format_int(42).size());
  EXPECT_EQ("-42", fmt::format_int(-42).str());
  EXPECT_EQ(3u, fmt::format_int(-42).size());
  EXPECT_EQ("42", fmt::format_int(42ul).str());
  EXPECT_EQ("-42", fmt::format_int(-42l).str());
  EXPECT_EQ("42", fmt::format_int(42ull).str());
  EXPECT_EQ("-42", fmt::format_int(-42ll).str());
  std::ostringstream os;
  os << std::numeric_limits<int64_t>::max();
  EXPECT_EQ(os.str(),
    fmt::format_int(std::numeric_limits<int64_t>::max()).str());
}

template <typename T>
std::string format_decimal(T value) {
  char buffer[10];
  char *ptr = buffer;
  fmt::format_decimal(ptr, value);
  return std::string(buffer, ptr);
}

TEST(FormatIntTest, FormatDec) {
  EXPECT_EQ("-42", format_decimal(static_cast<signed char>(-42)));
  EXPECT_EQ("-42", format_decimal(static_cast<short>(-42)));
  std::ostringstream os;
  os << std::numeric_limits<unsigned short>::max();
  EXPECT_EQ(os.str(),
    format_decimal(std::numeric_limits<unsigned short>::max()));
  EXPECT_EQ("1", format_decimal(1));
  EXPECT_EQ("-1", format_decimal(-1));
  EXPECT_EQ("42", format_decimal(42));
  EXPECT_EQ("-42", format_decimal(-42));
  EXPECT_EQ("42", format_decimal(42l));
  EXPECT_EQ("42", format_decimal(42ul));
  EXPECT_EQ("42", format_decimal(42ll));
  EXPECT_EQ("42", format_decimal(42ull));
}

TEST(FormatTest, Print) {
#if FMT_USE_FILE_DESCRIPTORS
  EXPECT_WRITE(stdout, fmt::print("Don't {}!", "panic"), "Don't panic!");
  EXPECT_WRITE(stderr,
    fmt::print(stderr, "Don't {}!", "panic"), "Don't panic!");
#endif
}

TEST(FormatTest, Variadic) {
  EXPECT_EQ("abc1", Bof_SafeFmtSprintf("{}c{}", "ab", 1));
  EXPECT_EQ(L"abc1", Bof_SafeFmtSprintf(L"{}c{}", L"ab", 1));
}

TEST(FormatTest, Dynamic) {
  typedef fmt::format_context ctx;
  std::vector<fmt::basic_format_arg<ctx>> args;
  args.emplace_back(fmt::internal::make_arg<ctx>(42));
  args.emplace_back(fmt::internal::make_arg<ctx>("abc1"));
  args.emplace_back(fmt::internal::make_arg<ctx>(1.2f));

  std::string result = fmt::vformat("{} and {} and {}",
    fmt::basic_format_args<ctx>(
      args.data(),
      static_cast<unsigned>(args.size())));

  EXPECT_EQ("42 and abc1 and 1.2", result);
}

TEST(FormatTest, JoinArg) {
  using fmt::join;
  int v1[3] = { 1, 2, 3 };
  std::vector<float> v2;
  v2.push_back(1.2f);
  v2.push_back(3.4f);
  void *v3[2] = { &v1[0], &v1[1] };

  EXPECT_EQ("(1, 2, 3)", Bof_SafeFmtSprintf("({})", join(v1, v1 + 3, ", ")));
  EXPECT_EQ("(1)", Bof_SafeFmtSprintf("({})", join(v1, v1 + 1, ", ")));
  EXPECT_EQ("()", Bof_SafeFmtSprintf("({})", join(v1, v1, ", ")));
  EXPECT_EQ("(001, 002, 003)", Bof_SafeFmtSprintf("({:03})", join(v1, v1 + 3, ", ")));
  EXPECT_EQ("(+01.20, +03.40)",
    Bof_SafeFmtSprintf("({:+06.2f})", join(v2.begin(), v2.end(), ", ")));

  EXPECT_EQ(L"(1, 2, 3)", Bof_SafeFmtSprintf(L"({})", join(v1, v1 + 3, L", ")));
  EXPECT_EQ("1, 2, 3", Bof_SafeFmtSprintf("{0:{1}}", join(v1, v1 + 3, ", "), 1));

  EXPECT_EQ(Bof_SafeFmtSprintf("{}, {}", v3[0], v3[1]),
    Bof_SafeFmtSprintf("{}", join(v3, v3 + 2, ", ")));

#if FMT_USE_TRAILING_RETURN && (!FMT_GCC_VERSION || FMT_GCC_VERSION >= 405)
  EXPECT_EQ("(1, 2, 3)", Bof_SafeFmtSprintf("({})", join(v1, ", ")));
  EXPECT_EQ("(+01.20, +03.40)", Bof_SafeFmtSprintf("({:+06.2f})", join(v2, ", ")));
#endif
}

template <typename T>
std::string str(const T &value) {
  return Bof_SafeFmtSprintf("{}", value);
}

TEST(StrTest, Convert) {
  EXPECT_EQ("42", str(42));
  std::string s = str(Date(2012, 12, 9));
  EXPECT_EQ("2012-12-9", s);
}

std::string vformat_message(int id, const char *format, fmt::format_args args) {
  fmt::memory_buffer buffer;
  format_to(buffer, "[{}] ", id);
  vformat_to(buffer, format, args);
  return to_string(buffer);
}

template <typename... Args>
std::string format_message(int id, const char *format, const Args & ... args) {
  auto va = fmt::make_format_args(args...);
  return vformat_message(id, format, va);
}

TEST(FormatTest, FormatMessageExample) {
  EXPECT_EQ("[42] something happened",
    format_message(42, "{} happened", "something"));
}

template<typename... Args>
void print_error(const char *file, int line, const char *format,
  const Args & ... args) {
  fmt::print("{}: {}: ", file, line);
  fmt::print(format, args...);
}

TEST(FormatTest, UnpackedArgs) {
  EXPECT_EQ("0123456789abcdefg",
    Bof_SafeFmtSprintf("{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}",
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 'a', 'b', 'c', 'd', 'e',
      'f', 'g'));
}

#if FMT_USE_USER_DEFINED_LITERALS
// Passing user-defined literals directly to EXPECT_EQ causes problems
// with macro argument stringification (#) on some versions of GCC.
// Workaround: Assing the UDL result to a variable before the macro.

using namespace fmt::literals;

TEST(LiteralsTest, Format) {
  auto udl_format = "{}c{}"_format("ab", 1);
  EXPECT_EQ(Bof_SafeFmtSprintf("{}c{}", "ab", 1), udl_format);
  auto udl_format_w = L"{}c{}"_format(L"ab", 1);
  EXPECT_EQ(Bof_SafeFmtSprintf(L"{}c{}", L"ab", 1), udl_format_w);
}

TEST(LiteralsTest, NamedArg) {
  auto udl_a = Bof_SafeFmtSprintf("{first}{second}{first}{third}",
    "first"_a = "abra", "second"_a = "cad", "third"_a = 99);
  EXPECT_EQ(Bof_SafeFmtSprintf("{first}{second}{first}{third}",
    fmt::arg("first", "abra"), fmt::arg("second", "cad"),
    fmt::arg("third", 99)),
    udl_a);
  auto udl_a_w = Bof_SafeFmtSprintf(L"{first}{second}{first}{third}",
    L"first"_a = L"abra", L"second"_a = L"cad", L"third"_a = 99);
  EXPECT_EQ(Bof_SafeFmtSprintf(L"{first}{second}{first}{third}",
    fmt::arg(L"first", L"abra"), fmt::arg(L"second", L"cad"),
    fmt::arg(L"third", 99)),
    udl_a_w);
}

TEST(FormatTest, UdlTemplate) {
  EXPECT_EQ("foo", "foo"_format());
  EXPECT_EQ("        42", "{0:10}"_format(42));
  EXPECT_EQ("42", fmt::format(FMT_STRING("{}"), 42));
  EXPECT_EQ(L"42", fmt::format(FMT_STRING(L"{}"), 42));
}
#endif // FMT_USE_USER_DEFINED_LITERALS

TEST(FormatTest, Enum) {
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{}", A));
}

TEST(FormatTest, EnumFormatterUnambiguous) {
  fmt::formatter<TestEnum> f;
//  ASSERT_GE(sizeof(f), 0); // use f to avoid compiler warning
}

#if FMT_HAS_FEATURE(cxx_strong_enums)
enum TestFixedEnum : short { B };

TEST(FormatTest, FixedEnum) {
  EXPECT_EQ("0", Bof_SafeFmtSprintf("{}", B));
}
#endif


//template <typename... Args>
//void custom_format(const char *format_str, const Args & ... args) {
//  auto va = fmt::make_format_args(args...);
//  return custom_vformat(format_str, va);
//}

//TEST(FormatTest, CustomArgFormatter) {
//  custom_format("{}", 42);
//}

TEST(FormatTest, NonNullTerminatedFormatString) {
  EXPECT_EQ("42", fmt::format(fmt::string_view("{}foo", 2), 42));
}

struct variant {
  enum { INT, STRING } type;
  explicit variant(int) : type(INT) {}
  explicit variant(const char *) : type(STRING) {}
};

FMT_BEGIN_NAMESPACE
template <>
struct formatter<variant> : dynamic_formatter<> {
  auto format(variant value, format_context& ctx) -> decltype(ctx.out()) {
    if (value.type == variant::INT)
      return dynamic_formatter<>::format(42, ctx);
    return dynamic_formatter<>::format("foo", ctx);
  }
};
FMT_END_NAMESPACE

TEST(FormatTest, DynamicFormatter) {
  auto num = variant(42);
  auto str = variant("foo");
  EXPECT_EQ("42", Bof_SafeFmtSprintf("{:d}", num));
  EXPECT_EQ("foo", Bof_SafeFmtSprintf("{:s}", str));
  EXPECT_EQ(" 42 foo ", Bof_SafeFmtSprintf("{:{}} {:{}}", num, 3, str, 4));
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{0:{}}", num),
    fmt::format_error, "cannot switch from manual to automatic argument indexing");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{:{0}}", num),
    fmt::format_error, "cannot switch from automatic to manual argument indexing");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{:=}", str),
    fmt::format_error, "format specifier requires numeric argument");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{:+}", str),
    fmt::format_error, "format specifier requires numeric argument");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{:-}", str),
    fmt::format_error, "format specifier requires numeric argument");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{: }", str),
    fmt::format_error, "format specifier requires numeric argument");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{:#}", str),
    fmt::format_error, "format specifier requires numeric argument");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{:0}", str),
    fmt::format_error, "format specifier requires numeric argument");
  EXPECT_THROW_MSG(Bof_SafeFmtSprintf("{:.2}", num),
    fmt::format_error, "precision not allowed for this argument type");
}

TEST(FormatTest, ToString) {
  EXPECT_EQ("42", fmt::to_string(42));
  EXPECT_EQ("0x1234", fmt::to_string(reinterpret_cast<void*>(0x1234)));
}

TEST(FormatTest, ToWString) {
  EXPECT_EQ(L"42", fmt::to_wstring(42));
}

TEST(FormatTest, OutputIterators) {
  std::list<char> out;
  fmt::format_to(std::back_inserter(out), "{}", 42);
  EXPECT_EQ("42", std::string(out.begin(), out.end()));
  std::stringstream s;
  fmt::format_to(std::ostream_iterator<char>(s), "{}", 42);
  EXPECT_EQ("42", s.str());
}

TEST(FormatTest, FormattedSize) {
  EXPECT_EQ(2u, fmt::formatted_size("{}", 42));
}

TEST(FormatTest, FormatToN) {
  char buffer[4];
  buffer[3] = 'x';
  auto result = fmt::format_to_n(buffer, 3, "{}", 12345);
  EXPECT_EQ(5u, result.size);
  EXPECT_EQ(buffer + 3, result.out);
  EXPECT_EQ("123x", fmt::string_view(buffer, 4));
  result = fmt::format_to_n(buffer, 3, "{:s}", "foobar");
  EXPECT_EQ(6u, result.size);
  EXPECT_EQ(buffer + 3, result.out);
  EXPECT_EQ("foox", fmt::string_view(buffer, 4));
}

TEST(FormatTest, WideFormatToN) {
  wchar_t buffer[4];
  buffer[3] = L'x';
  auto result = fmt::format_to_n(buffer, 3, L"{}", 12345);
  EXPECT_EQ(5u, result.size);
  EXPECT_EQ(buffer + 3, result.out);
  EXPECT_EQ(L"123x", fmt::wstring_view(buffer, 4));
}

#if FMT_USE_CONSTEXPR
struct test_arg_id_handler {
  enum result { NONE, EMPTY, INDEX, NAME, ERROR };
  result res = NONE;
  unsigned index = 0;
  string_view name;

  FMT_CONSTEXPR void operator()() { res = EMPTY; }

  FMT_CONSTEXPR void operator()(unsigned i) {
    res = INDEX;
    index = i;
  }

  FMT_CONSTEXPR void operator()(string_view n) {
    res = NAME;
    name = n;
  }

  FMT_CONSTEXPR void on_error(const char *) { res = ERROR; }
};

template <size_t N>
FMT_CONSTEXPR test_arg_id_handler parse_arg_id(const char(&s)[N]) {
  test_arg_id_handler h;
  fmt::internal::parse_arg_id(s, s + N, h);
  return h;
}

TEST(FormatTest, ConstexprParseArgID) {
  static_assert(parse_arg_id(":").res == test_arg_id_handler::EMPTY, "");
  static_assert(parse_arg_id("}").res == test_arg_id_handler::EMPTY, "");
  static_assert(parse_arg_id("42:").res == test_arg_id_handler::INDEX, "");
  static_assert(parse_arg_id("42:").index == 42, "");
  static_assert(parse_arg_id("foo:").res == test_arg_id_handler::NAME, "");
  static_assert(parse_arg_id("foo:").name.size() == 3, "");
  static_assert(parse_arg_id("!").res == test_arg_id_handler::ERROR, "");
}

struct test_format_specs_handler {
  enum Result { NONE, PLUS, MINUS, SPACE, HASH, ZERO, ERROR };
  Result res = NONE;

  fmt::alignment align_ = fmt::ALIGN_DEFAULT;
  char fill = 0;
  unsigned width = 0;
  fmt::internal::arg_ref<char> width_ref;
  unsigned precision = 0;
  fmt::internal::arg_ref<char> precision_ref;
  char type = 0;

  // Workaround for MSVC2017 bug that results in "expression did not evaluate
  // to a constant" with compiler-generated copy ctor.
  FMT_CONSTEXPR test_format_specs_handler() {}
  FMT_CONSTEXPR test_format_specs_handler(const test_format_specs_handler &other)
    : res(other.res), align_(other.align_), fill(other.fill),
    width(other.width), width_ref(other.width_ref),
    precision(other.precision), precision_ref(other.precision_ref),
    type(other.type) {}

  FMT_CONSTEXPR void on_align(fmt::alignment a) { align_ = a; }
  FMT_CONSTEXPR void on_fill(char f) { fill = f; }
  FMT_CONSTEXPR void on_plus() { res = PLUS; }
  FMT_CONSTEXPR void on_minus() { res = MINUS; }
  FMT_CONSTEXPR void on_space() { res = SPACE; }
  FMT_CONSTEXPR void on_hash() { res = HASH; }
  FMT_CONSTEXPR void on_zero() { res = ZERO; }

  FMT_CONSTEXPR void on_width(unsigned w) { width = w; }
  FMT_CONSTEXPR void on_dynamic_width(fmt::internal::auto_id) {}
  FMT_CONSTEXPR void on_dynamic_width(unsigned index) { width_ref = index; }
  FMT_CONSTEXPR void on_dynamic_width(string_view) {}

  FMT_CONSTEXPR void on_precision(unsigned p) { precision = p; }
  FMT_CONSTEXPR void on_dynamic_precision(fmt::internal::auto_id) {}
  FMT_CONSTEXPR void on_dynamic_precision(unsigned index) {
    precision_ref = index;
  }
  FMT_CONSTEXPR void on_dynamic_precision(string_view) {}

  FMT_CONSTEXPR void end_precision() {}
  FMT_CONSTEXPR void on_type(char t) { type = t; }
  FMT_CONSTEXPR void on_error(const char *) { res = ERROR; }
};

template <size_t N>
FMT_CONSTEXPR test_format_specs_handler parse_test_specs(const char(&s)[N]) {
  test_format_specs_handler h;
  fmt::internal::parse_format_specs(s, s + N, h);
  return h;
}

TEST(FormatTest, ConstexprParseFormatSpecs) {
  typedef test_format_specs_handler handler;
  static_assert(parse_test_specs("<").align_ == fmt::ALIGN_LEFT, "");
  static_assert(parse_test_specs("*^").fill == '*', "");
  static_assert(parse_test_specs("+").res == handler::PLUS, "");
  static_assert(parse_test_specs("-").res == handler::MINUS, "");
  static_assert(parse_test_specs(" ").res == handler::SPACE, "");
  static_assert(parse_test_specs("#").res == handler::HASH, "");
  static_assert(parse_test_specs("0").res == handler::ZERO, "");
  static_assert(parse_test_specs("42").width == 42, "");
  static_assert(parse_test_specs("{42}").width_ref.index == 42, "");
  static_assert(parse_test_specs(".42").precision == 42, "");
  static_assert(parse_test_specs(".{42}").precision_ref.index == 42, "");
  static_assert(parse_test_specs("d").type == 'd', "");
  static_assert(parse_test_specs("{<").res == handler::ERROR, "");
}

struct test_context {
  typedef char char_type;

  FMT_CONSTEXPR fmt::basic_format_arg<test_context> next_arg() {
    return fmt::internal::make_arg<test_context>(11);
  }

  template <typename Id>
  FMT_CONSTEXPR fmt::basic_format_arg<test_context> get_arg(Id) {
    return fmt::internal::make_arg<test_context>(22);
  }

  template <typename Id>
  FMT_CONSTEXPR void check_arg_id(Id) {}

  FMT_CONSTEXPR unsigned next_arg_id() { return 33; }

  void on_error(const char *) {}

  FMT_CONSTEXPR test_context &parse_context() { return *this; }
  FMT_CONSTEXPR test_context error_handler() { return *this; }
};

template <size_t N>
FMT_CONSTEXPR fmt::format_specs parse_specs(const char(&s)[N]) {
  fmt::format_specs specs;
  test_context ctx{};
  fmt::internal::specs_handler<test_context> h(specs, ctx);
  parse_format_specs(s, s + N, h);
  return specs;
}

TEST(FormatTest, ConstexprSpecsHandler) {
  static_assert(parse_specs("<").align() == fmt::ALIGN_LEFT, "");
  static_assert(parse_specs("*^").fill() == '*', "");
  static_assert(parse_specs("+").has(fmt::PLUS_FLAG), "");
  static_assert(parse_specs("-").has(fmt::MINUS_FLAG), "");
  static_assert(parse_specs(" ").has(fmt::SIGN_FLAG), "");
  static_assert(parse_specs("#").has(fmt::HASH_FLAG), "");
  static_assert(parse_specs("0").align() == fmt::ALIGN_NUMERIC, "");
  static_assert(parse_specs("42").width() == 42, "");
  static_assert(parse_specs("{}").width() == 11, "");
  static_assert(parse_specs("{0}").width() == 22, "");
  static_assert(parse_specs(".42").precision == 42, "");
  static_assert(parse_specs(".{}").precision == 11, "");
  static_assert(parse_specs(".{0}").precision == 22, "");
  static_assert(parse_specs("d").type == 'd', "");
}

template <size_t N>
FMT_CONSTEXPR fmt::internal::dynamic_format_specs<char>
parse_dynamic_specs(const char(&s)[N]) {
  fmt::internal::dynamic_format_specs<char> specs;
  test_context ctx{};
  fmt::internal::dynamic_specs_handler<test_context> h(specs, ctx);
  parse_format_specs(s, s + N, h);
  return specs;
}

TEST(FormatTest, ConstexprDynamicSpecsHandler) {
  static_assert(parse_dynamic_specs("<").align() == fmt::ALIGN_LEFT, "");
  static_assert(parse_dynamic_specs("*^").fill() == '*', "");
  static_assert(parse_dynamic_specs("+").has(fmt::PLUS_FLAG), "");
  static_assert(parse_dynamic_specs("-").has(fmt::MINUS_FLAG), "");
  static_assert(parse_dynamic_specs(" ").has(fmt::SIGN_FLAG), "");
  static_assert(parse_dynamic_specs("#").has(fmt::HASH_FLAG), "");
  static_assert(parse_dynamic_specs("0").align() == fmt::ALIGN_NUMERIC, "");
  static_assert(parse_dynamic_specs("42").width() == 42, "");
  static_assert(parse_dynamic_specs("{}").width_ref.index == 33, "");
  static_assert(parse_dynamic_specs("{42}").width_ref.index == 42, "");
  static_assert(parse_dynamic_specs(".42").precision == 42, "");
  static_assert(parse_dynamic_specs(".{}").precision_ref.index == 33, "");
  static_assert(parse_dynamic_specs(".{42}").precision_ref.index == 42, "");
  static_assert(parse_dynamic_specs("d").type == 'd', "");
}

template <size_t N>
FMT_CONSTEXPR test_format_specs_handler check_specs(const char(&s)[N]) {
  fmt::internal::specs_checker<test_format_specs_handler>
    checker(test_format_specs_handler(), fmt::internal::double_type);
  parse_format_specs(s, s + N, checker);
  return checker;
}

TEST(FormatTest, ConstexprSpecsChecker) {
  typedef test_format_specs_handler handler;
  static_assert(check_specs("<").align_ == fmt::ALIGN_LEFT, "");
  static_assert(check_specs("*^").fill == '*', "");
  static_assert(check_specs("+").res == handler::PLUS, "");
  static_assert(check_specs("-").res == handler::MINUS, "");
  static_assert(check_specs(" ").res == handler::SPACE, "");
  static_assert(check_specs("#").res == handler::HASH, "");
  static_assert(check_specs("0").res == handler::ZERO, "");
  static_assert(check_specs("42").width == 42, "");
  static_assert(check_specs("{42}").width_ref.index == 42, "");
  static_assert(check_specs(".42").precision == 42, "");
  static_assert(check_specs(".{42}").precision_ref.index == 42, "");
  static_assert(check_specs("d").type == 'd', "");
  static_assert(check_specs("{<").res == handler::ERROR, "");
}

struct test_format_string_handler {
  FMT_CONSTEXPR void on_text(const char *, const char *) {}

  FMT_CONSTEXPR void on_arg_id() {}

  template <typename T>
  FMT_CONSTEXPR void on_arg_id(T) {}

  FMT_CONSTEXPR void on_replacement_field(const char *) {}

  FMT_CONSTEXPR const char *on_format_specs(const char *begin, const char*) {
    return begin;
  }

  FMT_CONSTEXPR void on_error(const char *) { error = true; }

  bool error = false;
};

template <size_t N>
FMT_CONSTEXPR bool parse_string(const char(&s)[N]) {
  test_format_string_handler h;
  fmt::internal::parse_format_string<true>(fmt::string_view(s, N - 1), h);
  return !h.error;
}

TEST(FormatTest, ConstexprParseFormatString) {
  static_assert(parse_string("foo"), "");
  static_assert(!parse_string("}"), "");
  static_assert(parse_string("{}"), "");
  static_assert(parse_string("{42}"), "");
  static_assert(parse_string("{foo}"), "");
  static_assert(parse_string("{:}"), "");
}

struct test_error_handler {
  const char *&error;

  FMT_CONSTEXPR test_error_handler(const char *&err) : error(err) {}

  FMT_CONSTEXPR test_error_handler(const test_error_handler &other)
    : error(other.error) {}

  FMT_CONSTEXPR void on_error(const char *message) {
    if (!error)
      error = message;
  }
};

FMT_CONSTEXPR size_t len(const char *s) {
  size_t len = 0;
  while (*s++)
    ++len;
  return len;
}

FMT_CONSTEXPR bool equal(const char *s1, const char *s2) {
  if (!s1 || !s2)
    return s1 == s2;
  while (*s1 && *s1 == *s2) {
    ++s1;
    ++s2;
  }
  return *s1 == *s2;
}

template <typename... Args>
FMT_CONSTEXPR bool test_error(const char *fmt, const char *expected_error) {
  const char *actual_error = FMT_NULL;
  fmt::internal::do_check_format_string<char, test_error_handler, Args...>(
    string_view(fmt, len(fmt)), test_error_handler(actual_error));
  return equal(actual_error, expected_error);
}

#define EXPECT_ERROR_NOARGS(fmt, error) \
  static_assert(test_error(fmt, error), "")
#define EXPECT_ERROR(fmt, error, ...) \
  static_assert(test_error<__VA_ARGS__>(fmt, error), "")

TEST(FormatTest, FormatStringErrors) {
  EXPECT_ERROR_NOARGS("foo", FMT_NULL);
  EXPECT_ERROR_NOARGS("}", "unmatched '}' in format string");
  EXPECT_ERROR("{0:s", "unknown format specifier", Date);
#ifndef _MSC_VER
  // This causes an internal compiler error in MSVC2017.
  EXPECT_ERROR("{0:=5", "unknown format specifier", int);
  EXPECT_ERROR("{:{<}", "invalid fill character '{'", int);
  EXPECT_ERROR("{:10000000000}", "number is too big", int);
  EXPECT_ERROR("{:.10000000000}", "number is too big", int);
  EXPECT_ERROR_NOARGS("{:x}", "argument index out of range");
  EXPECT_ERROR("{:=}", "format specifier requires numeric argument",
    const char *);
  EXPECT_ERROR("{:+}", "format specifier requires numeric argument",
    const char *);
  EXPECT_ERROR("{:-}", "format specifier requires numeric argument",
    const char *);
  EXPECT_ERROR("{:#}", "format specifier requires numeric argument",
    const char *);
  EXPECT_ERROR("{: }", "format specifier requires numeric argument",
    const char *);
  EXPECT_ERROR("{:0}", "format specifier requires numeric argument",
    const char *);
  EXPECT_ERROR("{:+}", "format specifier requires signed argument", unsigned);
  EXPECT_ERROR("{:-}", "format specifier requires signed argument", unsigned);
  EXPECT_ERROR("{: }", "format specifier requires signed argument", unsigned);
  EXPECT_ERROR("{:.2}", "precision not allowed for this argument type", int);
  EXPECT_ERROR("{:s}", "invalid type specifier", int);
  EXPECT_ERROR("{:s}", "invalid type specifier", bool);
  EXPECT_ERROR("{:s}", "invalid type specifier", char);
  EXPECT_ERROR("{:+}", "invalid format specifier for char", char);
  EXPECT_ERROR("{:s}", "invalid type specifier", double);
  EXPECT_ERROR("{:d}", "invalid type specifier", const char *);
  EXPECT_ERROR("{:d}", "invalid type specifier", std::string);
  EXPECT_ERROR("{:s}", "invalid type specifier", void *);
#endif
  EXPECT_ERROR("{foo", "missing '}' in format string", int);
  EXPECT_ERROR_NOARGS("{10000000000}", "number is too big");
  EXPECT_ERROR_NOARGS("{0x}", "invalid format string");
  EXPECT_ERROR_NOARGS("{-}", "invalid format string");
  EXPECT_ERROR("{:{0x}}", "invalid format string", int);
  EXPECT_ERROR("{:{-}}", "invalid format string", int);
  EXPECT_ERROR("{:.{0x}}", "invalid format string", int);
  EXPECT_ERROR("{:.{-}}", "invalid format string", int);
  EXPECT_ERROR("{:.x}", "missing precision specifier", int);
  EXPECT_ERROR_NOARGS("{}", "argument index out of range");
  EXPECT_ERROR("{1}", "argument index out of range", int);
  EXPECT_ERROR("{1}{}",
    "cannot switch from manual to automatic argument indexing",
    int, int);
  EXPECT_ERROR("{}{1}",
    "cannot switch from automatic to manual argument indexing",
    int, int);
}

TEST(FormatTest, VFormatTo) {
  typedef fmt::format_context context;
  fmt::basic_format_arg<context> arg = fmt::internal::make_arg<context>(42);
  fmt::basic_format_args<context> args(&arg, 1);
  std::string s;
  fmt::vformat_to(std::back_inserter(s), "{}", args);
  EXPECT_EQ("42", s);
  s.clear();
  fmt::vformat_to(std::back_inserter(s), FMT_STRING("{}"), args);
  EXPECT_EQ("42", s);

  typedef fmt::wformat_context wcontext;
  fmt::basic_format_arg<wcontext> warg = fmt::internal::make_arg<wcontext>(42);
  fmt::basic_format_args<wcontext> wargs(&warg, 1);
  std::wstring w;
  fmt::vformat_to(std::back_inserter(w), L"{}", wargs);
  EXPECT_EQ(L"42", w);
  w.clear();
  fmt::vformat_to(std::back_inserter(w), FMT_STRING(L"{}"), wargs);
  EXPECT_EQ(L"42", w);
}

#endif  // FMT_USE_CONSTEXPR

TEST(FormatTest, ConstructU8StringViewFromCString) {
  fmt::u8string_view s("ab");
  EXPECT_EQ(s.size(), 2u);
  const fmt::char8_t *data = s.data();
  EXPECT_EQ(data[0], 'a');
  EXPECT_EQ(data[1], 'b');
}

TEST(FormatTest, ConstructU8StringViewFromDataAndSize) {
  fmt::u8string_view s("foobar", 3);
  EXPECT_EQ(s.size(), 3u);
  const fmt::char8_t *data = s.data();
  EXPECT_EQ(data[0], 'f');
  EXPECT_EQ(data[1], 'o');
  EXPECT_EQ(data[2], 'o');
}

#if FMT_USE_USER_DEFINED_LITERALS
TEST(FormatTest, U8StringViewLiteral) {
  using namespace fmt::literals;
  fmt::u8string_view s = "ab"_u;
  EXPECT_EQ(s.size(), 2u);
  const fmt::char8_t *data = s.data();
  EXPECT_EQ(data[0], 'a');
  EXPECT_EQ(data[1], 'b');
//  EXPECT_EQ(Bof_SafeFmtSprintf("{:*^5}"_u, "🤡"_u), "**🤡**"_u);
}
#endif

TEST(FormatTest, FormatU8String) {
  EXPECT_EQ(fmt::format(fmt::u8string_view("{}"), 42), fmt::u8string_view("42"));
}
#pragma warning( pop ) 