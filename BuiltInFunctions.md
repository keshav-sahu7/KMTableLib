# Built-In Functions Available To Use In **Formula**

There's a number of functions already defined that we can use in formula for most of our uses.
It is not loaded automatically so you must load it using km::initAllFnc() function call (one time call) before
using a formula. For example,

```cpp
//your other headers
#include <kmt/FunctionStore.hpp>

int main()
{
    km::initAllFnc();       // will load all built in functions in the function store.
    // now your code goes here!
    return 0;
}
```

Here **ALIAS** means another name which refers to same function with same params. It is for convenience.

## Contents

- [Arithmetic Functions](#arithmetic-functions)
- [Comparision Functions](#comparision-functions)
- [Date And DateTime Functions](#date-and-date-time-functions)
- [Logical Functions](#logical-functions)
- [String Functions](#string-functions)
- [Type Conversion Functions](#type-conversion-functions)
- [Some Drawbacks Of These Functions](#some-drawbacks-of-these-functions)

## Arithmetic Functions

There's a number of arithmetic functions available. Function names are self explainatory.

- addition of two numbers

  - `int32` add(`int32` value1, `int32` value2)
  - `int64` add(`int64` value1, `int64` value2)
  - `float32` add(`float32` value1, `float32` value2)
  - `float64` add(`float64` value1, `float64` value2)

- difference of two numbers. `sub is an alias for subtract`.
  - `int32` subtract(`int32` value1, `int32` value2)
  - `int64` subtract(`int64` value1, `int64` value2)
  - `float32` subtract(`float32` value1, `float32` value2)
  - `float64` subtract(`float64` value1, `float64` value2)

- multiplication of two numbers. `mul is an alias for multiply`.
  - `int32` multiply(`int32` value1, `int32` value2)
  - `int64` multiply(`int64` value1, `int64` value2)
  - `float32` multiply(`float32` value1, `float32` value2)
  - `float64` multiply(`float64` value1, `float64` value2)

- division of two values. `div is an alias for divide`.
  - `float32` divide(`int32` value1, `int32` value2)
  - `float64` divide(`int64` value1, `int64` value2)
  - `float32` divide(`float32` value1, `float32` value2)
  - `float64` divide(`float64` value1, `float64` value2)

- integer division
  - `int32` intDiv(`int32` value1, `int32` value2)
  - `int64` intDiv(`int64` value1, `int64` value2)

- modulo of two numbers
  - `int32` mod(`int32` value1, `int32` value2)
  - `int64` mod(`int64` value1, `int64` value2)
  - `float32` mod(`float32` value1, `float32` value2)
  - `float32` mod(`float32` value1, `float32` value2)

- square root of the number.
  - `float32` sqrt(`int32` value1)
  - `float64` sqrt(`int64` value1)
  - `float32` sqrt(`float32` value)
  - `float64` sqrt(`float64` value)

- exponent of the number.
  - `int32` pow(`int32` value1, `int32` value2)
  - `int64` pow(`int64` value1, `int64` value2)
  - `float32` pow(`float32` value1, `float32` value2)
  - `float64` pow(`float64` value1, `float64` value2)

- absolute (no sign) value of a number.
  - `int32` abs(`int32` value1)
  - `int64` abs(`int64` value1)
  - `float32` abs(`float32` value)
  - `float64` abs(`float64` value)

- floor of a number.
  - `float32` floor(`float32` value)
  - `float64` floor(`float64` value)

- ceil of a number.
  - `float32` ceil(`float32` value)
  - `float64` ceil(`float64` value)

- get if a number is in an inclusive range [start, end] (start <= end).
  - `boolean` isInRange(`int32` value, `int32` start, `int32` end)
  - `boolean` isInRange(`int64` value, `int64` start, `int64` end)
  - `boolean` isInRange(`float32` value, `float32` start, `float32` end)
  - `boolean` isInRange(`float64` value, `float64` start, `float64` end)
  - `boolean` isInRange(`string` value, `string` start, `string` end)
  - `boolean` isInRange(`date` value, `date` start, `date` end)
  - `boolean` isInRange(`date_time` value, `date_time` start, `date_time` end)

## Comparision Functions

All basic comparision functions are available. Function names are self explainatory.
Note : Here the word `TYPE` is used to refer the data type. It can be any type (`int32`, `int64`, `float32`, `float64`, `string`, `date`, and `date_time`) *except `boolean`*.

- Equality checking (value1 == value2).
  - `boolean` isEqual(`TYPE` value1, `TYPE` value2)

- isLess checks if wheather value1 < value2
  - `boolean` isLess(`TYPE` value1, `TYPE` value2)

- isGreater checks wheather value1 > value2
  - `boolean` isGreater(`TYPE` value1, `TYPE` value2)

- isLessOrEqual checks wheather value1 <= value2
  - `boolean` isLessOrEqual(`TYPE` value1, `TYPE` value2)

- isGreaterOrEqual checks wheather value1 >= value2
  - `boolean` isGreaterOrEqual(`TYPE` value1, `TYPE` value2)

## Date And Date Time Functions

Only a few date and date_time functions are available. Function names are self explainatory.

- get day from the `date` or `date_time`.
  - `int32` day(`date` value)
  - `int32` day(`date_time` value)

- get month from the `date` or `date_time`.
  - `int32` month(`date` value)
  - `int32` month(`date_time` value)

- get year from the `date` or `date_time`.
  - `int32` year(`date` value)
  - `int32` year(`date_time` value)

- get hour from the `date_time`.
  - `int32` hour(`date_time` value)

- get minute from the `date_time`.
  - `int32` minute(`date_time` value)

- get second from the `date_time`.
  - `int32` second(`date_time` value)

- get wheather year is leap year or not.
  - `int32` isLeapYear(`int32` year)

## Logical Functions

Basic logical functions such as AND, OR, NOT and XOR are available. It also offers a typical **if statement**.
Function names are self explanatory.

- AND of two values
  - `boolean` AND(`boolean` value1, `boolean` value2)

- OR of two values
  - `boolean` OR(`boolean` value1, `boolean` value2)

- NOT of a value
  - `boolean` NOT(`boolean` value)

- A typical if function, returns value1 if condition is true, else value2 is returned. `TYPE` can be any data type. But keep in mind that value1 and value2 must have same data type and return value will be same as the data type of second and third arguments.
  - `TYPE` IF(`boolean` cond, `TYPE` value1, `TYPE` value2)

## String Functions

There are some useful `string` functions that are used frequently. Function names are self explainatory.

- Concatenating / adding two `string`s
  - `string` concatenate(`string` str1, `string` str2)
  - `string` add(`string` str1, `string` str2)

- Check wheather str contains sub_str or not.
  - `boolean` contains(`string` str, `string` sub_str)

- Check wheather str contains any char from the chars.
  - `boolean` containsAnyOf(`string` str, `string` chars)

- Count occurence of char in str. Even if char contains multiple characters, it will only find for the first char. If char is empty then it is 0.
  - `int32` count(`string` str, `string` char)

- Get length of str.
  - `int32` length(`string` str)

- Convert str to lower case
  - `string` toLower(`string` str)
  - `string` lowerCase(`string` str)

- Convert str to upper case
  - `string` toUpper(`string` str)
  - `string` upperCase(`string` str)

## Type Conversion Functions

In many occations we may need to convert data of a data type to other data type. There are too many functions for type conversion. Function names are self explainatory.

- Converting data to `int32` data. If data is out of range of `int32`, it may produce unexpected data. In case of `boolean` False will be converted to 0, True will be converted to 1.
  - `int32` toInt32(`int64` value)
  - `int32` toInt32(`float32` value)
  - `int32` toInt32(`float64` value)
  - `int32` toInt32(`boolean` value)

- Converting `string` to `int32` data. In case of failure, first and third functions returns 0, second and fourth returns alternative value. Default base is 10. base should be in inclusive range of [2, 36].
  - `int32` toInt32(`string` value)
  - `int32` toInt32(`string` value, `int32` alternative)
  - `int32` toInt32B(`string` value, `int32` base)
  - `int32` toInt32B(`string` value, `int32` base, `int32` alternative)

- Converting data to `int64` data. If data is out of range of `int64`, it may produce unexpected data. In case of `boolean` False will be converted to 0, True will be converted to 1.
  - `int64` toInt64(`int32` value)
  - `int64` toInt64(`float32` value)
  - `int64` toInt64(`float64` value)
  - `int64` toInt64(`boolean` value)

- Converting `string` to `int64` data. In case of failure, first and third functions returns 0, second and fourth returns alternative value. Default base is 10. base should be in inclusive range of [2, 36].
  - `int64` toInt64(`string` value)
  - `int64` toInt64(`string` value, `int64` alternative)
  - `int64` toInt64B(`string` value, `int32` base)
  - `int64` toInt64B(`string` value, `int32` base, `int64` alternative)

- Converting data to `float32` data. If `string` value can not be converted then 0 or alternative will be returned according to the called function.
  - `float32` toFloat32(`int32` value)
  - `float32` toFloat32(`int64` value)
  - `float32` toFloat32(`float64` value)
  - `float32` toFloat32(`string` value)
  - `float32` toFloat32(`string` value, `float32` alternative)

- Converting data to `float64` data. If `string` value can not be converted then 0 or alternative will be returned according to the called function.
  - `float64` toFloat64(`int32` value)
  - `float64` toFloat64(`int64` value)
  - `float64` toFloat64(`float32` value)
  - `float64` toFloat64(`string` value)
  - `float64` toFloat64(`string` value, `float64` alternative)

- Converting data to `string` data. In case of `boolean`, if value is True then "True" is returned, if it is False then "False" is returned.
  - `string` toString(`int32` value)
  - `string` toString(`int64` value)
  - `string` toString(`float32` value)
  - `string` toString(`float64` value)
  - `string` toString(`boolean` value)

- Converting integer to `boolean`. If value is non zero then True is returned, if it is 0 then False is returned.
  - `boolean` toBoolean(`int32` value)
  - `boolean` toBoolean(`int64` value)

- Creating a `date` from integers.
  - `date` toDate(`int32` day, `int32` month, `int32` year)

- Creating `date_time` from integers and `date`.
  - `date_time` toDateTime(`int32` day, `int32` month, `int32` year, `int32` hour, `int32` minute, `int32` second)
  - `date_time` toDateTime(`date` date_value, `int32` hour, `int32` minute, `int32` second)

## Some Drawbacks Of These Functions

Functions have some draw backs. Major issues are listed here.

- Arithemetic functions such as addition/ subtraction/ multiplication of integers may cause of overflow or underflow and it is user's responsibility to avoid such conditions.

- Division by 0 is well defined because it is converted to floating point before division and return types are also floating point number which is already defined by the compiler. Infinity in integers are not defined so 0 is returned.

- Many basic date and date_time functions are missing.

- Integer/float to string conversions are automatic and user can not select the format.

These all issues can be solved by adding missing functions.
