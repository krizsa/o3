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
#ifndef J_C_XML_NODE_ARRAY_H
#define J_C_XML_NODE_ARRAY_H

namespace o3 {
    struct cFastXmlNodeSet : cScr, iXmlNodeSet {
        cFastXmlNodeSet(iXmlDocument* doc, xpath_node_set set) 
			: m_doc(doc), m_set(set)
		{
			o3_trace3 trace;
        }

        virtual ~cFastXmlNodeSet() 
		{
            o3_trace3 trace;
        }


        o3_begin_class(cScr)
            o3_add_iface(iXmlNodeSet)
        o3_end_class();

        o3_glue_gen()

		siXmlDocument	m_doc;
		xpath_node_set	m_set;

        o3_fun bool __query__(int idx) 
		{
            o3_trace3 trace;
			if (idx<0 || idx>=m_set.size())
				return false;
			return true;
        } 
        
        o3_fun siXmlNode __getter__(iCtx* ctx, int idx, siEx* ex = 0) 
		{
            ex;
            o3_trace3 trace;
            return item(idx);
        }

        virtual siXmlNode item(int index) 
		{
            o3_trace3 trace;
			if (index<0 || index>=m_set.size())
				return siXmlNode();
			xpath_node ref = m_set[index];
			if (ref.attribute())
				return wrapXmlNode(m_doc, ref.parent(), ref.attribute());
			
			return wrapXmlNode(m_doc, ref.node());
        }
        
        virtual o3_get int length() 
		{
            o3_trace3 trace;
            return m_set.size();           
        }

    };
}

#endif // J_C_XML_NODE_ARRAY_H
