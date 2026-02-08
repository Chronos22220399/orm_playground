#include <ess/orm/transaction.hpp>

namespace ess::orm {

thread_local TransactionContextManager TransactionContextManager::instance;

}
