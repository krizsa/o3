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
#ifndef J_C_XML_ELEMENT_H
#define J_C_XML_ELEMENT_H


namespace o3 {
    struct cFastXmlElement : cFastXmlNode, iXmlElement {
        cFastXmlElement(iXmlDocument* doc, xml_node node, xml_attribute attr) 
            : cFastXmlNode(doc,node,attr) 
		{
            o3_trace3 trace;
        }

		virtual ~cFastXmlElement()
		{

		}

        o3_begin_class(cFastXmlNode)
            o3_add_iface(iXmlElement)
        o3_end_class();

        o3_glue_gen()
        
        o3_fun siXmlNode selectSingleNode(const char* selection, iScr* ctxt = 0)
		{
			xpath_node selected = m_node.select_single_node(selection);
            if (selected.attribute())
				return wrapXmlNode(ownerDocument(), selected.node());
			return wrapXmlNode(ownerDocument(), selected.node(), selected.attribute());
        }

        o3_fun siXmlNodeSet selectNodes(const char* selection, iScr* ctxt = 0)
		{
            o3_trace3 trace;
			xpath_node_set selected = m_node.select_nodes(selection);
			if (selected.size()>0)
				return o3_new(cFastXmlNodeSet)(ownerDocument(),selected);
			return siXmlNodeSet();
        }
        
        virtual o3_get Str tagName() 
		{
            o3_trace3 trace;
			return m_node.name();
        }
        
        virtual o3_fun Str getAttribute(const char* name) 
		{
			o3_trace3 trace;
			o3_assert(name);
			siXmlAttr attr = getAttributeNode(name);
			if (attr)
				return attr->value();
			else
				return Str();
        }
        
        virtual o3_fun void setAttribute(const char* name, const char* value) 
		{
            o3_trace3 trace;
            o3_assert(name);
            o3_assert(value);
			if (siXmlAttr attr = getAttributeNode(name)) {
				attr->setValue(value);
				return;
			}
			m_node.append_attribute(name).set_value(value);
        }
        
        virtual o3_fun void removeAttribute(const char* name) 
		{
            o3_trace3 trace;
            o3_assert(name);
            removeAttributeNode(getAttributeNode(name));
        }
        
        o3_fun siXmlAttr getAttributeNode(const char* name) 
		{
            o3_trace3 trace;
            o3_assert(name);
			xml_attribute attr = m_node.attribute(name);
			return attr ? siXmlAttr(wrapXmlNode(ownerDocument(), m_node, attr)) : siXmlAttr();
        }
        
        o3_fun siXmlAttr setAttributeNode(iXmlAttr* new_attr, siEx* ex = 0) 
		{
            o3_trace3 trace;           
			if (!new_attr)
				return siXmlAttr();
			if (siXmlAttr a = getAttributeNode(new_attr->name())) {
				return replaceChild(siXmlNode(siXmlAttr(new_attr)),siXmlNode(a),ex);
			}
            
			return new_attr ? appendChild(siXmlNode(siXmlAttr(new_attr)),ex) : siXmlNode();
        }
        
        o3_fun siXmlAttr removeAttributeNode(iXmlAttr* old_attr)
		{
            o3_trace3 trace;                
            return old_attr ? removeChild(siXmlNode(old_attr)) : siXmlNode();
        }
        
		o3_fun siXmlNodeList getElementsByTagName(const char* name) 
		{
            o3_trace3 trace;
            o3_assert(name);
            return o3_new(cFastXmlNodeList)(ownerDocument(), this, name);
		}       
        
        o3_fun void normalize() 
		{
            o3_trace3 trace;
			siXmlNode child = firstChild();
			while (child != NULL) {
				if (child->nodeType() == TYPE_TEXT) {
					Str value;

					do {
						siXmlNode sibling = child->nextSibling(); 

						value += child->nodeValue();
						removeChild(child);
						child = sibling;
					} while (child != NULL && child->nodeType() == TYPE_TEXT);
					if (!value.empty()) {
						siXmlText text = ownerDocument()->createTextNode(value); 

						insertBefore(siXmlNode(text).ptr(), child);
					}
				} else {
					if (siXmlElement element = child) 
						element->normalize();
					child = child->nextSibling();
				} 
			}
        }
        
    };
}

#endif // J_C_XML_ELEMENT_H
