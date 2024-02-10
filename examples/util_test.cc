/*
 * ngtcp2
 *
 * Copyright (c) 2018 ngtcp2 contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "util_test.h"

#include <limits>
#include <array>

#include "util.h"

using namespace std::literals;

namespace ngtcp2 {

static const MunitTest tests[] = {
    munit_void_test(test_util_format_durationf),
    munit_void_test(test_util_format_uint),
    munit_void_test(test_util_format_uint_iec),
    munit_void_test(test_util_format_duration),
    munit_void_test(test_util_parse_uint),
    munit_void_test(test_util_parse_uint_iec),
    munit_void_test(test_util_parse_duration),
    munit_void_test(test_util_normalize_path),
    munit_void_test(test_util_hexdump),
    munit_test_end(),
};

const MunitSuite util_suite = {
    "/util", tests, NULL, 1, MUNIT_SUITE_OPTION_NONE,
};

namespace util {
std::optional<std::string> read_pem(const std::string_view &filename,
                                    const std::string_view &name,
                                    const std::string_view &type) {
  return {};
}
} // namespace util

namespace util {
int write_pem(const std::string_view &filename, const std::string_view &name,
              const std::string_view &type, const uint8_t *data,
              size_t datalen) {
  return -1;
}
} // namespace util

void test_util_format_durationf() {
  assert_stdstring_equal("0ns", util::format_durationf(0));
  assert_stdstring_equal("999ns", util::format_durationf(999));
  assert_stdstring_equal("1.00us", util::format_durationf(1000));
  assert_stdstring_equal("1.00us", util::format_durationf(1004));
  assert_stdstring_equal("1.00us", util::format_durationf(1005));
  assert_stdstring_equal("1.02us", util::format_durationf(1015));
  assert_stdstring_equal("2.00us", util::format_durationf(1999));
  assert_stdstring_equal("1.00ms", util::format_durationf(999999));
  assert_stdstring_equal("3.50ms", util::format_durationf(3500111));
  assert_stdstring_equal("9999.99s", util::format_durationf(9999990000000llu));
}

void test_util_format_uint() {
  assert_stdstring_equal("0", util::format_uint(0));
  assert_stdstring_equal("18446744073709551615",
                         util::format_uint(18446744073709551615ull));
}

void test_util_format_uint_iec() {
  assert_stdstring_equal("0", util::format_uint_iec(0));
  assert_stdstring_equal("1023", util::format_uint_iec((1 << 10) - 1));
  assert_stdstring_equal("1K", util::format_uint_iec(1 << 10));
  assert_stdstring_equal("1M", util::format_uint_iec(1 << 20));
  assert_stdstring_equal("1G", util::format_uint_iec(1 << 30));
  assert_stdstring_equal(
      "18446744073709551615",
      util::format_uint_iec(std::numeric_limits<uint64_t>::max()));
  assert_stdstring_equal("1025K", util::format_uint_iec((1 << 20) + (1 << 10)));
}

void test_util_format_duration() {
  assert_stdstring_equal("0ns", util::format_duration(0));
  assert_stdstring_equal("999ns", util::format_duration(999));
  assert_stdstring_equal("1us", util::format_duration(1000));
  assert_stdstring_equal("1ms", util::format_duration(1000000));
  assert_stdstring_equal("1s", util::format_duration(1000000000));
  assert_stdstring_equal("1m", util::format_duration(60000000000ull));
  assert_stdstring_equal("1h", util::format_duration(3600000000000ull));
  assert_stdstring_equal(
      "18446744073709551615ns",
      util::format_duration(std::numeric_limits<uint64_t>::max()));
  assert_stdstring_equal("61s", util::format_duration(61000000000ull));
}

void test_util_parse_uint() {
  {
    auto res = util::parse_uint("0");
    assert_true(res.has_value());
    assert_uint64(0, ==, *res);
  }
  {
    auto res = util::parse_uint("1");
    assert_true(res.has_value());
    assert_uint64(1, ==, *res);
  }
  {
    auto res = util::parse_uint("18446744073709551615");
    assert_true(res.has_value());
    assert_uint64(18446744073709551615ull, ==, *res);
  }
  {
    auto res = util::parse_uint("18446744073709551616");
    assert_false(res.has_value());
  }
  {
    auto res = util::parse_uint("a");
    assert_false(res.has_value());
  }
  {
    auto res = util::parse_uint("1a");
    assert_false(res.has_value());
  }
}

void test_util_parse_uint_iec() {
  {
    auto res = util::parse_uint_iec("0");
    assert_true(res.has_value());
    assert_uint64(0, ==, *res);
  }
  {
    auto res = util::parse_uint_iec("1023");
    assert_true(res.has_value());
    assert_uint64(1023, ==, *res);
  }
  {
    auto res = util::parse_uint_iec("1K");
    assert_true(res.has_value());
    assert_uint64(1 << 10, ==, *res);
  }
  {
    auto res = util::parse_uint_iec("1M");
    assert_true(res.has_value());
    assert_uint64(1 << 20, ==, *res);
  }
  {
    auto res = util::parse_uint_iec("1G");
    assert_true(res.has_value());
    assert_uint64(1 << 30, ==, *res);
  }
  {
    auto res = util::parse_uint_iec("11G");
    assert_true(res.has_value());
    assert_uint64((1ull << 30) * 11, ==, *res);
  }
  {
    auto res = util::parse_uint_iec("18446744073709551616");
    assert_false(res.has_value());
  }
  {
    auto res = util::parse_uint_iec("1x");
    assert_false(res.has_value());
  }
  {
    auto res = util::parse_uint_iec("1Gx");
    assert_false(res.has_value());
  }
}

void test_util_parse_duration() {
  {
    auto res = util::parse_duration("0");
    assert_true(res.has_value());
    assert_uint64(0, ==, *res);
  }
  {
    auto res = util::parse_duration("1");
    assert_true(res.has_value());
    assert_uint64(NGTCP2_SECONDS, ==, *res);
  }
  {
    auto res = util::parse_duration("0ns");
    assert_true(res.has_value());
    assert_uint64(0, ==, *res);
  }
  {
    auto res = util::parse_duration("1ns");
    assert_true(res.has_value());
    assert_uint64(1, ==, *res);
  }
  {
    auto res = util::parse_duration("1us");
    assert_true(res.has_value());
    assert_uint64(NGTCP2_MICROSECONDS, ==, *res);
  }
  {
    auto res = util::parse_duration("1ms");
    assert_true(res.has_value());
    assert_uint64(NGTCP2_MILLISECONDS, ==, *res);
  }
  {
    auto res = util::parse_duration("1s");
    assert_true(res.has_value());
    assert_uint64(NGTCP2_SECONDS, ==, *res);
  }
  {
    auto res = util::parse_duration("1m");
    assert_true(res.has_value());
    assert_uint64(60 * NGTCP2_SECONDS, ==, *res);
  }
  {
    auto res = util::parse_duration("1h");
    assert_true(res.has_value());
    assert_uint64(3600 * NGTCP2_SECONDS, ==, *res);
  }
  {
    auto res = util::parse_duration("2h");
    assert_true(res.has_value());
    assert_uint64(2 * 3600 * NGTCP2_SECONDS, ==, *res);
  }
  {
    auto res = util::parse_duration("18446744073709551616");
    assert_false(res.has_value());
  }
  {
    auto res = util::parse_duration("1x");
    assert_false(res.has_value());
  }
  {
    auto res = util::parse_duration("1mx");
    assert_false(res.has_value());
  }
  {
    auto res = util::parse_duration("1mxy");
    assert_false(res.has_value());
  }
}

void test_util_normalize_path() {
  assert_stdstring_equal("/", util::normalize_path("/"));
  assert_stdstring_equal("/", util::normalize_path("//"));
  assert_stdstring_equal("/foo", util::normalize_path("/foo"));
  assert_stdstring_equal("/foo/bar/", util::normalize_path("/foo/bar/"));
  assert_stdstring_equal("/foo/bar/", util::normalize_path("/foo/abc/../bar/"));
  assert_stdstring_equal("/foo/bar/",
                         util::normalize_path("/../foo/abc/../bar/"));
  assert_stdstring_equal("/foo/bar/",
                         util::normalize_path("/./foo/././abc///.././bar/./"));
  assert_stdstring_equal("/foo/", util::normalize_path("/foo/."));
  assert_stdstring_equal("/foo/bar", util::normalize_path("/foo/./bar"));
  assert_stdstring_equal("/bar", util::normalize_path("/foo/./../bar"));
  assert_stdstring_equal("/bar", util::normalize_path("/../../bar"));
}

void test_util_hexdump() {
  char buf[4096];

  struct hexdump_testdata {
    std::string_view data;
    std::string_view dump;
  };

  auto tests = std::to_array<hexdump_testdata>({
      {
          // empty data
          "",
          "",
      },
      {
          // 1 byte
          "0",
          "00000000  30                                                |0|\n"
          "00000001\n",
      },
      {
          // 8 bytes
          "01234567",
          "00000000  30 31 32 33 34 35 36 37                           "
          "|01234567|\n"
          "00000008\n",
      },
      {
          // 9 bytes
          "012345678",
          "00000000  30 31 32 33 34 35 36 37  38                       "
          "|012345678|\n"
          "00000009\n",
      },
      {
          // 15 bytes
          "0123456789abcde",
          "00000000  30 31 32 33 34 35 36 37  38 39 61 62 63 64 65     "
          "|0123456789abcde|\n"
          "0000000f\n",
      },
      {
          // 16 bytes,
          "0123456789abcdef",
          "00000000  30 31 32 33 34 35 36 37  38 39 61 62 63 64 65 66  "
          "|0123456789abcdef|\n"
          "00000010\n",
      },
      {
          // 17 bytes
          "0123456789abcdefg",
          "00000000  30 31 32 33 34 35 36 37  38 39 61 62 63 64 65 66  "
          "|0123456789abcdef|\n"
          "00000010  67                                                |g|\n"
          "00000011\n",
      },
      {
          // non printables
          "\0\a\b\t\n\v\f\r\x7f"sv,
          "00000000  00 07 08 09 0a 0b 0c 0d  7f                       "
          "|.........|\n"
          "00000009\n",
      },
      {
          // multiple lines
          "alpha bravo charlie delta echo foxtrot golf",
          "00000000  61 6c 70 68 61 20 62 72  61 76 6f 20 63 68 61 72  |alpha "
          "bravo char|\n"
          "00000010  6c 69 65 20 64 65 6c 74  61 20 65 63 68 6f 20 66  |lie "
          "delta echo f|\n"
          "00000020  6f 78 74 72 6f 74 20 67  6f 6c 66                 |oxtrot "
          "golf|\n"
          "0000002b\n",
      },
      {
          // repeated lines
          "00000000000000010000000000000001000000000000000100000000000000020000"
          "0000000000020000000000000003",
          "00000000  30 30 30 30 30 30 30 30  30 30 30 30 30 30 30 31  "
          "|0000000000000001|\n"
          "*\n"
          "00000030  30 30 30 30 30 30 30 30  30 30 30 30 30 30 30 32  "
          "|0000000000000002|\n"
          "*\n"
          "00000050  30 30 30 30 30 30 30 30  30 30 30 30 30 30 30 33  "
          "|0000000000000003|\n"
          "00000060\n",
      },
      {
          // ends with the repeated line
          "00000000000000010000000000000001000000000000000100000000000000020000"
          "000000000002",
          "00000000  30 30 30 30 30 30 30 30  30 30 30 30 30 30 30 31  "
          "|0000000000000001|\n"
          "*\n"
          "00000030  30 30 30 30 30 30 30 30  30 30 30 30 30 30 30 32  "
          "|0000000000000002|\n"
          "*\n"
          "00000050\n",
      },
  });

  for (auto &t : tests) {
    auto f = tmpfile();
    auto rv = util::hexdump(f, t.data.data(), t.data.size());

    assert_int(0, ==, rv);

    fseek(f, 0, SEEK_SET);
    auto nread = fread(buf, 1, sizeof(buf), f);
    buf[nread] = '\0';

    assert_string_equal(t.dump.data(), buf);

    fclose(f);
  }
}

} // namespace ngtcp2
