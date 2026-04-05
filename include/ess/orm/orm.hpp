#pragma once

// ess_orm - Main include header
// Include this header to get all public API of ess_orm library

// Core ORM functionality
#include <ess/orm/core/conn_factory.hpp>
#include <ess/orm/core/connection.hpp>
#include <ess/orm/core/connection_pool.hpp>
#include <ess/orm/core/context.hpp>
#include <ess/orm/core/dialect.hpp>
#include <ess/orm/core/impl.hpp>
#include <ess/orm/core/result.hpp>
#include <ess/orm/core/result_set_mapper.hpp>
#include <ess/orm/core/row.hpp>
#include <ess/orm/core/runtime.hpp>
#include <ess/orm/core/statement.hpp>
#include <ess/orm/core/transaction.hpp>

// Configuration system
#include <ess/orm/config/config.hpp>
#include <ess/orm/config/default.hpp>
#include <ess/orm/config/traits.hpp>

// Common utilities
#include <ess/orm/common/concept.hpp>
#include <ess/orm/common/defines.hpp>
#include <ess/orm/common/error.hpp>
#include <ess/orm/common/meta.hpp>

// DSL (Domain Specific Language)
#include <ess/orm/dsl/attribute.hpp>
#include <ess/orm/dsl/dsl.hpp>
#include <ess/orm/dsl/traits.hpp>

// SQL processing
#include <ess/orm/sql/common.hpp>
#include <ess/orm/sql/lexer.hpp>
#include <ess/orm/sql/parser.hpp>
#include <ess/orm/sql/token.hpp>
#include <ess/orm/sql/validator.hpp>

// Version information
#include <ess/orm/version.hpp>

// Third-party dependencies (embedded)
#include <ess/orm/thirdparty/expected.hpp>

// Note: Internal headers are not included as they are not part of public API
// Note: Test headers are not included as they are for testing only
