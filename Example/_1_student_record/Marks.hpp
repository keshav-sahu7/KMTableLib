#ifndef KMTABLE_EXAMPLE_MARKS_HPP
#define KMTABLE_EXAMPLE_MARKS_HPP

#include <kmt/Types.hpp>

class Marks
{
public:
    Marks(int math_marks, int physics_marks, int chemistry_makrs);
    km::KInt32 math() const;
    km::KInt32 physics() const;
    km::KInt32 chemistry() const;

private:
    km::KInt32 m_math;
    km::KInt32 m_physics;
    km::KInt32 m_chemistry;
};

inline Marks::Marks(km::KInt32 math_marks, km::KInt32 physics_marks, km::KInt32 chemistry_marks) : m_math(math_marks > 50 ? 0 : math_marks),
                                                                              m_physics(physics_marks > 50 ? 0 : physics_marks),
                                                                              m_chemistry(chemistry_marks > 50 ? 0 : chemistry_marks)
{
}

inline km::KInt32 Marks::math() const
{
    return m_math;
}

inline km::KInt32 Marks::physics() const
{
    return m_physics;
}

inline km::KInt32 Marks::chemistry() const
{
    return m_chemistry;
}

#endif // KMTABLE_EXAMPLE_MARKS_HPP