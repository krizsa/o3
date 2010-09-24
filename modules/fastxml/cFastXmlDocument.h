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
#ifndef J_C_XML_DOCUMENT_H
#define J_C_XML_DOCUMENT_H
         

namespace o3 {
    struct cFastXmlDocument : cFastXmlNode, iXmlDocument {   
        cFastXmlDocument() 
			: cFastXmlNode(0,xml_node(),xml_attribute()) 
		{            
			m_node = m_root;		
		}

        virtual ~cFastXmlDocument()
        {

        }

        o3_begin_class(cFastXmlNode)
            o3_add_iface(iXmlDocument)
        o3_end_class();
    
        o3_glue_gen()

		xml_document m_root;
            
        virtual o3_get siXmlElement documentElement() 
		{
            o3_trace3 trace;
            return siXmlElement(firstChild());
        }
        
        virtual o3_fun siXmlElement createElement(const char* tagName) 
		{
            o3_trace3 trace;
            o3_assert(tagName);
			// in pugi we can just append create nodes
			xml_node node = m_root.append_child(node_element);
			node.set_name(tagName);
			// let's remove it so it will become an orphan node (modified part of pugi)
			m_root.remove_child(node);
			return siXmlElement(wrapXmlNode(this,node));
        }
        
		virtual o3_fun siXmlText createTextNode(const char* data) 
		{
            o3_trace3 trace;
            o3_assert(data);
			xml_node node = m_root.append_child(node_pcdata);
			node.set_value(data);
			m_root.remove_child(node);
			return siXmlText(wrapXmlNode(this,node));

		}

		virtual o3_fun siXmlComment createComment(const char* data) 
		{
            o3_trace3 trace;
            o3_assert(data);

			xml_node node = m_root.append_child(node_comment);
			node.set_value(data);
			m_root.remove_child(node);
			return siXmlComment(wrapXmlNode(this,node));
		}
        
		virtual o3_fun siXmlCDATASection createCDATASection(const char* data) 
		{
            o3_trace3 trace;
            o3_assert(data);

			xml_node node = m_root.append_child(node_cdata);
			node.set_value(data);
			m_root.remove_child(node);
			return siXmlCDATASection(wrapXmlNode(this,node));		
		}
        
        virtual o3_fun siXmlAttr createAttribute(const char* name) 
		{
            o3_trace3 trace;
            o3_assert(name);

			xml_node tmp = m_root.append_child();
			xml_attribute attr = tmp.append_attribute(name);
			tmp.remove_attribute(attr);
			m_root.remove_child(tmp);
			return siXmlAttr(wrapXmlNode(this,xml_node(),attr));		
		}

		//virtual o3_fun siXmlNodeList getElementsByTagName(const char* tagName) 
		//{
  //          o3_trace3 trace;
  //          o3_assert(tagName);
		//	return siXmlNodeList();
		//}


		o3_fun bool setProperty(const Str& prop, const Str& val)
		{
			return true;
		}

		o3_fun siXmlNode importNode(iXmlNode* externalNode, bool deep)
		{
			if (!externalNode)
				return siXmlNode();

			cFastXmlNode* ext = (cFastXmlNode*) externalNode;
			if (!ext->m_node && !ext->m_attr 
				|| ext->m_node.type() == node_document) 
				return siXmlNode();

			if (ext->m_attr) {
				xml_attribute attr = m_root.append_copy(ext->m_attr);
				m_root.remove_attribute(attr);
				return wrapXmlNode(this, xml_node(), attr);
			}
			else {
				xml_node node = m_root.append_copy(ext->m_node);
				m_root.remove_child(node);
				return wrapXmlNode(ownerDocument(), node);
			}
		}

		o3_fun siXmlNode getElementById(const Str& id) 
		{
			xpath_node node = m_root.select_single_node(Str("descendant-or-self::node()[@id='") + id + "']");	
			return node.node() ? wrapXmlNode(ownerDocument(), node.node()) : siXmlNode();
		}

		o3_fun siXmlNodeList getElementsByTagName(const char* name) 
		{
			o3_trace3 trace;
			o3_assert(name);
			siXmlElement e = documentElement();
			return e ? e->getElementsByTagName(name) : siXmlNodeList();
		}   

		xml_document& doc() 
		{
			return m_root;
		}

    };
}

#endif // J_C_XML_DOCUMENT_H
