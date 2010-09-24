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
#ifndef J_C_XML_NAMED_NODE_MAP2_H
#define J_C_XML_NAMED_NODE_MAP2_H

namespace o3 {
    struct cXmlNamedNodeMap : cScr, iXmlNamedNodeMap2 {
        cXmlNamedNodeMap(iXmlElement2* elem) {
            o3_assert(elem);
            m_elem = elem;            
        }
       
		virtual ~cXmlNamedNodeMap()
		{

		}

        o3_begin_class(cScr)
            o3_add_iface(iXmlNamedNodeMap2)
        o3_end_class();

        o3_glue_gen()

        o3_fun bool __query__(int idx) {
            o3_trace3 trace;
            return (idx < length());
        } 
        
        o3_fun bool __deleter__(int , siEx* ex = 0) {
            o3_trace3 trace; ex;
            return false;
        } 
        
        o3_fun siXmlNode2 __getter__(iCtx* ctx, int idx, siEx* ex = 0) {
            ex;
            o3_trace3 trace;
            if (idx >= length() || idx < 0) {                
                    //o3_set_ex(ex_invalid_value); 
                
                return siXmlNode2();
            }
            return item(ctx, idx);
        }
        
		virtual o3_fun siXmlNode2 getNamedItem(iCtx* ctx, const char* name) {
            o3_trace3 trace;
			return m_elem->getAttributeNode(ctx, name);
		}
        
		virtual o3_set siXmlNode2 setNamedItem(iCtx* ctx, iXmlNode2* arg) {
            o3_trace3 trace;
			return m_elem->setAttributeNode(ctx, siXmlAttr2(arg));
		}
        
		virtual o3_fun siXmlNode2 removeNamedItem(iCtx* ctx, const char* name) {
            o3_trace3 trace;
			return m_elem->removeAttributeNode(m_elem->getAttributeNode(ctx, name));
		}

        virtual siXmlNode2 item(iCtx* ctx, int index) {
            o3_trace3 trace;
            xmlAttrPtr attr = m_elem->firstAttr();
            int l = 0;
            while(attr && index!=l) {
                attr = attr->next;    
                l++;
            }            
            siXmlNode2 owner = siXmlNode2(m_elem)->ownerNode();
            return wrapNode(ctx, (xmlNodePtr)attr, owner ? owner : siXmlNode2(m_elem));
		}
        
		virtual o3_get int length() {
            o3_trace3 trace;
            if (!m_elem) return 0;
            int l = 0;
            xmlAttrPtr attr = m_elem->firstAttr();
            while(attr) {
                l++;
                attr = attr->next;
            }
            return l;
		}

        siXmlElement2    m_elem;
    };
}

#endif // J_C_XML_NAMED_NODE_MAP_H
