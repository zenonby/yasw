#include <string>
#include <algorithm>
#include <cstdio>
#include <codecvt>
#include "SqliteDb.h"

#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(testSuiteSqliteDbBindings)

namespace {

	struct SqliteDbFixture
	{
		SqliteDbFixture()
		{
			m_tempFileName = std::tmpnam(nullptr);

			// string -> wstring
			std::wstring wTempFileName = std::wstring_convert<std::codecvt_utf8<wchar_t>>()
				.from_bytes(m_tempFileName.c_str());

			m_sqliteDb = std::make_unique<SqliteDb>(wTempFileName);

			m_sqliteDb->execute(L"create table test ( "
				L"val_int integer null, "
				L"val_text text null, "
				L"val_real real null, "
				L"val_blob blob null "
				L")");
		}

		~SqliteDbFixture()
		{
			m_sqliteDb->execute(L"drop table test");

			m_sqliteDb.reset();
			std::remove(m_tempFileName.c_str());
		}

		std::string m_tempFileName;
		std::unique_ptr<SqliteDb> m_sqliteDb;
	};

} // namespace

BOOST_FIXTURE_TEST_CASE(testBindInt, SqliteDbFixture)
{
	m_sqliteDb->prepare(L"insert into test (val_int) values (?)")
		.addParameter(10)
		.execute();

	int val = m_sqliteDb->select(L"select val_int from test").getInt(0).value();
	BOOST_CHECK_EQUAL(10, val);
}

BOOST_FIXTURE_TEST_CASE(testBindInt64, SqliteDbFixture)
{
	long long val1 = 1LL << 60;

	m_sqliteDb->prepare(L"insert into test (val_int) values (?)")
		.addParameter(val1)
		.execute();

	long long val2 = m_sqliteDb->select(L"select val_int from test").getInt64(0).value();
	BOOST_CHECK_EQUAL(val1, val2);
}

BOOST_FIXTURE_TEST_CASE(testBindDouble, SqliteDbFixture)
{
	m_sqliteDb->prepare(L"insert into test (val_real) values (?)")
		.addParameter(10.01)
		.execute();

	double val = m_sqliteDb->select(L"select val_real from test").getDouble(0).value();
	BOOST_CHECK_EQUAL(10.01, val);
}

BOOST_FIXTURE_TEST_CASE(testBindWString, SqliteDbFixture)
{
	m_sqliteDb->prepare(L"insert into test (val_text) values (?)")
		.addParameter(L"aaa")
		.execute();

	std::wstring val = m_sqliteDb->select(L"select val_text from test").getWString(0).value();
	BOOST_CHECK(val == L"aaa");
}

BOOST_FIXTURE_TEST_CASE(testBindBlob, SqliteDbFixture)
{
	std::string sz{ "abc" };
	std::vector<unsigned char> val1;
	std::copy(sz.cbegin(), sz.cend(), std::back_inserter(val1));

	m_sqliteDb->prepare(L"insert into test (val_blob) values (?)")
		.addParameterBlob(val1.data(), val1.size())
		.execute();

	auto val2 = m_sqliteDb->select(L"select val_blob from test").getBlob(0).value();
	BOOST_CHECK(val1 == val2);
}

BOOST_AUTO_TEST_SUITE_END()
