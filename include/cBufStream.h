/*
 * Copyright (C) 2010 Ajax.org BV
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this library; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

namespace o3 {

struct cBufStream : cStreamBase {
    Buf m_buf;
    size_t m_pos;

	cBufStream()
	{

	}

    cBufStream(Buf& buf)
        : m_pos(0)
    {
        m_buf = buf;
    }

    virtual ~cBufStream()
    {
    }

    o3_begin_class(cStreamBase)
    o3_end_class()

    bool eof()
    {
        return m_pos == m_buf.size();
    }

    bool error()
    {
        return false;
    }

    size_t pos()
    {
        return m_pos;
    }

    size_t setPos(size_t pos)
    {        
        if (pos > m_buf.size())
            return m_pos;

        return m_pos = pos;
    }

    size_t read(void* ptr, size_t size)
    {
        size_t left = m_buf.size() - m_pos;
        size_t to_read = min(left,size);
        memCopy(ptr, ((uint8_t*)m_buf.ptr()) + m_pos, to_read);
        m_pos += to_read;
        return to_read;
    }

    size_t write(const void* ptr, size_t size)
    {
        size_t left = m_buf.size() - m_pos;
        size_t to_del = min(left,size);
        m_buf.remove(m_pos,to_del);
        m_buf.insert(m_pos, ptr, size);
        m_pos += size;
        return size;
    }

    bool flush()
    {
        return true;
    }

    bool close()
    {
        return true;
    }

    size_t size()
    {
        return m_buf.size();
    }

    void* unwrap()
    {
        return 0;
    }

	Buf buf()
	{
		return m_buf;
	}
};

} // namespace o3
