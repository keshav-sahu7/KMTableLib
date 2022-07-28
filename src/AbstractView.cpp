#include "AbstractView.hpp"
namespace km
{
    void AbstractView::setSourceTable(AbstractTable *source_table)
    {
        if (m_source_table)
            m_source_table->uninstallView(this);
        m_source_table = source_table;
        if (m_source_table)
            m_source_table->installView(this);
    }

    AbstractView::~AbstractView()
    {
        if (m_source_table)
            m_source_table->uninstallView(this);
    }
}