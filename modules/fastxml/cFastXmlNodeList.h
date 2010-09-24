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
#ifndef J_C_XML_NODE_LIST_H
#define J_C_XML_NODE_LIST_H


namespace o3 {
    struct cFastXmlNodeList : cScr, iXmlNodeList {

        cFastXmlNodeList(iXmlDocument* doc, iXmlNode* parent, const char* name = 0) 
			: m_doc(doc), m_parent(parent), m_name(name)
		{
			m_query = name != 0;
        }

		virtual ~cFastXmlNodeList()
		{

		}

        o3_begin_class(cScr)
            o3_add_iface(iScr)
            o3_add_iface(iXmlNodeList)
        o3_end_class();

        o3_glue_gen()

		bool m_query;
		siXmlNode m_parent;
		Str m_name;
		siXmlDocument	m_doc;

        o3_fun bool __query__(iCtx* ctx, int idx)
		{
            o3_trace3 trace;
            return (idx < length(ctx) && idx>=0);
        } 
        
        o3_fun bool __deleter__(int , siEx* ex = 0) 
		{
            o3_trace3 trace; ex;
            return false;
        } 
        
        o3_fun siXmlNode __getter__(iCtx* ctx, int idx, siEx* ex = 0) 
		{
            o3_trace3 trace; ex;
            if (idx >= length(ctx) || idx < 0)
                return siXmlNode();
            return item(idx);
        }
        
        virtual siXmlNode item(int index) 
		{
            o3_trace3 trace;
			siXmlNode item;

			if (m_query) {
				item = queryItem(m_parent, m_name.ptr(), index);
			} else {
				item = m_parent->firstChild();            
				while (index-- > 0)
					item = item->nextSibling();
			}
			return item;
        }

        virtual o3_get int length(iCtx* ctx) 
		{
            o3_trace3 trace;

			int length = 0;
			siXmlNode child = m_parent->firstChild();

			while (child) {
				length += m_query ? queryLength(child, m_name) : 1;
				child = child->nextSibling();
			}
			return length;
		}

		siXmlNode queryItem(iXmlNode* parent, const char* name,
			int& index) 
		{
			o3_trace3 trace;
			siXmlNode child = parent->firstChild();

			while (child) {
				siXmlNode item = queryItem( child, name, index);
				if (item)
					return item;
				child = child->nextSibling();
			}

			siXmlElement element(parent);

			if (element && strEquals(element->tagName().ptr(), name)
				&& index-- == 0)
				return element;
			return siXmlNode();
		}

		int queryLength(iXmlNode* parent, const char* name) 
		{
			o3_trace3 trace;
			int length = 0;
			siXmlNode child = parent->firstChild();

			while (child) {
				length += queryLength(child, name);
				child = child->nextSibling();
			}

			siXmlElement element(parent);

			if (element && strEquals(element->tagName().ptr(), name))
				++length;
			return length;
		}
    };
}

#endif // J_C_XML_NODE_LIST_H
