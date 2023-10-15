#include <string>
#include <cstdio>
#include <codecvt>
#include "SqliteDb.h"

#define BOOST_TEST_MODULE testSqliteDb
#include <boost/test/included/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(testSuiteSqliteDb)

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
		}

		~SqliteDbFixture()
		{
			m_sqliteDb.reset();
			std::remove(m_tempFileName.c_str());
		}

		std::string m_tempFileName;
		std::unique_ptr<SqliteDb> m_sqliteDb;
	};

} // namespace

BOOST_FIXTURE_TEST_CASE(testSpacesAtTheEndOfStatement, SqliteDbFixture)
{
	m_sqliteDb->execute(L"create table dummy ( id integer primary key, name text not null )               ");
	m_sqliteDb->execute(L"drop table dummy                  ");
}

BOOST_FIXTURE_TEST_CASE(testDdl, SqliteDbFixture)
{
	m_sqliteDb->execute(L"create table dummy ( id integer primary key, name text not null )");
	m_sqliteDb->execute(L"drop table dummy");
}

BOOST_FIXTURE_TEST_CASE(testRecordsetIteration, SqliteDbFixture)
{
	m_sqliteDb->execute(L"create table products ( id integer primary key, name text not null )");

	auto sql = L"insert into products (name) values (?)";

	m_sqliteDb->prepare(sql)
		.addParameter(L"bread")
		.execute();

	m_sqliteDb->prepare(sql)
		.addParameter(L"butter")
		.execute();

	m_sqliteDb->prepare(sql)
		.addParameter(L"milk")
		.execute();

	int count = 0;
	for (auto rs = m_sqliteDb->select(L"select id, name from products"); rs; ++rs, ++count);

	BOOST_CHECK_EQUAL(3, count);

	m_sqliteDb->execute(L"drop table products");
}

BOOST_FIXTURE_TEST_CASE(testMultipleStatementsFail, SqliteDbFixture)
{
	m_sqliteDb->execute(L"create table products ( id integer primary key, name text not null )");

	auto sql = L"insert into products (name) values ('test'); "
		"select max(last_insert_rowid()) from products";

	BOOST_CHECK_THROW(m_sqliteDb->prepare(sql).execute(), MultipleStatementsUnsupportedError);

	m_sqliteDb->execute(L"drop table products");
}

BOOST_FIXTURE_TEST_CASE(testDmlEmptyRecordSet, SqliteDbFixture)
{
	m_sqliteDb->execute(L"create table products ( id integer primary key, name text not null )");

	auto sqlSelect = L"select max(last_insert_rowid()) from products";

	// Test empty recordset
	auto rs = m_sqliteDb->select(sqlSelect);

	m_sqliteDb->execute(L"drop table products");
}

BOOST_FIXTURE_TEST_CASE(testDmlPreparedCommands, SqliteDbFixture)
{
	m_sqliteDb->execute(L"create table products ( id integer primary key, name text not null )");

	auto sqlSelect = L"select max(last_insert_rowid()) from products";

	// Test empty recordset
	{
		auto rs = m_sqliteDb->select(sqlSelect);
	}

	auto sqlInsert = L"insert into products (name) values (?)";

	m_sqliteDb->prepare(sqlInsert)
		.addParameter(L"bread")
		.execute();

	int idBread = m_sqliteDb->select(sqlSelect)
		.getInt(0)
		.value();

	m_sqliteDb->prepare(sqlInsert)
		.addParameter(L"butter")
		.execute();

	m_sqliteDb->prepare(sqlInsert)
		.addParameter(L"milk")
		.execute();

	int idMilk = m_sqliteDb->select(sqlSelect)
		.getInt(0)
		.value();

	// Check IDs

	int idBread2 = m_sqliteDb->select(L"select id from products where name='bread'")
		.getInt(0)
		.value();
	BOOST_CHECK_EQUAL(idBread, idBread2);

	int idMilk2 = m_sqliteDb->select(L"select id from products where name='milk'")
		.getInt(0)
		.value();
	BOOST_CHECK_EQUAL(idMilk, idMilk2);

	m_sqliteDb->execute(L"drop table products");
}

BOOST_FIXTURE_TEST_CASE(testTransaction, SqliteDbFixture)
{
	m_sqliteDb->execute(L"create table products ( id integer primary key, name text not null )");

	auto sql = L"insert into products (name) values (?)";

	m_sqliteDb->prepare(sql)
		.addParameter(L"bread")
		.execute();

	auto transaction = m_sqliteDb->beginTransaction();

	m_sqliteDb->prepare(sql)
		.addParameter(L"butter")
		.execute();

	m_sqliteDb->prepare(sql)
		.addParameter(L"milk")
		.execute();

	transaction.rollback();

	// Check record count
	int recCount = m_sqliteDb->select(L"select count(*) from products")
		.getInt(0)
		.value();
	BOOST_CHECK_EQUAL(recCount, 1);

	m_sqliteDb->execute(L"drop table products");
}

BOOST_AUTO_TEST_SUITE_END()
