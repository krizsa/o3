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
#ifndef J_XML_CHARACTER_DATA_H
#define J_XML_CHARACTER_DATA_H

namespace o3 {
	struct cFastXmlCharacterData : cFastXmlNode, iXmlCharacterData {
        cFastXmlCharacterData(iXmlDocument* doc, xml_node node, xml_attribute attr) 
            : cFastXmlNode(doc,node,attr) 
		{
                o3_trace3 trace;
        }

		virtual ~cFastXmlCharacterData()
		{

		}

		o3_begin_class(cFastXmlNode)
			o3_add_iface(iXmlCharacterData)
		o3_end_class();

        o3_glue_gen()

        virtual o3_get Str data() 
		{
            o3_trace3 trace;
            return m_node.value();
		}

        virtual o3_set void setData(const char* data) 
		{
            o3_trace3 trace;
            o3_assert(data);
			m_node.set_value(data);
		}

        virtual o3_get int length() 
		{
            o3_trace3 trace;
			const char_t* v = m_node.value();
			return v ? strLen(v) : 0;
		}

        virtual o3_fun Str substringData(int offset, int count, siEx* ex) 
		{
			o3_trace3 trace;
			if (offset + count > length() || offset < 0 || count < 0) {
				//o3_set_ex("Invalid offset and/or count");                
				return Str();
			}

			Str data = this->data();
			return Str(data + offset, (size_t) count);

		}

        virtual o3_fun void appendData(const char* arg)
		{
            o3_trace3 trace;
            o3_assert(arg);
			Str data = this->data();

			data.append(arg);
			setData(data);
		}
        
        virtual o3_fun void insertData(int offset, const char* arg, siEx* ex = 0) 
		{
			ex;
			o3_trace3 trace;
			o3_assert(arg);

			if (offset >= length() || offset < 0) {
				//o3_set_ex("Invalid offset");
				return;
			}
			Str data = this->data();

			data.insert(offset, arg);
			setData(data);
		}
        
		virtual o3_fun void deleteData(int offset, int count, siEx* ex = 0)
		{
			ex;
			o3_trace3 trace;
			if (offset + count > length() || offset < 0 || count < 0) {
				//o3_set_ex("Invalid offset and/or count");
				return;
			}

			Str data = this->data();
			//Str::Iter first = data.begin() + offset;

			data.remove(offset, count);
			setData(data);
		}
        
		virtual o3_fun void replaceData(int offset, int count, const char* arg, siEx* ex = 0)
		{
			ex;
			o3_trace3 trace;            
			o3_assert(arg);
			if (offset + count > length()) {
				//o3_set_ex("Invalid offset and/or count");
				return;
			}
			deleteData(offset, count);
			insertData(offset, arg);
		}

	};
}

#endif // J_XML_CHARACTER_DATA_H
