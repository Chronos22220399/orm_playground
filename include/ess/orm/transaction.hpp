#pragma once
#include <ess/orm/context.hpp>

namespace ess::orm {

template <concepts::database_type DB = config::DefaultDB> class Transaction {};

} // namespace ess::orm
