## 1. class TransactionContext

### params:

#### private:

- m_conn: operate the database in a transaction
  > type: shared_ptr<Connection>
- m_thread_id: tag which thread the transaction belongs to
  > type: thread::id
- m_db_type: tag the database type (such as MainDB, LogDB...)
  > type: type_index
- m_nesting_level: [unfin] record the level
  > type: int

### funcs:

#### public:

- TransactionContext()
- verify_thread()

## 2. class TransactionContextManager

### params:

#### private:

- m_contexts: store the context which belongs to a database type

  > type: unordered_map<type_index, TransactionContext>

#### public:

- instance: a instance of Manager to offer unified interface

  > type: static thread_local TransactionContextManager

### func:

- set(TransactionContext ctx): set the context for database type DB
- clear(): clear the context for database type DB
- get_default(): get the default database's transaction context pointer
- in_any_transaction(): check whether the current query in any transaction

## 3. class TransactionGuard

### params:

#### private:

- m_prev: the previous transaction context for every database type, used to
  store the context to restore
  > type: TransactionContext

### funcs:

#### public:

- TransactionGuard()
- ~TransactionGuard()

## 4. class Context

### params:

#### private:

- m_pools: a mapper that map a database type to a connection pool
  > type: unordered_map<type_index, std::unique_ptr<ConnectionPool>>

### funcs:

#### private:

- Context(): default constructor

#### public:

- register_db(connection_url, pool_size): register a connection pool for database type
- pool(): get a pool instance for given database type
- init(): init the context more flexible, rather than until first call the instance.

## 5. class Transaction

### params:

#### private:

- m_conn: a connection of registered db
