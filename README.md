# Yet Another SQLite Wrapper
## Overview
Basic wrapper for libsqlite3.

Not thread-safe, not for concurrent execution. Synchronization must be guaranteed by a calling code.

A single statement can be prepared/executed at once.

## Basic usage
```
SqliteDb db(L"/tmp/test.db");

// Execute non-query statement
db.prepare(L"insert into students (name) values (?)")
  .addParameter(L"Bob")
  .execute();

// Execute non-query statement with an optional NULL value

std::optional<std::wstring> name;
auto cmd = std::move(db.prepare(L"insert into students (name) values (?)"));

if (name.has_value())
  cmd.addParameter(name.value());
else
  cmd.addParameterNull();

cmd.execute();

// Query
auto rs = db.prepare(L"select count(*) from students where name = ?")
  .addParameter(L"Bob")
  .select();
int studentCount = rs.getInt(0).value();

// Iterate over query results
for (auto rs = db.select(L"select id, name from students"); !!rs; ++rs)
{
  int id = rs.getInt(0).value();
  auto name = rs.getWString(1).value();
  // ...
}

// Transaction
auto transaction = db.beginTransaction();

// ... do some stuff

transaction.commit(); // or transaction.rollback();

```
