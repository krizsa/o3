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
#ifndef J_C_XML_NAMED_NODE_MAP_H
#define J_C_XML_NAMED_NODE_MAP_H

namespace o3 {
    struct cFastXmlNamedNodeMap : cScr, iXmlNamedNodeMap {
		cFastXmlNamedNodeMap(iXmlElement* elem) {
			o3_assert(elem);
			m_elem = elem;            
		}

		virtual ~cFastXmlNamedNodeMap()
		{

		}

		o3_begin_class(cScr)
			o3_add_iface(iXmlNamedNodeMap)
			o3_end_class();

		o3_glue_gen()

		siXmlElement    m_elem;
		
		o3_fun bool __query__(int idx) {
				o3_trace3 trace;
				return (idx < length());
		} 

		o3_fun bool __deleter__(int , siEx* ex = 0) {
			o3_trace3 trace; ex;
			return false;
		} 

		o3_fun siXmlNode __getter__(int idx, siEx* ex = 0) {
			ex;
			o3_trace3 trace;
			if (idx >= length() || idx < 0) {                
				//o3_set_ex(ex_invalid_value); 

				return siXmlNode();
			}
			return item(idx);
		}

		virtual o3_fun siXmlNode getNamedItem(const char* name) {
			o3_trace3 trace;
			return m_elem->getAttributeNode(name);
		}

		virtual o3_set siXmlNode setNamedItem(iXmlNode* arg) {
			o3_trace3 trace;
			return m_elem->setAttributeNode(siXmlAttr(arg));
		}

		virtual o3_fun siXmlNode removeNamedItem(const char* name) {
			o3_trace3 trace;
			return m_elem->removeAttributeNode(m_elem->getAttributeNode(name));
		}

		virtual siXmlNode item(int index) {
			o3_trace3 trace;
			siXmlNode elem(m_elem);
			xml_attribute attr = elem->node().first_attribute();
			int l = 0;
			while(attr && index!=l) {
				attr = attr.next_attribute();    
				l++;
			}            
			return wrapXmlNode(elem->ownerDocument(),elem->node(),attr);
		}

		virtual o3_get int length() {
			o3_trace3 trace;
			if (!m_elem) return 0;
			int l = 0;
			siXmlNode elem(m_elem);
			xml_attribute attr = elem->node().first_attribute();
			while(attr) {
				l++;
				attr = attr.next_attribute();
			}
			return l;
		}
    };
}

#endif // J_C_XML_NAMED_NODE_MAP_H
