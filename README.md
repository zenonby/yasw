# Yet Another SQLite Wrapper
## Overview
Basic wrapper for libsqlite3. Not thread-safe, not for concurent execution. Synchronization must be guaranteed by a calling code.
Only single statements can be prepared/executed at once.

## Basic usage
```
SqliteDb db(L"/tmp/test.db");

// Execute non-query statement
db.prepare(L"insert into students (name) values (?)")
  .addParameter(L"Bob")
  .execute();

// Query
auto rs = db.prepare(L"select count(*) from students where name = ?")
  .addParameter(L"Bob")
  .select();
int studentCount = rs.getInt(0).value();

// Transaction
auto transaction = db.createTransaction();

// ... do some stuff

transaction.commit(); // or transaction.rollback();

```
