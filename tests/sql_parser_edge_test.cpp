#include <ess/orm/sql/parse_result.hpp>

using namespace ess::orm::sql;
using namespace ess::orm::meta;

template <FixedString SQL>
struct ParserAccepts
    : std::bool_constant<!SqlParseResult<SQL>::parse_result.has_error> {};

template <FixedString SQL>
struct ParserRejects
    : std::bool_constant<SqlParseResult<SQL>::parse_result.has_error> {};

// ======================== VALID CASES ========================

// Basic SELECT
static_assert(ParserAccepts<FixedString("SELECT * FROM goods")>::value);
static_assert(ParserAccepts<FixedString("SELECT id, title FROM goods")>::value);
static_assert(
    ParserAccepts<FixedString("SELECT * FROM goods WHERE id > 0")>::value);
static_assert(
    ParserAccepts<FixedString("SELECT * FROM goods WHERE id = ?")>::value);
static_assert(ParserAccepts<FixedString("SELECT g.id FROM goods g")>::value);

// WHERE condition operators
static_assert(
    ParserAccepts<FixedString("SELECT * FROM goods WHERE id IS NULL")>::value);
static_assert(ParserAccepts<
              FixedString("SELECT * FROM goods WHERE id IS NOT NULL")>::value);
static_assert(ParserAccepts<FixedString(
                  "SELECT * FROM goods WHERE title LIKE '%foo%'")>::value);
static_assert(ParserAccepts<
              FixedString("SELECT * FROM goods WHERE id IN (1, 2, 3)")>::value);
static_assert(ParserAccepts<FixedString(
                  "SELECT * FROM goods WHERE id BETWEEN 1 AND 10")>::value);

// AND / OR chaining
static_assert(
    ParserAccepts<FixedString(
        "SELECT * FROM goods WHERE id > 0 AND name = 'test'")>::value);
static_assert(ParserAccepts<FixedString(
                  "SELECT * FROM goods WHERE id > 0 OR name = 'test'")>::value);
static_assert(ParserAccepts<
              FixedString("SELECT * FROM goods WHERE id > 0 AND 1")>::value);
static_assert(ParserAccepts<
              FixedString("SELECT * FROM goods WHERE id > 0 AND true")>::value);
static_assert(ParserAccepts<FixedString(
                  "SELECT * FROM goods WHERE id > 0 AND false")>::value);
static_assert(ParserAccepts<
              FixedString("SELECT * FROM goods WHERE id > 0 AND NULL")>::value);

// Table alias
static_assert(ParserAccepts<
              FixedString("SELECT g.id FROM goods g WHERE g.id > 0")>::value);
static_assert(ParserAccepts<FixedString("SELECT g.id FROM goods g WHERE g.id > "
                                        "0 AND g.title = 'test'")>::value);

// Compound SELECT
static_assert(
    ParserAccepts<FixedString(
        "SELECT id FROM goods UNION SELECT cid FROM category")>::value);
static_assert(
    ParserAccepts<FixedString(
        "SELECT id FROM goods EXCEPT SELECT cid FROM category")>::value);

// WITH clause
static_assert(
    ParserAccepts<FixedString(
        "WITH cte AS (SELECT id FROM goods) SELECT * FROM cte")>::value);

// ======================== INVALID CASES ========================

// Truncated clauses
static_assert(ParserRejects<FixedString("SELECT * FROM goods WHERE")>::value);
static_assert(
    ParserRejects<FixedString("SELECT * FROM goods WHERE id > 0 AND")>::value);
static_assert(
    ParserRejects<FixedString("SELECT * FROM goods WHERE id > 0 OR")>::value);

// Incomplete comparison operator RHS
static_assert(
    ParserRejects<FixedString("SELECT * FROM goods WHERE id >")>::value);
static_assert(
    ParserRejects<FixedString("SELECT * FROM goods WHERE id =")>::value);
static_assert(
    ParserRejects<FixedString("SELECT * FROM goods WHERE id <")>::value);
static_assert(
    ParserRejects<FixedString("SELECT * FROM goods WHERE id >=")>::value);
static_assert(
    ParserRejects<FixedString("SELECT * FROM goods WHERE id <=")>::value);
static_assert(
    ParserRejects<FixedString("SELECT * FROM goods WHERE id !=")>::value);

// Consecutive values without operator
static_assert(ParserRejects<
              FixedString("SELECT * FROM goods WHERE id > 0 AND 1 1")>::value);
static_assert(ParserRejects<FixedString(
                  "SELECT * FROM goods WHERE id > 0 AND 'a' 'b'")>::value);
static_assert(ParserRejects<FixedString(
                  "SELECT * FROM goods WHERE id > 0 AND true false")>::value);
static_assert(ParserRejects<FixedString(
                  "SELECT * FROM goods WHERE id > 0 AND NULL NULL")>::value);
static_assert(ParserRejects<FixedString(
                  "SELECT * FROM goods WHERE id > 0 AND 1 'text'")>::value);

// Trailing junk after complete expression
static_assert(
    ParserRejects<FixedString("SELECT * FROM goods WHERE id > 0 1")>::value);
static_assert(
    ParserRejects<FixedString("SELECT * FROM goods WHERE id = 1 1")>::value);
static_assert(ParserRejects<FixedString(
                  "SELECT * FROM goods WHERE id > 0 AND 1 = 1 1")>::value);

// Missing operator between column and value
static_assert(
    ParserRejects<FixedString("SELECT * FROM goods WHERE id 1")>::value);
static_assert(
    ParserRejects<FixedString("SELECT * FROM goods WHERE id 'text'")>::value);
static_assert(
    ParserRejects<FixedString("SELECT * FROM goods WHERE id NULL")>::value);
static_assert(
    ParserRejects<FixedString("SELECT * FROM goods WHERE id ?")>::value);
static_assert(
    ParserRejects<FixedString("SELECT * FROM goods WHERE g.id 1")>::value);

// Missing operator between two column identifiers
static_assert(
    ParserRejects<FixedString("SELECT * FROM goods WHERE id title")>::value);
static_assert(ParserRejects<
              FixedString("SELECT * FROM goods WHERE g.id g.title")>::value);
// Missing operator between column and parenthesized expression
static_assert(
    ParserRejects<FixedString("SELECT * FROM goods WHERE id (1)")>::value);

// OFFSET without LIMIT should be rejected
static_assert(ParserRejects<FixedString(
                  "SELECT g.id FROM goods g WHERE g.id > 0 OFFSET 10")>::value);
static_assert(ParserRejects<FixedString(
                  "SELECT g.id FROM goods g ORDER BY id OFFSET 10")>::value);

// LIMIT OFFSET combination is valid
static_assert(ParserAccepts<FixedString(
                  "SELECT g.id FROM goods g LIMIT 10 OFFSET 5")>::value);
static_assert(
    ParserAccepts<FixedString("SELECT g.id FROM goods g LIMIT 10")>::value);

int main() { return 0; }