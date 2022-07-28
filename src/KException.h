#ifndef KMTABLE_SRC_EXCEPTION_H
#define KMTABLE_SRC_EXCEPTION_H

#include <stdexcept>

/**
 * @brief std::invalid_argument with str. KM_IA_EXCEPTION("Table ~ invalid name") will
 * become std::invalid_argument("Table ~ invalid name, see logs for more information!")
 */
#define KM_IA_EXCEPTION(str) std::invalid_argument(str ", see logs for more information!")

#endif // KMTABLE_SRC_EXCEPTION_H
